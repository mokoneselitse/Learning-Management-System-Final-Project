// Integration tests for change request CR1 (v1.1): class-wide deadline extensions,
// shown on the student's assignment list.
#include <catch2/catch_test_macros.hpp>

#include "Errors.h"
#include "LmsFixture.h"

TEST_CASE_METHOD(LmsFixture,
                 "IT-23 Class extension rules: own course, later date, reason, before the deadline",
                 "[integration][CR1][FR24]") {
    int a = createAssignment();  // deadline = testDeadline(), now = 2 days before
    Session lec = login("lec1");
    Session other = login("lec2");
    Session stu = login("stu1");
    Timestamp later = testDeadline() + hours(48);

    CHECK_THROWS_AS(assignmentService.grantClassExtension(stu, a, later, "x"), AccessDeniedError);
    CHECK_THROWS_AS(assignmentService.grantClassExtension(other, a, later, "x"),
                    AccessDeniedError);
    CHECK_THROWS_AS(assignmentService.grantClassExtension(lec, 999, later, "x"), NotFoundError);
    CHECK_THROWS_AS(assignmentService.grantClassExtension(lec, a, testDeadline(), "same"),
                    ValidationError);  // must be later than the original
    CHECK_THROWS_AS(assignmentService.grantClassExtension(lec, a, later, "  "), ValidationError);

    assignmentService.grantClassExtension(lec, a, later, "Public holiday");
    REQUIRE(assignments.findClassExtension(a).has_value());
    CHECK(assignments.findClassExtension(a)->getNewDeadline() == later);

    // granting again replaces the first one (one per assignment)
    assignmentService.grantClassExtension(lec, a, later + hours(24), "Power cut");
    CHECK(assignments.findClassExtension(a)->getNewDeadline() == later + hours(24));
    CHECK(assignments.findClassExtension(a)->getReason() == "Power cut");

    // not allowed once the original deadline has passed (no LATE status to undo)
    clock.set(testDeadline());
    CHECK_THROWS_AS(assignmentService.grantClassExtension(lec, a, later, "too late"),
                    ValidationError);
}

TEST_CASE_METHOD(LmsFixture,
                 "IT-24 Every enrolled student can submit on time until the class deadline",
                 "[integration][CR1][FR16][FR24]") {
    int a = createAssignment();
    Session lec = login("lec1");
    assignmentService.grantClassExtension(lec, a, testDeadline() + hours(48), "Exam week");

    clock.set(testDeadline() + hours(24));  // after the original, before the class deadline
    Session s1 = login("stu1");
    CHECK(submissionService.submit(s1, a, workFile).getStatus() == SubmissionStatus::SUBMITTED);
    CHECK(submissionService.submit(s1, a, workFile).getAttemptNo() == 2);  // resubmit still OK

    clock.set(testDeadline() + hours(48) + seconds(1));  // 1 s after the class deadline
    Session s2 = login("stu2");
    Submission late = submissionService.submit(s2, a, workFile);
    CHECK(late.getStatus() == SubmissionStatus::LATE);
    CHECK(late.getDaysLate() == 1);  // counted from the class deadline, not the original
}

TEST_CASE_METHOD(LmsFixture, "IT-25 The student's list shows which deadline applies and why",
                 "[integration][CR1][FR15]") {
    int a = createAssignment();
    Session lec = login("lec1");
    Session s1 = login("stu1");
    Session s2 = login("stu2");

    DeadlineInfo d = assignmentService.deadlineFor(s1, a);
    CHECK(d.effective == testDeadline());
    CHECK(d.extendedBy.empty());

    assignmentService.grantClassExtension(lec, a, testDeadline() + hours(24), "Exam week");
    assignmentService.grantExtension(lec, a, studentId, testDeadline() + hours(72), "Sick note");

    d = assignmentService.deadlineFor(s1, a);  // own extension is later
    CHECK(d.original == testDeadline());
    CHECK(d.effective == testDeadline() + hours(72));
    CHECK(d.extendedBy == "individual");

    d = assignmentService.deadlineFor(s2, a);  // only the class extension
    CHECK(d.effective == testDeadline() + hours(24));
    CHECK(d.extendedBy == "class");

    CHECK_THROWS_AS(assignmentService.deadlineFor(lec, a), AccessDeniedError);
}

TEST_CASE("IT-26 A v1.0 database is upgraded automatically and keeps its data",
          "[integration][CR1][persistence]") {
    fs::path folder = fs::temp_directory_path() / "lms_test_upgrade";
    fs::remove_all(folder);
    fs::create_directories(folder);
    std::string path = (folder / "lms.db").string();
    {
        SqliteDatabase db(path);  // make it look like a v1.0 database: no class_extension table
        SqliteUserRepository users(db);
        users.add(Role::ADMIN, "admin", "Admin", "$argon2id$v=19$m=1,t=1,p=1$fake");
        db.execute("DROP TABLE class_extension;");
    }
    {
        SqliteDatabase db(path);  // opened by v1.1
        SqliteUserRepository users(db);
        CHECK(users.findByUsername("admin") != nullptr);  // old data kept
        Statement st = db.prepare(
            "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = "
            "'class_extension';");
        st.step();
        CHECK(st.columnInt(0) == 1);  // new table created
    }
    fs::remove_all(folder);
}

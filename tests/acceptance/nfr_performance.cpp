// nfr_performance.cpp - acceptance measurements for NFR01 and NFR05.
// Builds a full-size database (840 users; five years of marking history) with the
// real repositories and the real Argon2id settings, then times:
//   NFR01  login with 800 students stored            (limit: under 2 seconds)
//   NFR05  audit history of a five-year-old record    (limit: within 3 seconds)
// Run: ./build/lms_nfr_perf     (it creates and deletes nfr_perf.db in the current folder)
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "AuditService.h"
#include "AuthService.h"
#include "SodiumPasswordHasher.h"
#include "SqliteRepositories.h"

using Clock = std::chrono::steady_clock;

static double millisecondsBetween(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}

int main() {
    const char* path = "nfr_perf.db";
    std::remove(path);
    double worstLogin = 0, worstQuery = 0;
    long long auditRows = 0;
    {
        SqliteDatabase db(path);
        SqliteUserRepository users(db);
        SqliteAuditRepository audit(db);
        SodiumPasswordHasher hasher;  // the same cost settings as the real program
        AuthService auth(users, hasher);

        std::cout << "Creating 800 students and 40 lecturers...\n";
        std::string hash = hasher.hash("pw");  // every user's hash has the same cost
        db.runInTransaction([&] {
            for (int i = 1; i <= 800; i++) {
                users.add(Role::STUDENT, "student" + std::to_string(i),
                          "Student " + std::to_string(i), hash);
            }
            for (int i = 1; i <= 40; i++) {
                users.add(Role::LECTURER, "lecturer" + std::to_string(i),
                          "Lecturer " + std::to_string(i), hash);
            }
        });

        // ---- NFR01
        std::vector<std::string> logins = {"student1",   "student400", "student800", "lecturer20",
                                           "student777", "student123", "student555", "lecturer40",
                                           "student2",   "student799"};
        for (const std::string& name : logins) {
            Clock::time_point start = Clock::now();
            Session s = auth.login(name, "pw");
            worstLogin = std::max(worstLogin, millisecondsBetween(start, Clock::now()));
        }

        // ---- NFR05: 800 students x 12 assignments a year x 5 years = 48,000 submissions,
        // each with 2 audit entries (MARK + MODERATE) = 96,000 audit rows.
        std::cout << "Creating five years of marking history...\n";
        db.execute("INSERT INTO department(name) VALUES ('Test');");
        db.execute("INSERT INTO course(code, title, department_id, lecturer_id) "
                   "VALUES ('T1', 'Test', 1, 801);");
        const long long fiveYearsAgo = 1640995200;  // 1 Jan 2022
        const long long month = 30LL * 24 * 3600;
        db.runInTransaction([&] {
            int submissionId = 0;
            for (int a = 1; a <= 60; a++) {
                long long t = fiveYearsAgo + a * month;
                Statement st = db.prepare(
                    "INSERT INTO assignment(course_id, title, deadline, max_mark, created_by, "
                    "created_at) VALUES (1, ?, ?, 50, 801, ?);");
                st.bind(1, "Assignment " + std::to_string(a)).bind(2, t).bind(3, t);
                st.step();
                for (int s = 1; s <= 800; s++) {
                    Statement sub = db.prepare(
                        "INSERT INTO submission(assignment_id, student_id, file_path, "
                        "submitted_at, "
                        "status, raw_mark, mark, feedback, marked_by, marked_at, moderated_by, "
                        "moderated_at, released_at) "
                        "VALUES (?, ?, 'f', ?, 'RELEASED', 40, 42, 'ok', 801, ?, 801, ?, ?);");
                    sub.bind(1, a).bind(2, s).bind(3, t).bind(4, t).bind(5, t).bind(6, t);
                    sub.step();
                    submissionId++;
                    audit.append(AuditEntry(0, submissionId, 801, fromUnixSeconds(t),
                                            AuditAction::MARK, std::nullopt, 40, "Initial mark"));
                    audit.append(AuditEntry(0, submissionId, 801, fromUnixSeconds(t + 3600),
                                            AuditAction::MODERATE, 40, 42, "Moderated"));
                }
            }
        });
        Statement count = db.prepare("SELECT COUNT(*) FROM audit_log;");
        count.step();
        auditRows = count.columnInt64(0);

        users.add(Role::HOD, "hod", "HoD", hash);
        Session hod = auth.login("hod", "pw");
        AuditService auditService(audit);
        for (int id : {1, 12000, 24000, 36000, 48000}) {  // id 1 is five years old
            Clock::time_point start = Clock::now();
            std::vector<AuditEntry> history = auditService.history(hod, id);
            worstQuery = std::max(worstQuery, millisecondsBetween(start, Clock::now()));
            if (history.size() != 2) {
                std::cout << "Unexpected result for submission " << id << "\n";
                return 1;
            }
        }
    }
    std::remove(path);

    bool nfr01 = worstLogin < 2000;
    bool nfr05 = worstQuery < 3000;
    std::cout << "\nNFR01  slowest of 10 logins with 840 users: " << worstLogin << " ms"
              << "  (limit 2000 ms)  " << (nfr01 ? "PASS" : "FAIL") << "\n";
    std::cout << "NFR05  slowest of 5 audit queries over " << auditRows
              << " audit rows: " << worstQuery << " ms  (limit 3000 ms)  "
              << (nfr05 ? "PASS" : "FAIL") << "\n";
    return (nfr01 && nfr05) ? 0 : 1;
}

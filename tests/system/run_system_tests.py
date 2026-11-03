# run_system_tests.py - system tests ST-01 to ST-20 (Phase 5) and ST-21 to ST-22 (CR1, v1.1).
# Runs the REAL lms program, types the test input into it exactly as a user would,
# and checks the output. Each test's full input and output is saved as evidence.
#
# Usage (from the project folder, after building):
#   python tests/system/run_system_tests.py build/lms.exe system-test-run      (Windows)
#   python3 tests/system/run_system_tests.py build/lms system-test-run         (Linux)
#
# The tests share one fresh database and run in order, like a real term:
# set-up -> courses -> assignments -> submissions -> marking -> moderation -> release.
import json, os, shutil, sqlite3, subprocess, sys, time, datetime as dt

LMS = os.path.abspath(sys.argv[1])
WORK = os.path.abspath(sys.argv[2])
shutil.rmtree(WORK, ignore_errors=True); os.makedirs(WORK)
DB = os.path.join(WORK, "lms.db")
open(os.path.join(WORK, "essay.txt"), "w").write("My project report")
open(os.path.join(WORK, "essay_v2.txt"), "w").write("My project report v2")
ESSAY = os.path.join(WORK, "essay.txt"); ESSAY2 = os.path.join(WORK, "essay_v2.txt")

def fmt(t): return t.strftime("%Y-%m-%d %H:%M")
now = dt.datetime.now()
D3 = fmt(now + dt.timedelta(days=3)); D5 = fmt(now + dt.timedelta(days=5))
PAST = fmt(now - dt.timedelta(days=1))

def run(lines):
    p = subprocess.run([LMS, DB], input="\n".join(lines) + "\n\n", capture_output=True,
                       text=True, cwd=WORK, timeout=60)
    return p.stdout + p.stderr

def move_deadline_back(assignment_id, hours_late):
    """Test setup step: put the assignment deadline in the past."""
    c = sqlite3.connect(DB)
    c.execute("UPDATE assignment SET deadline=? WHERE assignment_id=?",
              (int(time.time()) - hours_late * 3600, assignment_id))
    c.commit(); c.close()

results = []
def test(tid, use_case, frs, title, lines, expect, forbid=()):
    out = run(lines)
    missing = [e for e in expect if e not in out]
    present = [f for f in forbid if f in out]
    ok = not missing and not present and "Fatal" not in out
    results.append(dict(id=tid, use_case=use_case, frs=frs, title=title, passed=ok,
                        missing=missing, unexpected=present))
    with open(os.path.join(WORK, f"{tid}.txt"), "w") as f:
        f.write(f"{tid} {title}\nInput:\n" + "\n".join(lines) + "\n\nOutput:\n" + out)
    print(f"{tid}: {'PASS' if ok else 'FAIL'}  {title}" + (f"  missing={missing} unexpected={present}" if not ok else ""))

# ---------------- set-up (pre-conditions), done through the real admin menu
setup = run(["Admin Person", "adminpw", "admin", "adminpw",
  "1","2","lec1","Lecturer One","pw",  "1","2","lec2","Lecturer Two","pw",
  "1","3","hod1","HoD One","pw",       "1","3","hod2","HoD Two","pw",
  "1","1","stu1","Student One","pw",   "1","1","stu2","Student Two","pw",
  "1","1","stu3","Student Three","pw",
  "6","Computer Science","4",  "6","Mathematics","5",
  "9","CS4433","Software Engineering","1",
  "12","1","2",  "13","6","1",  "13","7","1",  "0"])
open(os.path.join(WORK, "SETUP.txt"), "w").write(setup)
assert "Course created with id 1" in setup, setup[-500:]
# user ids: admin 1, lec1 2, lec2 3, hod1 4, hod2 5, stu1 6, stu2 7, stu3 8

# ---------------- UC Log in
test("ST-01", "Log in", "FR20, FR21", "Valid login opens the menu for the user's role",
     ["stu1","pw","0", "lec1","pw","0", "hod1","pw","0"],
     ["Welcome, Student One", "Student menu", "Lecturer menu", "Head of Department menu"],
     forbid=["Administrator menu"])
test("ST-02", "Log in (exception flows)", "FR20, FR01", "Wrong password, unknown user and deactivated account are refused",
     ["stu1","wrong", "ghost","pw", "admin","adminpw","4","8","0","0", "stu3","pw",
      "admin","adminpw","4","8","1","0"],
     ["Invalid username or password.", "User deactivated.", "This account has been deactivated."])

# ---------------- Administrator use cases
test("ST-03", "Manage user accounts", "FR01", "Create, edit, reset password; duplicate username refused",
     ["admin","adminpw", "1","1","stu1","Copy","pw", "1","1","stu4","Student Four","pw",
      "2","9","stu4x","Student Four Edited", "3","9","newpw", "5", "0", "stu4x","newpw","0"],
     ["Error: Username 'stu1' is already taken.", "User created with id 9", "User updated.",
      "Password changed.", "stu4x  | Student Four Edited", "Welcome, Student Four Edited"])
test("ST-04", "Manage departments and courses", "FR02", "Edit course, list; a HoD cannot head two departments",
     ["admin","adminpw", "6","Physics","4", "10","1","CS4433","Software Engineering I", "8", "11","0"],
     ["Error: That Head of Department already heads a department.", "Course updated.",
      "[1] Computer Science  | HoD: HoD One", "CS4433  Software Engineering I", "lecturer: Lecturer One"])
test("ST-05", "Assign lecturer to course", "FR03", "Only a Lecturer can be assigned",
     ["admin","adminpw", "12","1","6", "12","99","2", "0"],
     ["Error: That user is not a Lecturer.", "Error: No course with that id."])
test("ST-06", "Enrol student in course", "FR04", "A student cannot be enrolled twice; only students can enrol",
     ["admin","adminpw", "13","6","1", "13","2","1", "0"],
     ["Error: The student is already enrolled on this course.", "Error: That user is not a Student."])

# ---------------- Lecturer: create assignment
test("ST-07", "Create assignment", "FR05", "Lecturer creates an assignment; invalid ones refused",
     ["lec2","pw", "3","1","Hack","x",D3,"50", "0",
      "lec1","pw", "1", "3","1","Bad",  "x",PAST,"50",  "3","1","Bad2","x",D3,"0",
      "3","1","Final Project","Build the LMS",D3,"50", "3","1","Late Lab","Lab work",D3,"20", "2","1","0"],
     ["Error: You are not the lecturer of this course.", "[1] CS4433  Software Engineering",
      "Error: The deadline must be in the future.", "Error: Maximum mark must be greater than 0.",
      "Assignment created with id 1.", "Assignment created with id 2.", "[1] Final Project"])

# ---------------- Student: view courses / assignments
test("ST-08", "View enrolled courses", "FR14", "Enrolled student sees course; non-enrolled sees none",
     ["stu1","pw","1","0", "stu3","pw","1","0"],
     ["CS4433  Software Engineering", "You are not enrolled on any course yet."])
test("ST-09", "View assignments", "FR15", "Student sees assignments with deadline and maximum mark",
     ["stu1","pw","2","0"],
     ["[1] Final Project  | deadline " + D3, "max mark 50", "[2] Late Lab"])

# ---------------- Submit work
test("ST-10", "Submit work", "FR16", "On-time submission accepted; missing file and other course refused",
     ["stu1","pw", "3","1","no_such_file.pdf", "3","1",ESSAY, "0", "stu3","pw","3","1",ESSAY,"0"],
     ["Error: File not found: no_such_file.pdf", "Submitted. Status: SUBMITTED, attempt 1",
      "Error: You are not enrolled on this course."])
test("ST-11", "Submit work + Resubmit work", "FR17", "Resubmission before the deadline replaces the file",
     ["stu1","pw","3","1",ESSAY2,"0", "lec1","pw","5","1","0"],
     ["Submitted. Status: SUBMITTED, attempt 2", "attempt 2  | file ", "submissions/a1_s6_attempt2.txt"])

# ---------------- Extension
test("ST-12", "Grant individual extension", "FR06", "Extension shown on the student's list; bad requests refused",
     ["lec1","pw", "4","1","7",PAST,"Sick", "4","1","7",D5,"", "4","1","7",D5,"Medical note", "0",
      "stu2","pw","2","0", "stu1","pw","2","0"],
     ["Error: The new deadline must be later than the original deadline.",
      "Error: Reason cannot be empty.", "Extension granted.",
      "(extended to " + D5 + " for you)"])

# deadline of assignment 1 and 2 moves 30 h into the past (test set-up)
move_deadline_back(1, 30); move_deadline_back(2, 30)

test("ST-13", "Submit work + Apply late penalty (exception flows)", "FR16, FR17, FR06, C2, O2",
     "After the deadline: late work accepted, resubmission refused, extension still on time",
     ["stu1","pw", "3","2",ESSAY, "3","2",ESSAY, "3","1",ESSAY, "0",
      "stu2","pw", "3","1",ESSAY, "0"],
     ["Submitted. Status: LATE, attempt 1, 2 day(s) late", "Error: Resubmission is not allowed.",
      "Submitted. Status: SUBMITTED, attempt 1"])

# submissions: 1 = A1/stu1 (on time), 2 = A2/stu1 (late, 2 days), 3 = A1/stu2 (extension)
test("ST-14", "Mark submission + Write feedback + Record audit entry", "FR07, FR08, FR18, FR22",
     "Marks validated; late penalty applied; mark hidden from student, feedback visible",
     ["lec2","pw","6","1","40","x","0",
      "lec1","pw", "6","1","51","Too high", "6","1","40","", "6","1","40","Good structure",
      "6","2","25","Too high for this one", "6","2","16","Late but solid", "6","3","35","Fair", "5","2", "0",
      "stu1","pw","4","0"],
     ["Error: This submission is not for one of your assignments.",
      "Error: Mark must be between 0 and 50.", "Error: Feedback cannot be empty.", "Error: Mark must be between 0 and 20.",
      "Marked. The student can now read the feedback", "MARKED  | days late 2", "mark 12",
      "Mark: not released yet", "Feedback: Good structure", "Feedback: Late but solid"],
     forbid=["Mark: 40 / 50", "Mark: 12 / 20", "50.000000"])
test("ST-15", "Mark submission (re-mark)", "FR22", "Re-mark needs a reason and is recorded",
     ["lec1","pw", "7","1","45","", "7","1","45","Missed the testing section", "0"],
     ["Error: Reason cannot be empty.", "Re-marked. The change is recorded in the audit log."])

# ---------------- Head of Department
test("ST-16", "Review department marks", "FR10", "HoD sees marked work of own department only",
     ["hod1","pw","1","0", "hod2","pw","1","0"],
     ["[1] assignment 1, Student One  | raw 45", "[2] assignment 2, Student One  | raw 16  | days late 2  | final 12",
      "Nothing is waiting for moderation."])
test("ST-17", "Moderate mark", "FR11, FR22", "Approve; adjust needs reason and valid mark; other department refused",
     ["hod2","pw","2","1","0",
      "hod1","pw", "2","1", "3","2","13","", "3","2","99","Too high", "3","2","13","Illness evidence accepted", "0"],
     ["Error: This assignment is not in your department.", "Mark approved.",
      "Error: Reason cannot be empty.", "Error: Mark must be between 0 and 20.",
      "Mark adjusted. The change is recorded in the audit log."])
test("ST-18", "Release marks", "FR12, FR13, FR19", "Only moderated marks are released and become visible",
     ["hod1","pw","4","1","4","2","0", "stu1","pw","4","0", "stu2","pw","4","0"],
     ["1 mark(s) released.", "Mark: 45 / 50", "Mark: 13 / 20", "Feedback: Fair", "Mark: not released yet"])
test("ST-19", "View audit log", "FR22, O1", "Every mark change shows who, when, old, new and why",
     ["hod1","pw","5","1","5","2","0", "admin","adminpw","14","2","0", "lec1","pw","0"],
     ["MARK by Lecturer One: - -> 40  (Initial mark)",
      "REMARK by Lecturer One: 40 -> 45  (Missed the testing section)",
      "MARK by Lecturer One: - -> 12  (Initial mark)",
      "MODERATE by HoD One: 12 -> 13  (Illness evidence accepted)"])

# ---------------- robustness
test("ST-20", "All (invalid input)", "Phase 4: exceptions/validation", "Bad input never crashes the program",
     ["stu1","pw", "abc", "", "99", "3","xyz","1","   ", "0",
      "lec1","pw","3","1","T","d","next friday","2026-13-45 99:99", D3, "fifty","50","0"],
     ["Please enter a whole number.", "Unknown option.", "Error: File path cannot be empty.",
      "Please use the format YYYY-MM-DD HH:MM", "Please enter a number", "Goodbye."],
     forbid=["Unexpected error", "Fatal"])

# ---------------- v1.1: change request CR1 (class-wide extension)
test("ST-21", "Extend deadline for whole class (CR1, v1.1)", "FR24, FR15, FR16",
     "Lecturer extends a whole class; students see it and submit on time after the original deadline",
     ["lec1","pw", "3","1","Group Project","Team work",D3,"40",
      "8","1",D5,"Too late now",
      "8","4",PAST,"Earlier date",  "8","4",D5,"",  "8","4",D5,"Exam week clash", "0"],
     ["Assignment created with id 4.",
      "Error: The original deadline has passed. Use individual extensions for late cases.",
      "Error: The new deadline must be later than the original deadline.",
      "Error: Reason cannot be empty.",
      "Deadline extended to " + D5 + " for every student on the course."])
move_deadline_back(4, 30)   # the original deadline of assignment 4 has now passed
# (assignment 3 was created by the robustness test ST-20)
test("ST-22", "View assignments + Submit work after a class extension (CR1, v1.1)", "FR15, FR16, FR24",
     "Student list shows the class extension; submitting after the original deadline is on time",
     ["stu1","pw", "2", "3","4",ESSAY, "0"],
     ["[4] Group Project", "(extended to " + D5 + " for the whole class)",
      "Submitted. Status: SUBMITTED, attempt 1"],
     forbid=["Status: LATE"])

json.dump(results, open(os.path.join(WORK, "results.json"), "w"), indent=1)
print(f"\n{sum(r['passed'] for r in results)}/{len(results)} system tests passed")

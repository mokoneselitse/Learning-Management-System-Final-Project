# Phase 5 – Verification and Validation

Moderated LMS · CS4433 Final Project · Selitse M.S (202322621) · 2 Nov 2026

> **Note (v1.1):** these figures are from the end of Phase 5 (v1.0). After change request CR1 the suite is 51 automated tests (349 checks) and 22 system tests, all passing; see [CR1_CHANGE_ANALYSIS.md](CR1_CHANGE_ANALYSIS.md) Section 4.

## 1. Overview

The Moderated LMS passed every test at all four V-Model levels: 46 automated unit and integration test cases (319 checks), 20 system tests and 5 of the 6 acceptance tests. Line coverage of the business-logic layer is 96%, against a target of 70%.

| Level | Tests | Passed | Evidence |
| --- | --- | --- | --- |
| Unit | 24 (UT-01 to UT-24) | 24 | `tests/unit/`, Catch2 output |
| Integration | 22 (IT-01 to IT-22) | 22 | `tests/integration/`, Catch2 output |
| System | 20 (ST-01 to ST-20) | 20 | `tests/system/run_system_tests.py`, one transcript per test |
| Acceptance | 6 (AT-01 to AT-06, one per NFR) | 5; AT-02 (usability) still to be run with a participant | `tests/acceptance/nfr_performance.cpp`, Section 6 |

Testing found 9 defects: 5 in the product and 4 in the tests themselves. All 9 are fixed and retested (Section 7).

The figures in this document are from the end of Phase 5. The Phase 6 code review added one check (students may not look up other users' names) and raised business-logic coverage to 97%; all tests still pass (see `docs/QUALITY_ASSURANCE.md`).

## 2. Test plan

Each V-Model test level checks the artefact produced at the matching level on the left side of the V, so every requirement and design decision is verified by a specific set of tests.

| V-Model level | Verifies this artefact | What is tested | Tool | Test IDs |
| --- | --- | --- | --- | --- |
| Unit testing | Detailed design: class diagrams, Submission state machine, penalty rule | Single classes on their own: StandardLatePenalty, daysLate, Submission transitions, User roles, UserFactory, Session | Catch2 | UT-01 to UT-24 |
| Integration testing | Architecture: the four layers, repositories, IUnitOfWork, sequence diagrams | Services working with the real SQLite repositories and Argon2id hashing, including transactions and database constraints | Catch2 + real SQLite database | IT-01 to IT-22 |
| System testing | SRS functional requirements and use case model | The complete program (`lms`), used through its menus, one test per use case incl. exception flows | `run_system_tests.py` driving the real program | ST-01 to ST-20 |
| Acceptance testing | Stakeholder needs and non-functional requirements (NFR01 to NFR06) | Measured performance, offline use, security, usability | `lms_nfr_perf`, system tests offline, participant session | AT-01 to AT-06 |

### Entry and exit criteria

| Level | Entry criteria (start when) | Exit criteria (finished when) |
| --- | --- | --- |
| Unit | The class compiles with no warnings; its design (class diagram, state table) is approved | Every unit test passes; every state-table transition and every penalty boundary has a test |
| Integration | All unit tests pass; the layer libraries build and link | Every integration test passes; business-logic line coverage is at least 70%; no open High or Medium defects |
| System | All integration tests pass; the `lms` program builds; a fresh database is used | Every use case has at least one passing system test, including its exception flows; every defect found is logged, fixed and retested |
| Acceptance | All system tests pass | Every Must NFR is met with measured evidence; remaining Should items are recorded with a plan |

All automated tests also run on every push through the CI pipeline (Phase 6), which turns the unit and integration levels into a regression suite.

## 3. Test design: equivalence partitioning and boundary values

The riskiest rules (deadlines, the late penalty and the mark range) were tested with equivalence partitioning (one value from each class) and boundary value analysis (values on and just beside every edge), because off-by-one errors happen at the edges.

### Submission time against the deadline (daysLate = ceil((t − deadline) / 24 h))

| Partition | Expected | Test values | Tested in |
| --- | --- | --- | --- |
| Before the deadline | SUBMITTED, 0 days late | deadline − 5 h, deadline − 1 s | UT-07 |
| Exactly at the deadline (boundary) | SUBMITTED, 0 days late | t = deadline | UT-07, IT-07 |
| 1 second after the deadline (boundary) | LATE, 1 day | deadline + 1 s | UT-08 |
| Whole-day edges | 1 day / 2 days | +24 h / +24 h + 1 s | UT-09 |
| Third-day edge | 3 days / 4 days | +72 h / +72 h + 1 s | UT-09 |
| After deadline, inside an extension | SUBMITTED | deadline + 24 h with extension to +48 h | IT-09, ST-13 |

### Late penalty (maximum mark 50, raw mark 40)

| Partition | Days late | Expected final mark | Tested in |
| --- | --- | --- | --- |
| On time | 0 (and −1) | 40 | UT-01, UT-05 |
| Penalised (1 to 3 days) | 1, 2, 3 | 35, 30, 25 | UT-01 |
| Zero (more than 3 days) | 4, 30 | 0 | UT-01, UT-02 |
| Penalty larger than the mark | raw 5, 3 days | 0, never negative | UT-03 |
| Rate uses the maximum mark | raw 20, max 80, 1 day | 12 (not 18) | UT-04 |

### Mark entered by the lecturer or HoD (valid range 0 to maximum)

| Partition | Test values (maximum 50) | Expected | Tested in |
| --- | --- | --- | --- |
| Below range | −1 | Rejected | IT-10, IT-22 |
| Lower boundary | 0 | Accepted | IT-22 |
| Upper boundary | 50 | Accepted | IT-22 |
| Above range | 50.5, 51 | Rejected | IT-10, IT-22, ST-14 |
| HoD adjustment above range | 60, 99 | Rejected | IT-14, ST-17 |

## 4. Automated unit and integration tests

All 46 automated test cases pass (319 checks) in about 1 second. They are written with Catch2 v3, built by CMake as `lms_tests`, and also run through `ctest`. Integration tests use a fresh real SQLite database and a fixed clock for every test, so results are repeatable; they were run 90 times in random order with no failures.

To confirm the tests really detect faults, two bugs were planted on purpose and then removed: changing `>` to `>=` in the penalty rule made UT-01 and UT-02 fail, and releasing MARKED instead of MODERATED work made IT-12 and IT-15 fail.

### Test cases

| ID | Test | Requirement |
| --- | --- | --- |
| UT-01 | Standard penalty matches the design worked example (max 50, raw 40) | FR18 |
| UT-02 | Penalty boundaries: 3 days is penalised, 4 days gives zero | FR18 |
| UT-03 | Penalty never makes a mark negative | FR18 |
| UT-04 | Penalty is 10% of the maximum mark, not of the raw mark | FR18 |
| UT-05 | Days late of zero or less means no penalty | FR18 |
| UT-06 | NoLatePenalty always returns the raw mark | Design (Strategy) |
| UT-07 | Submitting before or exactly at the deadline is 0 days late | FR16 |
| UT-08 | One second late counts as one whole day | FR18 |
| UT-09 | Day boundaries are rounded up | FR18 |
| UT-10 | A new submission can only start as SUBMITTED or LATE | Design (state machine) |
| UT-11 | Full path SUBMITTED → MARKED → MODERATED → RELEASED | FR07, FR11, FR12 |
| UT-12 | Resubmission is allowed only while SUBMITTED | FR17, O2 |
| UT-13 | Late work can be marked; re-marking keeps it MARKED | FR07 |
| UT-14 | There is no MARKED → RELEASED transition | FR13, C1 |
| UT-15 | Invalid transitions are rejected | Design (state machine) |
| UT-16 | Feedback is visible from MARKED, the mark only when RELEASED | FR09, FR13 |
| UT-17 | Each role can perform exactly its own actions | FR21 |
| UT-18 | UserFactory creates the right subclass for each role | Design (Factory) |
| UT-19 | Session::require rejects a forbidden action | FR21 |
| UT-20 | Enum text conversions round-trip and reject unknown text | Design (data mapping) |
| UT-21 | Timestamps convert to Unix seconds and back | Design (data mapping) |
| UT-22 | Effective deadline uses a later extension, otherwise the original | FR06 |
| UT-23 | Marks in messages are shown without extra zeros (regression test for DEF-02) | DEF-02 |
| UT-24 | Blank text is rejected, real text is accepted | Validation |
| IT-01 | Login accepts correct details and rejects wrong ones | FR20 |
| IT-02 | Passwords are stored only as Argon2id hashes | NFR04 |
| IT-03 | Admin creates, edits and deactivates users | FR01 |
| IT-04 | Admin manages departments and courses | FR02 |
| IT-05 | Lecturer assignment and enrolment show up in the lists | FR03, FR04, FR14 |
| IT-06 | Lecturer creates assignments only for their own course | FR05, FR15, FR23 |
| IT-07 | On-time submission, resubmission and file storage | FR16, FR17 |
| IT-08 | Late work is accepted with days late; late resubmission is refused | FR16, FR17, C2, O2 |
| IT-09 | An extension lets only that student submit on time later | FR06 |
| IT-10 | Marking applies the late penalty and writes an audit entry | FR07, FR08, FR18, FR22 |
| IT-11 | Re-marking needs a reason and records old and new marks | FR22 |
| IT-12 | Students see feedback after marking but the mark only after release | FR09, FR13, FR19, C1 |
| IT-13 | The HoD sees only marked work from their own department | FR10 |
| IT-14 | Adjusting a mark needs a reason and a valid mark, and is audited | FR11, FR22 |
| IT-15 | Release only releases moderated work | FR12, FR13 |
| IT-16 | Every role is blocked from other roles' actions | FR21 |
| IT-17 | Users cannot act on work outside their own course or department | FR21, NFR06 |
| IT-18 | If the audit entry cannot be saved, the mark is not saved either (rollback) | FR22 |
| IT-19 | The audit log cannot be changed or deleted | FR22 |
| IT-20 | Data is still there after the program restarts | Persistence |
| IT-21 | The database rejects rows that break the life-cycle rules | Design (state machine) |
| IT-22 | Mark boundaries: 0 and the maximum are accepted, −1 and max + 1 are not | FR07 |

### Coverage of the business-logic layer

Line coverage of `src/logic` is 96% (388 of 404 lines), measured with gcov and gcovr (`-DLMS_COVERAGE=ON`). The 16 uncovered lines are error branches such as "no user with that id" and the real `SystemClock`, which tests replace with a fixed clock.

| File | Lines | Executed | Coverage |
| --- | --- | --- | --- |
| MarkingService.cpp | 74 | 74 | 100% |
| SubmissionService.cpp | 64 | 64 | 100% |
| ModerationService.cpp | 63 | 63 | 100% |
| AuthService.cpp | 15 | 15 | 100% |
| LatePenalty.cpp / .h | 9 | 9 | 100% |
| Validation.h | 10 | 10 | 100% |
| Session.cpp | 6 | 6 | 100% |
| AuditService.cpp | 4 | 4 | 100% |
| AssignmentService.cpp | 50 | 46 | 92% |
| AdminService.cpp | 107 | 96 | 89% |
| IClock.h | 2 | 1 | 50% |
| **Total** | **404** | **388** | **96%** |

For the whole program (all layers, including the menus) automated coverage is 66%; the menus are tested by the system tests in Section 5 instead.

## 5. System tests

All 20 system tests pass on the final build. Every use case in the use case model is covered, each with its exception flows. The tests ran on the real `lms` program: a script typed each test's input into the menus exactly as a user would, checked the output, and saved the full input and output of every test as evidence (`ST-01.txt` to `ST-20.txt`).

The tests share one fresh database and run in order like a real term: set-up, courses, assignments, submissions, marking, moderation, release. To test lateness without waiting days, the test set-up moves two assignment deadlines 30 hours into the past before ST-13.

| ID | Use case | Normal flow checked | Exception flows checked | Requirements | Result |
| --- | --- | --- | --- | --- | --- |
| ST-01 | Log in | Each role gets its own menu only | — | FR20, FR21 | Pass |
| ST-02 | Log in | — | Wrong password; unknown user; deactivated account refused, works again after reactivation | FR20, FR01 | Pass |
| ST-03 | Manage user accounts | Create, edit, reset password, list users | Duplicate username refused | FR01 | Pass |
| ST-04 | Manage departments and courses | Edit course, list departments and courses | HoD who already heads a department refused | FR02 | Pass |
| ST-05 | Assign lecturer to course | Lecturer shown on the course list | Student assigned as lecturer refused; unknown course refused | FR03 | Pass |
| ST-06 | Enrol student in course | (enrolled during set-up) | Second enrolment refused; non-student refused | FR04 | Pass |
| ST-07 | Create assignment; View own assignments | Two assignments created and listed by deadline | Other lecturer's course, past deadline, maximum mark 0 refused | FR05, FR23 | Pass |
| ST-08 | View enrolled courses | Enrolled student sees CS4433 | Student with no courses sees a clear message | FR14 | Pass |
| ST-09 | View assignments | Deadline and maximum mark shown | — | FR15 | Pass |
| ST-10 | Submit work | On-time submission, status SUBMITTED, attempt 1 | Missing file refused; student not on the course refused | FR16 | Pass |
| ST-11 | Submit work → Resubmit work | Resubmission gives attempt 2; file copied to storage | — | FR17 | Pass |
| ST-12 | Grant individual extension | Extension granted and shown on the student's assignment list | Earlier date refused; empty reason refused | FR06 | Pass |
| ST-13 | Submit work → Apply late penalty | Late work accepted as LATE, 2 days late; extended student still on time | Late resubmission refused; resubmission after the deadline refused | FR16, FR17, FR06, C2, O2 | Pass |
| ST-14 | Mark submission; Write feedback; Record audit entry | Late mark 16/20 becomes 12 (penalty); student sees feedback but not the mark | Other lecturer refused; marks above maximum refused; empty feedback refused | FR07, FR08, FR09, FR13, FR18 | Pass |
| ST-15 | Mark submission (re-mark) | Re-mark 40 → 45 recorded | Empty reason refused | FR22 | Pass |
| ST-16 | Review department marks | HoD sees raw mark, days late, final mark, feedback | Other department's HoD sees nothing | FR10 | Pass |
| ST-17 | Moderate mark | Approve; adjust 12 → 13 | Other department refused; empty reason refused; mark above maximum refused | FR11, FR22 | Pass |
| ST-18 | Release marks; View released marks and feedback | Moderated marks released and shown (45/50, 13/20) | Work that is only MARKED stays hidden | FR12, FR13, FR19 | Pass |
| ST-19 | View audit log | HoD and admin see every change with who, when, old, new, reason | — (lecturer and student access is refused in IT-16) | FR22, O1 | Pass |
| ST-20 | All menus (invalid input) | — | Letters, empty input, unknown options, bad dates and bad numbers handled; no crash | Phase 4 robustness | Pass |

The first run passed 14 of 20. The failures came from one real product defect (DEF-02) and two mistakes in the test script (DEF-03, DEF-04); after the fixes all 20 passed (Section 7).

## 6. Acceptance tests

Five of the six non-functional requirements are met with measured evidence; NFR02 (usability) needs a session with a real participant, set out in AT-02 below. Each acceptance test checks one NFR from the SRS, using the metric the SRS names.

| ID | Requirement | How it was tested | Measured result | Limit | Result |
| --- | --- | --- | --- | --- | --- |
| AT-01 | NFR01: login under 2 s with 800 students stored | `lms_nfr_perf` creates 800 students and 40 lecturers, then times 10 logins with the real Argon2id settings | Slowest login about 80 ms; SQLite uses the username index | 2,000 ms | Pass |
| AT-02 | NFR02: untrained lecturer creates an assignment and marks a submission within 10 minutes | Timed session with a participant who has not seen the system (procedure below) | Not yet run | 10 min | Pending |
| AT-03 | NFR03: fully operational with no internet | All 20 system tests re-run with networking removed (no network interface available) | 20 of 20 passed; the system uses only the local `lms.db` file and `submissions` folder | All core functions work | Pass |
| AT-04 | NFR04: no plain-text passwords; salted hashes from a recognised library | Database inspection (IT-02) and code review | Every `password_hash` is an Argon2id string from libsodium with its own random salt; the database refuses a plain-text value; passwords are hidden while typed and never printed | No plain text stored | Pass |
| AT-05 | NFR05: audit record found within 3 s, for records up to five years old | `lms_nfr_perf` creates five years of history: 800 students × 12 assignments a year × 5 years, two audit entries each (96,000 rows), then queries the oldest and newest records | Slowest of 5 queries under 0.2 ms; SQLite uses the `idx_audit_submission` index | 3,000 ms | Pass |
| AT-06 | NFR06: academic records only visible to roles that permit it | IT-16, IT-17 and ST-08, ST-10, ST-17, ST-19 | Students see only their own results; lecturers only their own courses; HoDs only their own department; audit log only HoD and admin | Every wrong-role attempt refused | Pass |

AT-01 and AT-05 were measured on the development computer. Re-run `./build/lms_nfr_perf` on the computer the school will use and record those numbers in the final report.

### AT-02 procedure: usability session (NFR02)

1. Choose a participant who has not seen or used the system (for example a classmate from another course).
2. Prepare a fresh database: an administrator has created the participant as a lecturer, a course assigned to them, and one enrolled student who has already submitted work.
3. Give the participant only this task card: "Log in, create an assignment for your course due next Friday with a maximum mark of 50, then mark the student's submission and give feedback." Give no other help; they may only use the on-screen menus.
4. Start a stopwatch when they press Enter on the username; stop it when the program says "Marked".
5. Record the time, the number of wrong menu choices and any comments.

| Participant | Date | Time taken | Wrong menu choices | Completed without help? | Result (pass if ≤ 10 min) |
| --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |

## 7. Defect log

Nine defects were found: five in the product and four in the tests themselves. All nine are fixed and retested, and none is open. Every product defect now has an automated or system test that would catch it if it came back.

Severity: **High** = crash, data loss or security risk; **Medium** = a requirement not met, or a wrong result; **Low** = cosmetic, or a fault in a test rather than the product.

| ID | Found by | Severity | Description | Cause | Fix | Retest result |
| --- | --- | --- | --- | --- | --- | --- |
| DEF-01 | Integration testing (repeated runs) | Low (test) | IT-02 failed at random, about 1 run in 15 | The test checked that a password hash did not contain "pw"; a random salt can contain those letters by chance | Test now uses a long unique password that cannot appear by chance | Passed 90 runs in a row |
| DEF-02 | System testing (ST-14) | Low | Error message showed "Mark must be between 0 and 50.000000" | `std::to_string(double)` always prints six decimal places | New `formatNumber()` in Validation.h, used in MarkingService and ModerationService | ST-14 and ST-17 pass; regression test UT-23 added |
| DEF-03 | System testing (ST-11) | Low (test) | ST-11 failed although the program was correct | The test expected a short file path; the program shows the full path | Test expectation corrected | ST-11 passes |
| DEF-04 | System testing (ST-14 to ST-19) | Low (test) | Five system tests failed in a chain | Test data gave a mark of 40 to an assignment with a maximum of 20; the program correctly refused it, so later tests had no mark to work with | Test data changed to 16 out of 20 | ST-14 to ST-19 pass |
| DEF-05 | Code review of the tests | Low (test) | On Windows, integration tests would leave temporary files behind | The temp folder was deleted before the database file was closed, and Windows cannot delete an open file | Temp folder is now the first member of the fixture, so it is removed last | No files left after 90 runs |
| DEF-06 | Phase 4 review against the SRS | Medium | FR01 requires editing user accounts, but only create and deactivate existed | The class diagram had no edit operation | Added `editUser()` and `resetPassword()` with matching repository methods | IT-03 and ST-03 pass |
| DEF-07 | Phase 4 review against the SRS | Medium | FR02 requires managing departments and courses, but only create existed | The class diagram had no edit or list operations | Added `editCourse()`, `setDepartmentHod()` and list operations | IT-04 and ST-04 pass |
| DEF-08 | Phase 4 review | High | Only the path to a submitted file was saved; if the student moved or deleted the file, the work was lost | The design stored `filePath` but did not say where the file is kept | SubmissionService copies each attempt into the `submissions` folder | IT-07 and ST-11 pass; the copied files exist |
| DEF-09 | Phase 4 code review | High | A very long or unusual file path could crash the program | The file-system call could throw an exception the menus did not catch | Error-code versions of the file checks, and the menu loop now also catches any standard exception | IT-07 (5,000-character path and a folder) and ST-20 pass |

## 8. How to re-run the tests

All commands run in the MSYS2 UCRT64 terminal, from the project folder, after `cmake -S . -B build -G Ninja` and `cmake --build build`.

| What | Command | Expected output |
| --- | --- | --- |
| Unit + integration tests | `./build/lms_tests.exe` | All tests passed (46 test cases) |
| Same, through CMake | `ctest --test-dir build` | 100% tests passed |
| Only one level | `./build/lms_tests.exe "[unit]"` or `"[integration]"` | Only that level runs |
| Coverage report | `cmake -S . -B build-cov -G Ninja -DLMS_COVERAGE=ON`, `cmake --build build-cov`, `./build-cov/lms_tests.exe`, then `gcovr -r . build-cov --filter src/logic/ --html-details coverage.html` | Table with a TOTAL of about 96%; `coverage.html` to open in a browser |
| System tests | `python tests/system/run_system_tests.py build/lms.exe system-test-run` | 20/20 system tests passed; one transcript per test in `system-test-run` |
| Acceptance measurements (NFR01, NFR05) | `./build/lms_nfr_perf.exe` | Two PASS lines with the measured times |
| Offline check (NFR03) | Turn off Wi-Fi, then run the system tests again | 20/20 system tests passed |

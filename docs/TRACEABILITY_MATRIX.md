# Requirements Traceability Matrix (v1.1)

Each requirement is traced to its use case, the design elements that realise it, the source files that
implement it, and the tests that verify it. Test IDs: UT = unit, IT = integration, ST = system,
AT = acceptance ([test plan and results](PHASE5_VERIFICATION_AND_VALIDATION.md)). All source files are under `src/`.

## Functional requirements

| Req. | Use case | Design elements (classes, diagrams, tables) | Source files | Tests |
|---|---|---|---|---|
| FR01 | Manage user accounts | User + 4 role classes, UserFactory; `users` table | AdminService.cpp, SqliteUserRepository.cpp, AdminMenu.cpp | IT-03, ST-02, ST-03 |
| FR02 | Manage departments and courses | Department, Course; `department`, `course` | AdminService.cpp, SqliteCourseRepository.cpp, AdminMenu.cpp | IT-04, ST-04 |
| FR03 | Assign lecturer to course | Course.lecturerId; `course.lecturer_id` | AdminService.cpp, SqliteCourseRepository.cpp | IT-05, ST-05 |
| FR04 | Enrol student in course | Enrolment; `enrolment` | AdminService.cpp, SqliteCourseRepository.cpp | IT-05, ST-06 |
| FR05 | Create assignment | Assignment; `assignment` | AssignmentService.cpp, SqliteAssignmentRepository.cpp, LecturerMenu.cpp | IT-06, ST-07 |
| FR06 | Grant individual extension | Extension, Assignment::effectiveDeadlineFor; `extension` | AssignmentService.cpp, Assignment.cpp | UT-22, IT-09, ST-12, ST-13 |
| FR07 | Mark submission | Submission::recordMark (state machine); MarkingService | MarkingService.cpp, Submission.cpp | UT-11, UT-13, IT-10, IT-22, ST-14 |
| FR08 | Write feedback | Submission.feedback; `submission.feedback` | MarkingService.cpp, LecturerMenu.cpp | IT-10, ST-14 |
| FR09 | View released marks and feedback | isFeedbackVisibleToStudent; `student_result_view` | Submission.cpp, SqliteSubmissionRepository.cpp | UT-16, IT-12 |
| FR10 | Review department marks | ModerationService::pendingForDepartment; seq_moderate | ModerationService.cpp, HodMenu.cpp | IT-13, ST-16 |
| FR11 | Moderate mark | Submission::approve / adjustMark; AuditEntry | ModerationService.cpp, Submission.cpp | UT-11, IT-14, ST-17 |
| FR12 | Release marks | Submission::release; state MODERATED → RELEASED | ModerationService.cpp | UT-11, IT-15, ST-18 |
| FR13 | View released marks and feedback | isMarkVisibleToStudent; no MARKED → RELEASED; `student_result_view` | Submission.cpp, Schema.h, SubmissionService.cpp | UT-14, UT-16, IT-12, IT-15, ST-18 |
| FR14 | View enrolled courses | ICourseRepository::coursesForStudent | AssignmentService.cpp, StudentMenu.cpp | IT-05, ST-08 |
| FR15 | View assignments | AssignmentService::listForStudent, deadlineFor (v1.1) | AssignmentService.cpp, StudentMenu.cpp | IT-06, IT-25, ST-09, ST-21, ST-22 |
| FR16 | Submit work | SubmissionService::submit; seq_submit; LATE state | SubmissionService.cpp | UT-07, IT-07, IT-08, IT-24, ST-10, ST-13, ST-22 |
| FR17 | Resubmit work | Submission::resubmit (SUBMITTED → SUBMITTED) | SubmissionService.cpp, Submission.cpp | UT-12, IT-07, IT-08, ST-11, ST-13 |
| FR18 | Apply late penalty | ILatePenaltyPolicy, StandardLatePenalty (Strategy) | LatePenalty.cpp, SubmissionService.cpp, MarkingService.cpp | UT-01–UT-05, UT-08, UT-09, IT-10, ST-14 |
| FR19 | View released marks and feedback | StudentResult; `student_result_view` | SubmissionService.cpp, StudentMenu.cpp | IT-12, ST-18 |
| FR20 | Log in | AuthService, IPasswordHasher, SodiumPasswordHasher | AuthService.cpp, SodiumPasswordHasher.cpp, main.cpp | IT-01, ST-01, ST-02 |
| FR21 | Log in (all roles) | User::canPerform, Session::require, MenuFactory | User.cpp, Session.cpp, all *Service.cpp | UT-17, UT-19, IT-16, IT-17, ST-01 |
| FR22 | Record audit entry; View audit log | AuditEntry, IAuditRepository, IUnitOfWork; `audit_log` + triggers | MarkingService.cpp, ModerationService.cpp, SqliteAuditRepository.cpp | IT-10, IT-11, IT-14, IT-18, IT-19, ST-14, ST-15, ST-17, ST-19 |
| FR23 | View own assignments | AssignmentService::listForLecturer | AssignmentService.cpp, LecturerMenu.cpp | IT-06, ST-07 |
| FR24 (CR1) | Extend deadline for whole class | ClassExtension; `class_extension` | AssignmentService.cpp, SqliteAssignmentRepository.cpp, LecturerMenu.cpp | UT-25, IT-23, IT-24, ST-21, ST-22 |

## Non-functional requirements

| Req. | Requirement | Design element | Tests |
|---|---|---|---|
| NFR01 | Login under 2 s with 800 students | `users.username` unique index | AT-01 (`lms_nfr_perf`) |
| NFR02 | Untrained lecturer done within 10 min | Short numbered menus; user manuals | AT-02 (participant session) |
| NFR03 | Works with no internet | Embedded SQLite, single machine | AT-03 (system tests offline) |
| NFR04 | No plain-text passwords | Argon2id via libsodium; CHECK constraint | IT-02, AT-04 |
| NFR05 | Audit record within 3 s (5 years) | `idx_audit_submission` index | AT-05 (`lms_nfr_perf`) |
| NFR06 | Records only for permitted roles | Session::require + ownership checks | IT-16, IT-17, AT-06 |

Every functional requirement has at least one automated (UT/IT) and one system (ST) test.
Design diagrams: [images/](images/) (v1.0 originals and v1.1 updates for CR1).

# Change Request CR1 – Analysis, Implementation and Regression Testing

> **CR1:** "A lecturer should be able to extend the deadline for a whole class, not just an individual
> student, and extensions should be shown on the student's assignment list."

Delivered as **release v1.1.0** (tag `v1.1`). All 51 automated tests and 22 system tests pass,
including every v1.0 test (regression).

## 1. Classification

**Perfective maintenance** (ISO/IEC 14764; in Sommerville's terms, *functionality addition*).

| Type | Applies? | Reason |
|---|---|---|
| Corrective (fixing a fault) | No | v1.0 meets its requirements: individual-only extensions were a deliberate scope decision (conflict C3 in the SRS) |
| Adaptive (new environment) | No | Same platform, compiler, database and computer |
| **Perfective (new or improved functionality)** | **Yes** | Users asked for a new capability after release (class-wide extension) and better information on an existing screen (the student's list) |
| Preventive (future maintainability) | No | Not the purpose of the change, although the design keeps the deadline rule in one domain method |

Because the change adds features without breaking anything, the version goes from 1.0.0 to **1.1.0**
(minor version, semantic versioning). A v1.0 database keeps working.

## 2. Impact analysis

### Requirements

| Item | Change |
|---|---|
| **FR24 (new)** | The system shall allow a Lecturer to extend the deadline of an assignment for every student on the course, with a recorded reason, before the original deadline. |
| FR15 (changed) | The student's assignment list shall also show the deadline that applies to the student, and whether it was extended for the whole class or for the student only. |
| FR16 (changed) | "up to its deadline (as individually extended, if applicable)" becomes "… as extended for the class or individually". |
| FR06, FR17, FR18 | Unchanged; the effective deadline they use now includes a class extension. |
| C3 (SRS conflict) | Resolved: class-wide extension was deferred to change control; CR1 now adds it. |
| NFR01–NFR06 | Not affected (one extra indexed lookup by primary key per submission). |

### Use cases ([v1.1 excerpt](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/use_case_cr1_v1.1.png))

| Use case | Change |
|---|---|
| Extend deadline for whole class (Lecturer) | **New** |
| View assignments (Student) | Changed: shows the extended deadline and its kind |
| Submit work (Student) | Changed: the deadline used includes a class extension |
| Grant individual extension, all others | Unchanged |

### Design diagrams (v1.0 and v1.1 versions in `images/`; changes drawn in blue)

| Diagram | Affected? | Change |
|---|---|---|
| Domain class diagram | Yes | New class `ClassExtension`; composition Assignment 1 → 0..1 ClassExtension; new `Assignment::effectiveDeadlineFor(classExt, ext)` ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/class_domain_v1.1.png)) |
| Business-logic class diagram | Yes | `AssignmentService::grantClassExtension`, `deadlineFor`; new struct `DeadlineInfo` ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/class_logic_v1.1.png)) |
| Data-access class diagram | Yes | `IAssignmentRepository::saveClassExtension`, `findClassExtension` ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/class_data_v1.1.png)) |
| ERD | Yes | New table `class_extension` (assignment 1 → 0..1) ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/erd_v1.1.png)) |
| Sequence: submit assignment | Yes | New call `findClassExtension(assignmentId)` before the deadline is worked out ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/seq_submit_v1.1.png)) |
| Use case diagram | Yes | See above |
| Deployment diagram | Yes (labels only) | Version 1.1.0, release zip name, database schema v2 ([v1.1](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/deployment_diagram_v1.1.png)) |
| Submission state machine | No | Because of the decision in Section 3, no submission is LATE when a class extension is granted, so no new transition is needed; the guard "effective deadline" now simply includes it |
| Activity diagram, component diagram, moderation sequence | No | The process, the components and moderation are unchanged |

### Code (17 files changed, 2 added; about 300 lines)

| Layer | Class / file | Change |
|---|---|---|
| Domain | `ClassExtension.h` | New |
| Domain | `Assignment` | New `effectiveDeadlineFor(classExt, ext)`; the v1.0 version now calls it |
| Data | `Schema.h` | New table `class_extension` |
| Data | `IAssignmentRepository`, `SqliteAssignmentRepository` | `saveClassExtension`, `findClassExtension` |
| Logic | `AssignmentService` | `grantClassExtension`, `deadlineFor`, struct `DeadlineInfo`; `myExtension` removed (replaced by `deadlineFor`) |
| Logic | `SubmissionService::effectiveDeadline` | Also reads the class extension |
| Presentation | `LecturerMenu` | Option 8 "Extend the deadline for the whole class" (added at the end so existing option numbers stay the same) |
| Presentation | `StudentMenu` | Assignment list shows "(extended to … for the whole class)" or "(… for you)" |
| Build | `CMakeLists.txt` | Version 1.1.0 |
| Not changed | `Submission`, `MarkingService`, `ModerationService`, `AdminService`, `AuditService`, `AuthService`, the other repositories and menus | — |

### Database

New table `class_extension` (primary key = foreign key `assignment_id`, so at most one per assignment).
No existing table changes. A v1.0 database is upgraded automatically the first time v1.1 opens it,
because the schema uses `CREATE TABLE IF NOT EXISTS`; all existing data is kept (test IT-26).

### Tests

| Test | Change |
|---|---|
| UT-25 | New: effective deadline is the latest of original, class and individual |
| IT-23 | New: class extension rules (own course, later date, reason, before the deadline, replaces an earlier one) |
| IT-24 | New: every student submits on time until the class deadline; lateness counted from the class deadline |
| IT-25 | New: the student's list shows which deadline applies and why |
| IT-26 | New: a v1.0 database is upgraded and keeps its data |
| ST-21, ST-22 | New system tests through the real menus |
| IT-09 | Changed: uses `deadlineFor` instead of the removed `myExtension` |
| All other tests | Unchanged, re-run as the regression suite |

### Documents

SRS (FR24, FR15, FR16), the diagrams above, Lecturer and Student user manuals, installation guide
(upgrade), release notes (v1.1), README.

## 3. Design decision: a class extension must be granted before the original deadline

If a whole-class extension were allowed after the deadline, students who had already submitted late
would need their LATE status and days-late value undone. That would need a new LATE → SUBMITTED
transition in the state machine and changes to the database CHECK rules, which raises the risk of the
change. Lecturers normally announce a class extension before the deadline; for late cases after it,
the v1.0 individual extension (FR06) still exists. So v1.1 refuses a class extension once the
original deadline has passed. **Alternative rejected:** re-classifying existing late submissions,
for the reasons above.

Other rules: the new deadline must be later than the original; a reason is required; only the course's
lecturer may grant it; granting again replaces the earlier class extension; if a student also has an
individual extension, the later of the two applies.

## 4. Regression testing

The v1.0 suite was run **before** the change as a baseline and **after** it, unchanged except IT-09.

| Suite | v1.0 baseline | After CR1 (v1.1) |
|---|---|---|
| Unit tests | 24 / 24 | 25 / 25 |
| Integration tests | 22 / 22 | 26 / 26 |
| Total automated (checks) | 46 test cases (320 checks) | 51 test cases (349 checks) |
| System tests | 20 / 20 | 22 / 22 (ST-01 to ST-20 unchanged) |
| Business-logic coverage | 97% (395 / 407 lines) | 96% (418 / 434 lines) |
| Static analysis | 0 findings | 0 findings (one new finding, an unused method, fixed) |

The new tests were checked to detect faults: removing the class extension from
`SubmissionService::effectiveDeadline` made IT-24 fail.

| ID | Found by | Severity | Description | Fix | Retest |
|---|---|---|---|---|---|
| DEF-10 | cppcheck (CI gate) | Low | `AssignmentService::myExtension` was left unused after the student list switched to `deadlineFor` | Removed; IT-09 updated | cppcheck clean, all tests pass |
| DEF-11 | System testing (ST-21) | Low (test) | ST-21 failed: the test used assignment id 3, but ST-20 had already created assignment 3 | Test uses id 4 | ST-21, ST-22 pass |

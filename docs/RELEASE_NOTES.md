# Release Notes

## Version 1.1.0 – class-wide extensions (tag `v1.1`)

Change request **CR1** ([analysis](CR1_CHANGE_ANALYSIS.md)).

### New
* **Lecturers can extend the deadline for the whole class** (Lecturer menu, option 8), with a reason,
  before the original deadline (FR24). Granting it again replaces the earlier date.
* **Students see extended deadlines on their assignment list**, marked "for the whole class" or
  "for you" (FR15).

### Changed
* The deadline for submitting is now the latest of the original deadline, the class extension and the
  student's own extension (FR16). Late days are counted from that deadline.
* All existing menu options keep their numbers.

### Upgrade
* Replace the program files and keep `C:\LMS\data`. The database is upgraded automatically on first
  start (new table `class_extension`); no data is lost. See the installation guide, Section 7.

### Quality
* 51 automated tests (5 new) and 22 system tests (2 new), all passing; all v1.0 tests re-run as
  regression tests (only IT-09 updated, for a renamed method). Business-logic coverage 96%.

### Known limitations
* A class extension cannot be granted after the original deadline; use individual extensions then.
* The other v1.0 limitations below still apply, except the one about class-wide extensions.

## Version 1.0.0 – first release (tag `v1.0`)

The first release of the Moderated LMS: an offline, single-computer system in which lecturers mark work,
the Head of Department moderates and releases the marks, and every mark change is audited.

### Features

**All users**
* Login with username and password; each role sees only its own menu (FR20, FR21).
* Passwords are hidden while typed.

**Administrator**
* Create, edit and deactivate user accounts for all four roles; reset passwords (FR01).
* Create and edit departments and courses, set each department's HoD; list users, departments and
  courses (FR02).
* Assign lecturers to courses and enrol students (FR03, FR04).
* View the audit log of any submission.

**Lecturer**
* Create assignments with a deadline and maximum mark for their own courses (FR05); list them sorted
  by deadline or date created (FR23).
* Grant an individual deadline extension with a reason (FR06).
* View submissions, mark them with written feedback (FR07, FR08) and re-mark with a reason.

**Head of Department**
* Review all marked work in the department (FR10).
* Approve marks or adjust them with a required reason (FR11).
* Release moderated marks for an assignment (FR12).
* View the audit log of a submission (FR22).

**Student**
* View enrolled courses and assignments, with any personal extension shown (FR14, FR15).
* Submit work up to the deadline and resubmit before it; late work is accepted (FR16, FR17).
* Read feedback as soon as work is marked (FR09); see the mark only after release (FR13, FR19).

### Rules built in
* Late penalty: 10% of the maximum mark per day late (rounded up to whole days) for up to 3 days;
  0 after that (FR18).
* Every mark change (mark, re-mark, moderation) is saved with who, when, old mark, new mark and reason,
  in the same transaction as the change; the audit log cannot be edited or deleted (FR22).
* Passwords are stored only as salted Argon2id hashes (libsodium) (NFR04).
* Submitted files are copied into the data folder, one file per attempt.

### Quality
* 46 automated unit and integration tests and 20 system tests, all passing; 97% line coverage of the
  business-logic layer.
* Login in about 0.1 s with 840 users; audit history found in under 1 ms among 96,000 records
  (NFR01, NFR05). Works with no network (NFR03).
* Static analysis (cppcheck, clang-tidy) clean; built and tested automatically on every push.

### Installation
See [INSTALLATION_GUIDE.md](INSTALLATION_GUIDE.md). Download `LMS-1.0.0-windows.zip` from the
GitHub Releases page. Requires Windows 10/11 64-bit; no internet or server needed.

### Known limitations
* Only one person can use the system at a time (single computer, by design).
* Extensions are for individual students only; class-wide extensions are planned (CR1).
* Lecturers cannot list the students on their course; the administrator gives them student ids.
* If the only administrator forgets their password it cannot be reset; create a second administrator.
* No maximum file size for submissions.

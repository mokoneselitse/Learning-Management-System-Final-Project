# Quality Assurance

Static analysis, code metrics and a self code review of the Moderated LMS. Every finding was either
fixed or accepted with a reason, and all tests were re-run after the changes:
46/46 automated tests (320 checks), 20/20 system tests, 97% business-logic coverage, 0 compiler warnings.

## 1. Static analysis

Two analysers were run on all of `src/`:

* **cppcheck 2.13** (`--enable=all`): style, performance, portability and unused-code checks.
  It also runs in the CI pipeline on every push.
* **clang-tidy 18** (`bugprone-*`, `performance-*`): patterns that often lead to bugs.

| Analyser | Findings before | After | Raw reports |
|---|---|---|---|
| cppcheck | 29 | 0 (2 accepted, suppressed with a reason) | [before](qa/cppcheck_before.txt), [after](qa/cppcheck_after.txt) |
| clang-tidy | 14 | 0 | [before](qa/clang_tidy_before.txt), [after](qa/clang_tidy_after.txt) |

| Finding | Count | Decision | Change |
|---|---|---|---|
| `Session& s` could be `const Session& s` (cppcheck `constParameterReference`) | 26 | Fixed | Services never change the session, so every service now takes `const Session&` |
| Raw loop could be a standard algorithm (cppcheck `useStlAlgorithm`) | 1 | Fixed | `MarkingService::submissionsFor` uses `list.insert(...)` |
| `isFeedbackVisibleToStudent` / `isMarkVisibleToStudent` never used (cppcheck `unusedFunction`) | 2 | Accepted | They are the C1 rule in the domain model and are used by unit test UT-16; cppcheck only scans `src/`. Suppressed in the code with a comment |
| `24 * 60 * 60` calculated as `int` then widened (clang-tidy) | 1 | Fixed | Written as `24LL * 60 * 60` |
| Optional value used without a visible check (clang-tidy) | 11 | Fixed | `SubmissionService::submit` restructured so each branch is clearly safe; menus read an optional once into a variable before using it |
| Empty `catch` blocks in number input (clang-tidy) | 2 | Fixed | `readInt` / `readDouble` now use `std::from_chars`, which reports a bad number without throwing; spaces around a number are trimmed |

## 2. Code metrics

Measured with **lizard 1.24** ([before](qa/lizard_metrics_before.txt), [after](qa/lizard_metrics_after.txt)).
NLOC = lines of code without blanks and comments; CCN = cyclomatic complexity (1 + number of decisions).

### Size

| Part | Files | Lines | NLOC | Functions | Average CCN |
|---|---|---|---|---|---|
| Domain layer (`src/domain`) | 14 | 683 | 525 | 97 | 1.5 |
| Data access layer (`src/data`) | 20 | 1,073 | 880 | 81 | 1.6 |
| Business logic layer (`src/logic`) | 21 | 929 | 721 | 60 | 2.3 |
| Presentation layer (`src/presentation` + `main.cpp`) | 11 | 854 | 694 | 56 | 3.1 |
| **Total program** | **66** | **3,539** | **2,820** | **294** | **2.0** |
| Tests (`tests/`) | 13 | 1,238 | | | |

281 of the 294 functions (96%) have a CCN of 5 or less.

### Key functions

| Function | NLOC | CCN | Why it matters |
|---|---|---|---|
| `Submission::transitionTo` | 25 | 10 | The submission state machine (one `case` per state) |
| `SubmissionService::submit` | 35 | 8 | The four branches of the submit sequence diagram |
| `AssignmentService::createAssignment` | 15 | 5 | Ownership and input checks (FR05) |
| `StandardLatePenalty::apply` | 7 | 4 | The late-penalty rule (FR18) |
| `AuthService::login` | 10 | 4 | Login (FR20) |
| `MarkingService::mark` | 21 | 3 | Penalty + audit in one transaction (FR18, FR22) |
| `ModerationService::adjust` | 19 | 3 | Moderation with a reason (FR11) |
| `ModerationService::release` | 14 | 2 | Release of moderated marks only (FR12) |
| `SubmissionService::daysLate` | 6 | 2 | Rounds lateness up to whole days |
| `SqliteDatabase::runInTransaction` | 10 | 2 | Commit or roll back |

### Finding and change: menu functions too complex

Before the review, lizard flagged three functions over its limit of CCN 15: `AdminMenu::handle`
(125 NLOC, CCN 27), `StudentMenu::handle` (54, 17) and `LecturerMenu::handle` (81, 16). Each one held
the code of every menu option inside one big `switch`.

Each option was moved into its own small method (e.g. `AdminMenu::createUser`), and `handle()` now only
chooses the option:

| Function | Before (NLOC / CCN) | After (NLOC / CCN) |
|---|---|---|
| `AdminMenu::handle` | 125 / 27 | 23 / 16 |
| `StudentMenu::handle` | 54 / 17 | 13 / 6 |
| `LecturerMenu::handle` | 81 / 16 | 16 / 9 |
| `HodMenu::handle` | 47 / 9 | 14 / 7 |

`AdminMenu::handle` still scores 16 because lizard counts each of its 16 menu choices as a decision;
it is a flat list with no nesting. With lizard's modified count (`-m`, a whole `switch` counts as one
decision) no function in the program is above the limit.

## 3. Self code review

The code was reviewed against the checklist below, after the analysers were run.

| # | Checklist item | Result | Action |
|---|---|---|---|
| 1 | Class and method names match the design diagrams | Pass | Additions made during coding are listed in the report |
| 2 | Code follows CODING_STANDARD.md (naming, file headers, ≤ 100 columns, 4 spaces) | **15 lines over 100 columns** | R2 |
| 3 | Every public service method checks the user's permission first (FR21) | **`AuthService::nameOf` had no check** | R1 |
| 4 | All SQL uses `?` placeholders; no SQL is built from user input | Pass | — |
| 5 | Passwords are never stored or printed in plain text (NFR04) | Pass | — |
| 6 | Bad input or a database error cannot crash the program | Pass | Number input improved (R6) |
| 7 | A mark change and its audit entry are saved in one transaction (FR22) | Pass (IT-18) | — |
| 8 | No raw `new`/`delete`; resources are freed by destructors (RAII) | Pass | — |
| 9 | Parameters and methods are `const` where they can be | **26 places** | R4 |
| 10 | No function is too long or too complex | **3 menu functions** | R3 |
| 11 | No copied code | Minor: `findSubmission` appears in both MarkingService and ModerationService | Accepted: 5 lines, keeps each service independent |
| 12 | Every requirement has at least one test | Pass (Phase 5 traceability) | — |
| 13 | Tests are independent and repeatable | Pass (fixed clock, fresh database, 90 runs in random order) | — |

### Changes made as a result

| ID | Change | Reason | Retest |
|---|---|---|---|
| R1 | `AuthService::nameOf` refuses students | Names are personal data (NFR06); only staff screens need them | New check in IT-16 passes |
| R2 | Long lines re-wrapped; `.clang-format` added with the standard's rules | Coding standard | No line over 100 columns |
| R3 | Menu options split into one method each | Complexity (Section 2) | 20/20 system tests pass (menus behave the same) |
| R4 | `Session&` → `const Session&` in all services | cppcheck | All tests pass |
| R5 | `submit()` restructured; optionals read once; `24LL`; loop → `insert` | clang-tidy, cppcheck | All tests pass |
| R6 | Number input uses `std::from_chars` and trims spaces | clang-tidy (empty catch) | ST-20 passes; `" 5 "` accepted, `abc` and `99999999999` asked again |

### Known limitations (accepted, recorded for the future)

* A submitted file is copied before its database row is saved. If the database write fails, the
  copy stays in the `submissions` folder unused. This is rare and wastes only disk space.
* There is no maximum size for a submitted file.
* Acceptance test AT-02 (NFR02, usability with an untrained lecturer) still has to be run with a participant.

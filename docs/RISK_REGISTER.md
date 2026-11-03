# Risk Register – updated at project close (v1.1, November 2026)

The Phase 1 register ([Project Management Plan, Section 5](PHASE1_PROJECT_MANAGEMENT_PLAN.pdf)) scored
each risk as Likelihood × Impact on a 1–5 scale (exposure ≥ 15 High, 8–14 Medium, ≤ 7 Low). This update
records what happened to each risk and adds the risks that appeared during the project.
Owner of every risk: Project Lead (individual project).

## Original risks R1–R8

| ID | Risk | Phase 1 exposure | What happened | Status | Exposure now |
|---|---|---|---|---|---|
| R1 | Requirements and design overrun, squeezing implementation | 20 High | Phases 2–3 finished at their gates; implementation was built layer by layer (domain → data → logic → menus), each layer compiled and tested before the next | Closed | 0 |
| R2 | Stakeholder conflicts resolved wrongly, causing rework | 12 Medium | Conflicts C1–C4 resolved in writing in the SRS; C1 (marks hidden until release) is enforced in code, the database view and tests IT-12, ST-18. C3 (class-wide extension) came back as change request CR1 and was handled through change control, not rework | Closed | 0 |
| R3 | SQLite schema drifts from the class diagram | 9 Medium | The ERD and class diagrams were kept in step; CR1 changed both together (v1.1 diagrams in `images/`) | Closed | 0 |
| R4 | 70% coverage target missed | 9 Medium | Tests written with each layer; business-logic coverage 96%; CI fails any run below 70% | Closed | 0 |
| R5 | Password hashing done insecurely | 10 Medium | libsodium Argon2id with random salt; verified by IT-02 and AT-04 | Closed | 0 |
| R6 | Illness, hardware failure or disruption | 10 Medium | Work pushed to GitHub after each layer and phase; no loss occurred | Open until submission | 2 × 5 = 10 → 1 × 5 = 5 Low |
| R7 | Scope creep | 9 Medium | Only SRS requirements were built; extra ideas (lecturer class lists, file-size limit) were recorded as known limitations instead | Closed | 0 |
| R8 | A design decision cannot be explained at the viva | 8 Medium | Decisions recorded in the design document, CR1 analysis (Section 3) and the code review records | Open until viva | 2 × 4 = 8 |

## New risks identified during the project

| ID | Risk | Type | L | I | Exposure | Mitigation / outcome | Status |
|---|---|---|---|---|---|---|---|
| R9 | Windows build tools (MSYS2) fail to install or download over a slow connection | Technical | 4 | 3 | 12 Medium | Happened in Phase 4: fixed by `pacman -Syu --disable-download-timeout`, one download at a time and installing packages one by one; build steps written into the README | Closed |
| R10 | GitHub push problems (credentials, token scopes, unrelated histories) delay deliverables | Technical | 3 | 3 | 9 Medium | Happened in Phases 4 and 7: personal access token with `repo` and `workflow` scopes; pull-and-merge before push; branching rules in CONFIGURATION_MANAGEMENT.md | Closed |
| R11 | A change request arrives after release and breaks existing behaviour | Technical | 3 | 4 | 12 Medium | Happened as CR1: impact analysis first, full v1.0 suite re-run as regression (CR1_CHANGE_ANALYSIS.md Section 4) | Closed |
| R12 | Submission format changed late (report limited to 4–5 pages, diagrams as external files, video) | Project | 4 | 3 | 12 Medium | Detail moved into linked documents in `docs/` and `images/`; report kept as a summary with links | Closed |
| R13 | Usability test AT-02 (NFR02) not run before submission because no participant is available | Project | 3 | 3 | 9 Medium | Procedure written (Phase 5, Section 6); run with a classmate before the viva; if not done, report it as a gap | Open |
| R14 | Data held on the school computer is copied or changed outside the program (database not encrypted) | Legal / security | 2 | 4 | 8 Medium | Folder permissions, separate Windows accounts and BitLocker in the installation guide, Section 4a; encryption recommended for a later version | Open (transferred to the school) |
| R15 | Video recording exceeds GitHub's 100 MB file limit | Project | 3 | 2 | 6 Low | Record at 720p; if still too large, upload to a video host and link it from the README | Open |

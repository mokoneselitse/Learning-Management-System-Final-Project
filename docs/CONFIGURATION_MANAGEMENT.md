# Configuration Management

How the Moderated LMS source code, documents and releases are controlled.

## 1. What is under version control

Everything that makes up the project is in this one Git repository on GitHub:

| Folder / file | Contents |
|---|---|
| `src/` | Source code, one folder per layer (domain, data, logic, presentation) |
| `tests/` | Unit, integration, system and acceptance tests |
| `CMakeLists.txt`, `.clang-format` | Build script and formatting rules |
| `.github/workflows/ci.yml` | The CI pipeline |
| `docs/` | Report, phase documents, QA records, user manuals |
| `images/` | All diagrams (PNG) referenced from the report |
| `README.md`, `CODING_STANDARD.md` | Build guide and coding standard |

Generated files are **not** committed (see `.gitignore`): the `build/` folders, databases (`*.db`),
stored submissions, coverage HTML and test-run output. They can always be re-created from the source.

## 2. Branching strategy

The project uses **feature branches merged by pull request**:

```
main ──●────────────●──────────────●──────●────▶   (always builds, always passes CI)
        \          /  \            /       |
         feature/ci   feature/deployment-docs   tag v1.0, later v1.1
```

1. `main` always holds working, tested code. Nobody commits to `main` directly.
2. Each piece of work gets its own branch from `main`, named after what it does:
   `feature/<name>` for new work (e.g. `feature/class-extension`), `fix/<name>` for bug fixes
   (e.g. `fix/mark-message-format`), `docs/<name>` for documents.
3. When the work is finished, the branch is pushed and a **pull request** (PR) is opened into `main`.
4. The CI pipeline runs on the PR. The PR is merged only when CI shows a green tick and the
   change has been reviewed against the code review checklist (QUALITY_ASSURANCE.md).
5. After merging, the branch is deleted.

Typical commands:

```bash
git checkout main
git pull
git checkout -b feature/class-extension     # start new work
# ... edit, build, test ...
git add .
git commit -m "Add class-wide deadline extension (CR1)"
git push -u origin feature/class-extension  # then open a pull request on GitHub
```

## 3. Commit messages

* One logical change per commit.
* Short summary in the imperative, saying what changed: `Add integration tests for moderation`,
  `Fix mark range message showing six decimals (DEF-02)`.
* Mention the requirement, defect or change-request ID when there is one (FR13, DEF-02, CR1).

## 4. Releases and version numbers

Releases use **semantic versioning** (MAJOR.MINOR.PATCH) and are marked with an annotated Git tag:

| Version | Tag | Contents |
|---|---|---|
| 1.0.0 | `v1.0` | First release: all Phase 1–7 work (Phase 7) |
| 1.1.0 | `v1.1` | Change request CR1: class-wide extensions (Phase 8) |

```bash
git tag -a v1.0 -m "Release 1.0: first release of the Moderated LMS"
git push origin v1.0
```

The version number is also set in `CMakeLists.txt` (`project(ModeratedLMS VERSION ...)`), and every
release has notes in `docs/RELEASE_NOTES.md`.

## 5. Continuous integration

`.github/workflows/ci.yml` runs on **every push to every branch and on every pull request**, on a
clean Ubuntu 24.04 machine:

| Step | Fails the run when |
|---|---|
| Static analysis (cppcheck) | cppcheck reports any finding |
| Build | the code does not compile, or there is any compiler warning |
| Unit + integration tests (`ctest`) | any of the 51 test cases fails |
| System tests | any of the 22 system tests fails |
| Coverage | business-logic line coverage falls below 70% |

The coverage report, system-test transcripts and results are saved with every run
(**Actions → the run → Artifacts → test-reports**). The pipeline was checked before use: with a
planted bug in the penalty rule the run failed at the test step, and with a planted compiler warning it
failed at the static-analysis step.

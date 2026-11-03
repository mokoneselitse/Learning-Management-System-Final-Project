# Professional, Legal and Ethical Issues

The Moderated LMS holds learners' personal information and their marks, so it raises legal duties
(data protection, disability equity, software licences) and ethical ones (fairness, honesty,
professional responsibility). For each issue, this section gives what is required, how the design
meets it, and what remains open.

## 1. Data protection of learners' personal information

**Law.** Lesotho's *Data Protection Act, 2011* (Act No. 5 of 2012) applies to the school as the data
controller. It requires personal information to be collected for a specified purpose and kept to the
minimum needed (s.16, s.18), kept accurate (s.24), not kept longer than necessary (s.19), and secured
against loss and unlawful access (s.20). Data subjects may see and correct their information
(s.26, s.27), and security breaches must be reported to the Data Protection Commission and to the
people affected (s.23). Information about a **child** (anyone under 18) is *sensitive personal
information* (s.29) and may generally be processed only with prior parental consent (s.36). Most of
the school's learners are children.

**How the design addresses it**

* **Minimal data:** only a name, a username and a password hash are stored about each person; there is
  no date of birth, address, phone number or ID number.
* **One purpose:** the data is used only for assignments and marks, and is never sent anywhere. The
  system runs offline on one computer (NFR03), so there is no cloud provider or third party.
* **Access control (NFR06):** every service method checks the user's role first (FR21), and ownership
  is checked too: lecturers see only their own courses, HoDs only their own department, students only
  their own results. Students cannot even look up other users' names (code-review finding R1).
* **Accuracy and correction:** the administrator can correct names and usernames (FR01); every change to
  a mark is recorded with who, when and why.

**Open issues (recommendations to the school)**

* The school must obtain **parental consent** for learners under 18, or confirm another lawful basis
  under s.36, before the system is used. This is an organisational duty that software cannot do.
* The system has **no deletion or retention feature**: users can only be deactivated. The school should
  set a retention period (s.19) and a later version should add de-identification of old records.
* The school needs a simple **breach procedure** (who tells the Commission and the learners, s.23).

## 2. Security of assessment records

Marks decide learners' progress, so they must be protected from being seen early, changed without
trace, or lost.

| Threat | Design response |
|---|---|
| Stolen or guessed passwords | Passwords stored only as salted **Argon2id** hashes (libsodium, NFR04); hidden while typed; same message for a wrong username or password |
| Marks seen before moderation | Marks visible only after HoD release (FR13); enforced in `Submission`, the services *and* the `student_result_view` in the database |
| Unauthorised or untraceable mark changes | Role and ownership checks; every mark change written to an **append-only audit log** in the same transaction as the change (FR22); database triggers block edits and deletes |
| SQL injection | All SQL uses `?` placeholders (checked in the code review) |
| Loss of data | Weekly backup procedure in the installation guide; submitted files copied into the data folder |

**Open issue.** The database file itself is not encrypted. Anyone who can open `C:\LMS\data` in Windows
could copy it or change it directly, bypassing the program's checks. The installation guide (Section
4a) therefore asks for separate Windows accounts, restricted folder permissions and BitLocker where
available.

## 3. Accessibility for users with disabilities

Lesotho's *Persons with Disability Equity Act, 2021* (Act 2 of 2021) prohibits discrimination on the
basis of disability, requires inclusive education (s.23) and reasonable accommodation, and gives a right
to information in an accessible format (s.26, s.34).

**Strengths of the design:** the console interface is plain text, so the built-in Windows screen reader
(Narrator) and free readers such as NVDA can read every screen. It is fully keyboard-operated, needs no
mouse, uses no colour to carry meaning, and has no time limits on any input. Text size can be increased
in the console window settings. Short numbered menus (NFR02) help users with cognitive impairments.

**Limitations:** there are no audio cues; long tables (for example the HoD review list) are tiring to
hear through a screen reader; and the single shared computer may not be physically accessible to every
learner. As reasonable accommodation, a lecturer can grant an individual extension (FR06), and a
staff member can sit with a learner and operate the keyboard for them while they log in and submit. A future version could add a "simple list"
mode with fewer columns.

## 4. Fairness of the late-penalty rules

The rule (10% of the maximum mark per day late, up to three days, then zero, FR18) was set by the Head
of Department, and late work is always accepted instead of being refused (C2).

**Fair by design:** the rule is applied identically to everyone by one piece of code
(`StandardLatePenalty`); students are told the rule in the user manual; the days late are shown to the
student, lecturer and HoD; feedback is given even on zero-mark work; individual and class-wide
extensions (FR06, FR24) with a recorded reason cover illness and other good causes; and the HoD can
adjust a mark with a recorded reason (FR11).

**Fairness concerns raised honestly:**

* **Rounding up:** one second late counts as a whole day (ceiling rule), which is harsh near midnight.
  This was the specified rule; the school could add a short grace period through a new strategy
  class without changing the rest of the system.
* **One computer:** learners queueing for a single computer just before a deadline may be late
  through no fault of their own. Lecturers should allow for this, or extend the deadline for the class.
* **The clock decides:** penalties depend on the computer's clock, so the installation guide asks the
  technician to keep it correct.

## 5. Professional responsibility and licensing

The work follows the *ACM/IEEE-CS Software Engineering Code of Ethics*. Under **Product**, it was
tested at four levels with every defect logged and retested (Phase 5); under **Public** and
**Client**, known limitations are stated openly in the release notes rather than hidden; under
**Judgement**, design decisions (such as the class-extension rule in CR1) record the alternatives
rejected and why.

All third-party code is used within its licence (see `THIRD_PARTY_LICENSES.md`). SQLite is public
domain. libsodium is ISC-licensed, which requires its notice to accompany every copy, so the release
zip now includes the licence files. Catch2 (Boost Software License) is used only for testing. No
copyleft (GPL) library is linked into the program, so the school is not required to publish its
source code.

## References

* Data Protection Act, 2011 (Act No. 5 of 2012), Lesotho. https://lesotholii.org/akn/ls/act/2012/5
* Persons with Disability Equity Act, 2021 (Act No. 2 of 2021), Lesotho. https://lesotholii.org/akn/ls/act/2021/2
* ACM/IEEE-CS Joint Task Force (1999) *Software Engineering Code of Ethics and Professional Practice*.
* libsodium documentation, licence (ISC). https://doc.libsodium.org/
* SQLite copyright (public domain). https://sqlite.org/copyright.html

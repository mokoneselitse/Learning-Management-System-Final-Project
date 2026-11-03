# User Manual – Head of Department (HoD)

The HoD checks (moderates) the marks that lecturers in the department have given, and releases them to
students. Students never see a mark until the HoD has moderated and released it.

## Logging in and out

Start **School LMS** from the desktop. Type your username and password (the password is not shown while
you type). Choose **0** to log out when you finish.

## Your menu

```
=== Head of Department menu (Teboho Lebona) ===
1. Review marks waiting for moderation
2. Approve a mark
3. Adjust a mark
4. Release marks for an assignment
5. View the audit log of a submission
0. Log out
```

## Moderating and releasing marks

1. **Review** (option 1). You see every marked piece of work in your department that has not been
   moderated yet:
   ```
     [1] assignment 1, Nthabiseng Molapo  | raw 42  | days late 0  | final 42
         feedback: Good structure; add more tests.
   ```
   *raw* is the lecturer's mark, *final* is the mark after any late penalty.
2. For each one, either:
   * **Approve** it as it is (option 2, give the submission id in brackets), or
   * **Adjust** it (option 3): give the submission id, the new mark and the **reason**. The reason is
     required and is saved in the audit log with the old and new mark.
     ```
     Submission id: 1
     New mark: 44
     Reason for the change: Testing section deserved more credit
     Mark adjusted. The change is recorded in the audit log.
     ```
3. **Release** (option 4) with the assignment id. All approved or adjusted marks of that assignment
   become visible to the students: `1 mark(s) released.` Work that you have not moderated yet stays
   hidden, so you can release in stages.

Released marks are final and cannot be changed.

## Audit log

Option 5 shows every change to a submission's mark: when, who, old mark, new mark and why.
```
  2026-11-03 16:28  MARK by Mpho Mokoena: - -> 42  (Initial mark)
  2026-11-03 16:28  MODERATE by Teboho Lebona: 42 -> 44  (Testing section deserved more credit)
```
The audit log cannot be edited or deleted by anyone.

## Messages you may see

| Message | Meaning |
|---|---|
| `Nothing is waiting for moderation.` | No new marks in your department |
| `This assignment is not in your department.` | You can only moderate your own department |
| `Reason cannot be empty.` | Adjusting a mark always needs a reason |
| `Only a MARKED submission can be approved.` | It was already approved or adjusted |
| `0 mark(s) released.` | Nothing for that assignment has been moderated yet |

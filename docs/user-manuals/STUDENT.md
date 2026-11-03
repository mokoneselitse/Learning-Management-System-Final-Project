# User Manual – Student

Students see their courses and assignments, hand in work, and read their feedback and released marks.

## Logging in and out

Go to the school computer and start **School LMS** from the desktop. Type your username and password
(the password is not shown while you type). **Always choose 0 to log out** before you leave, so nobody
else can use your account.

## Your menu

```
=== Student menu (Nthabiseng Molapo) ===
1. View my courses
2. View my assignments
3. Submit an assignment
4. View my results and feedback
0. Log out
```

## See your assignments

Option **2** lists every assignment for your courses with its id, deadline and maximum mark:
```
  [1] Assignment 1  | deadline 2026-11-08 23:59  | max mark 50
```
If your lecturer gave you an extension, you see your own deadline too:
```
  [1] Assignment 1  | deadline 2026-11-08 23:59 (extended to 2026-11-10 23:59 for you)  | max mark 50
```

## Hand in work

1. Bring your file on a USB drive and plug it in. In File Explorer, note the drive letter
   (for example `E:`) and the file name.
2. Choose **3**, type the assignment id, then the full path to your file:
   ```
   Assignment id: 1
   Path to your file: E:\Essay.pdf
   Submitted. Status: SUBMITTED, attempt 1
   ```
   The system keeps its own copy, so you can unplug the USB drive straight away.

**Rules**

* You can hand in again **before the deadline**; the newest version replaces the old one
  (`attempt 2`, `attempt 3`, …).
* After the deadline your work is still accepted but marked **LATE**. A late penalty is taken off:
  10% of the maximum mark per day late, up to 3 days; after 3 days the mark is 0.
* Late work, or work that has been marked, cannot be handed in again.

## Read your feedback and marks

Option **4**:
```
  Assignment 1  | MARKED
    Mark: not released yet
    Feedback: Good structure; add more tests.
```
You can read your lecturer's **feedback as soon as your work is marked**. The **mark** appears only after
the Head of Department has checked and released it:
```
  Assignment 1  | RELEASED
    Mark: 44 / 50
    Feedback: Good structure; add more tests.
```

## Messages you may see

| Message | Meaning |
|---|---|
| `Invalid username or password.` | Check your username and password; ask the administrator if you forgot it |
| `File not found: …` | The path is wrong; check the drive letter and file name |
| `Resubmission is not allowed.` | The deadline has passed, or your work is late or already marked |
| `You are not enrolled on this course.` | Ask the administrator to enrol you |

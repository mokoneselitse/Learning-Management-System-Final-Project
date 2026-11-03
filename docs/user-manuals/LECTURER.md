# User Manual – Lecturer

Lecturers create assignments for their courses, give individual deadline extensions, and mark students'
work with written feedback.

## Logging in and out

Start **School LMS** from the desktop. Type your username and password (the password is not shown while
you type). Choose **0** to log out when you finish.

## Your menu

```
=== Lecturer menu (Mpho Mokoena) ===
1. View my courses
2. View my assignments
3. Create an assignment
4. Grant a deadline extension
5. View submissions for an assignment
6. Mark a submission
7. Re-mark a submission
8. Extend the deadline for the whole class
0. Log out
```

## Create an assignment

1. Choose **1** to see your courses and their id numbers: `[1] CS2201  Programming II`.
2. Choose **3** and answer the questions:
   ```
   Course id (see option 1): 1
   Title: Assignment 1
   Description: Write a linked list
   Deadline (YYYY-MM-DD HH:MM): 2026-11-08 23:59
   Maximum mark: 50
   Assignment created with id 1.
   ```
   Type the deadline exactly in that format (year-month-day, then 24-hour time). It must be in the
   future, and the maximum mark must be more than 0.
3. Choose **2** to check it. You can sort by deadline (1) or by date created (2).

## Grant a deadline extension

Choose **4**, then give the assignment id, the student's id, the new deadline and a reason
(the reason is required). The new deadline must be later than the original one. Only that student gets
the extra time, and they see it on their assignment list.

You need the student's **id number**: ask the administrator for it, or find it in option 5 once the
student has submitted.

## Extend the deadline for the whole class (new in version 1.1)

Choose **8**, give the assignment id, the new deadline and a reason:
```
Assignment id: 1
New deadline for the whole class (YYYY-MM-DD HH:MM): 2026-11-10 23:59
Reason: Exam week clash
Deadline extended to 2026-11-10 23:59 for every student on the course.
```
* It must be done **before the original deadline**. After the deadline, use individual extensions
  (option 4) for students who need more time.
* The new deadline must be later than the original one, and a reason is required.
* Doing it again replaces the earlier class extension.
* Students see the new deadline on their assignment list. If a student also has an individual
  extension, the later of the two dates applies.

## Mark work

1. Choose **5** and give the assignment id to see what has been handed in:
   ```
     [1] Nthabiseng Molapo (id 4)  | SUBMITTED  | days late 0  | attempt 1  | file …\a1_s4_attempt1.pdf
   ```
   The file shown is the school's copy of the student's work; open it from File Explorer.
2. Choose **6**, give the submission id (the number in brackets), the mark and your feedback:
   ```
   Submission id: 1
   Mark (before any late penalty): 42
   Feedback: Good structure; add more tests.
   Marked. The student can now read the feedback; the mark stays hidden until the HoD releases it.
   ```
   * Type the mark the work deserves. **The system takes off the late penalty itself**: 10% of the
     maximum mark for each day late, up to 3 days; work more than 3 days late gets 0.
   * The mark must be between 0 and the maximum. Feedback is required.
   * The student can read your feedback straight away, but **not the mark**: the Head of Department
     checks and releases marks.

## Change a mark

Choose **7** (re-mark) for work you have already marked. Give the submission id, the new mark and the
**reason for the change**. The old mark, new mark, your name, the time and the reason are saved in the
audit log. Once the HoD has moderated a mark, only the HoD can change it.

## Messages you may see

| Message | Meaning |
|---|---|
| `You are not the lecturer of this course.` | Check the course id with option 1 |
| `Mark must be between 0 and 50.` | The mark is above the assignment's maximum (or negative) |
| `This submission is already marked. Use re-mark instead.` | Use option 7 |
| `This submission is not for one of your assignments.` | Check the submission id with option 5 |
| `Please use the format YYYY-MM-DD HH:MM` | Type e.g. `2026-11-08 23:59` |
| `The original deadline has passed. Use individual extensions for late cases.` | A class extension is only possible before the deadline (option 8) |

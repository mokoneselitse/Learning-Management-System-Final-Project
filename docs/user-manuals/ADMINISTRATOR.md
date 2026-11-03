# User Manual – Administrator

The Administrator sets the system up: user accounts, departments, courses, which lecturer teaches a
course and which students take it. The Administrator can also read the audit log of mark changes.

## Logging in and out

Start **School LMS** from the desktop. Type your username and password (the password is not shown while
you type). To finish, choose **0** to log out; to close the program, press Enter on an empty username.
Never leave the computer while you are logged in.

## Your menu

```
=== Administrator menu (Thabo Mokoena) ===
-- Users --
 1. Create a user
 2. Edit a user
 3. Reset a user's password
 4. Activate / deactivate a user
 5. List all users
-- Departments and courses --
 6. Create a department
 7. Change a department's HoD
 8. List departments
 9. Create a course
10. Edit a course
11. List courses
12. Assign a lecturer to a course
13. Enrol a student on a course
-- Other --
14. View the audit log of a submission
 0. Log out
```

Most options ask for an **id number** (a user id, course id or department id). The id is shown when
something is created, and options **5**, **8** and **11** list them at any time.

## Setting up a new term (recommended order)

1. **Create the users** (option 1): Heads of Department first, then lecturers, then students.
   Choose the role by number: 1 = Student, 2 = Lecturer, 3 = HoD, 4 = Admin. Usernames are 3 to 32
   characters with no spaces and must be unique.
   ```
   Role (1 = Student, 2 = Lecturer, 3 = HoD, 4 = Admin): 2
   Username: mmokoena
   Full name: Mpho Mokoena
   Password:
   User created with id 2.
   ```
2. **Create each department** (option 6) and give the HoD's user id (or 0 if there is no HoD yet).
   A HoD can head only one department.
3. **Create each course** (option 9) with its code, title and department id.
4. **Assign the lecturer** to each course (option 12): course id, then the lecturer's user id.
5. **Enrol the students** (option 13): student's user id, then course id. Repeat for each course.
6. Check with options **5** and **11**:
   ```
     [1] CS2201  Programming II  | department 1  | lecturer: Mpho Mokoena
   ```

## Everyday tasks

| Task | Option | Notes |
|---|---|---|
| Correct a name or username | 2 | The username must still be unique |
| A user forgot their password | 3 | Type a new password and give it to the user privately |
| Someone leaves the school | 4, then 0 | Deactivated users cannot log in, but their work and marks are kept |
| A user returns | 4, then 1 | Reactivates the account |
| Change a course code or title | 10 | |
| New HoD for a department | 7 | Type 0 to leave the department without a HoD |
| See who changed a mark, when and why | 14 | Ask the HoD for the submission id |

## Messages you may see

| Message | Meaning |
|---|---|
| `Username '…' is already taken.` | Choose another username |
| `That user is not a Lecturer.` / `… not a Student.` / `… not a Head of Department.` | Check the user id with option 5 |
| `That Head of Department already heads a department.` | A HoD can only head one department |
| `The student is already enrolled on this course.` | Nothing to do |
| `You cannot deactivate your own account.` | Ask the second administrator |

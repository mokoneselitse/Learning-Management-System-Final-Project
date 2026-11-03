# Installation Guide – Moderated LMS 1.1

For the school's IT technician. No programming knowledge is needed. Installing takes about 10 minutes.

The system runs on **one Windows computer** (for example in the staff room). It needs **no internet
and no server**: everything is stored in a folder on that computer. Staff and students log in one at a
time at that computer. See the [deployment diagram](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/blob/main/images/deployment_diagram.png).

## 1. What you need

| Item | Requirement |
|---|---|
| Computer | Windows 10 or Windows 11, 64-bit |
| Disk space | About 50 MB, plus space for submitted files |
| Internet | Not needed (only to download the release once) |
| Release file | `LMS-1.1.0-windows.zip` from the [GitHub Releases page](https://github.com/mokoneselitse/Learning-Management-System-Final-Project/releases), or copied on a USB drive |
| For backups | A USB flash drive |

The zip contains `lms.exe` (the program), several `.dll` files the program needs, this guide, the user
manuals and the release notes. **Keep all these files together in one folder.**

## 2. Install

1. Copy `LMS-1.1.0-windows.zip` to the computer (for example to the Desktop).
2. Right-click the zip → **Extract All…** → type `C:\` as the destination → **Extract**.
   This creates the folder `C:\LMS-1.1.0-windows`.
3. Rename that folder to `C:\LMS` (right-click → **Rename**).
4. Check the program works: press **Windows key + R**, type `cmd`, press Enter, and type:
   ```
   C:\LMS\lms.exe --version
   ```
   It should print `Moderated LMS version 1.1.0`. Close the window.

## 3. Create the desktop shortcut

1. Right-click the desktop → **New** → **Shortcut**.
2. For the location, type exactly:
   ```
   C:\LMS\lms.exe C:\LMS\data\lms.db
   ```
   then click **Next**, name it `School LMS`, and click **Finish**.
3. Right-click the new shortcut → **Properties** → in **Start in** type `C:\LMS` → **OK**.

All the school's data will be kept in `C:\LMS\data` (the database file `lms.db` and a `submissions`
folder). The folder is created automatically the first time the program starts.

## 4. First start: create the administrator

Double-click **School LMS**. Because the database is empty, the program asks for the administrator:

```
First start: please create the administrator account (username: admin).
Full name: Thabo Mokoena
Password:                       (not shown while typing)
Administrator created. You can now log in as 'admin'.

===== Moderated LMS 1.1.0 =====

Username (leave empty to quit): admin
Password:
Welcome, Thabo Mokoena.
```

Then, from the Administrator menu (see the [Administrator manual](user-manuals/ADMINISTRATOR.md)):

1. **Create a second administrator account** (for example for the Principal). If the only administrator
   forgets their password, nobody else can reset it.
2. Create the Heads of Department, then the departments, the lecturers, the courses and the students,
   then assign lecturers and enrol students.
3. Give each person their username and first password, and ask them to keep it private.

To close the program, choose **0** (log out), then press Enter on an empty username.

## 5. How students hand in files

Students bring their work on a USB drive. When they choose **Submit an assignment** they type the file's
full path, for example `E:\Essay.pdf` (the USB drive letter may differ; check in File Explorer).
The program **copies** the file into `C:\LMS\data\submissions`, so the USB drive can be removed straight
after submitting.

## 6. Backup (do this every week)

1. Make sure nobody is using the program (close its window).
2. Copy the whole folder `C:\LMS\data` to a USB drive and rename the copy with the date,
   for example `LMS-backup-2026-11-14`.
3. Keep at least the last four backups, and keep the USB drive away from the computer.

**Restore:** close the program, rename the broken `C:\LMS\data` to `C:\LMS\data-old`, copy the backup
folder back to `C:\LMS\data`, and start the program.

## 7. Upgrade to a new version

1. Make a backup (Section 6).
2. Extract the new release zip somewhere temporary.
3. Copy `lms.exe`, all the `.dll` files, the guides and the user manuals over the old ones in `C:\LMS`.
   **Do not touch `C:\LMS\data`**: it holds all the school's data and is kept by the new version.
4. Check: `C:\LMS\lms.exe --version` shows the new version number.

**From 1.0 to 1.1:** nothing else is needed. The first time version 1.1 starts, it adds the new
`class_extension` table to the existing database automatically; all users, courses, submissions and
marks are kept.

## 8. Uninstall

Make a final backup if the data is needed, then delete the `C:\LMS` folder and the desktop shortcut.

## 9. Troubleshooting

| Problem | What to do |
|---|---|
| "The code execution cannot proceed because `libsqlite3-0.dll` (or another `.dll`) was not found" | The DLL files were separated from `lms.exe`. Extract the zip again and keep all files in `C:\LMS`. |
| "Cannot create the data folder …" | The path in the shortcut is wrong, or this Windows user cannot write there. Check Section 3, or use a folder the user owns. |
| "Fatal error: … unable to open database file" | Same as above: check the shortcut path and folder permissions. |
| "Error: database is locked" | The program is open in two windows at once. Close the extra window. |
| A user forgot their password | An administrator uses **Reset a user's password**. |
| The only administrator forgot their password | Restore can't fix this. That is why Section 4 asks for a second administrator account. |
| "File not found" when a student submits | The file path was typed wrongly. Check the drive letter and file name in File Explorer. |
| The window closes immediately | Start it from the shortcut, not by double-clicking `lms.exe` inside a zip. Run `C:\LMS\lms.exe --version` in `cmd` to see any message. |

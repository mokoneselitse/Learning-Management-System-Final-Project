# Third-party software

The Moderated LMS uses the following libraries. Their licences allow use in this program, including
giving the program to the school, as long as the notices below travel with it. The release zip
contains each library's own licence file in the `third-party-licenses` folder.

| Library | Used for | Licence | What the licence requires |
|---|---|---|---|
| [SQLite](https://sqlite.org/copyright.html) | Database | Public domain | Nothing |
| [libsodium](https://doc.libsodium.org/) | Argon2id password hashing | ISC | Keep its copyright and permission notice with every copy |
| GCC runtime (libstdc++, libgcc) | C++ standard library (Windows build) | GPL v3 with the GCC Runtime Library Exception | The exception allows programs built with GCC to be distributed under any licence |
| winpthreads | Thread support (Windows build) | MIT / ZPL-style | Keep its notice |
| [Catch2](https://github.com/catchorg/Catch2) | Automated tests only (not shipped) | Boost Software License 1.0 | Nothing for the shipped program |

No library under a "copyleft" licence such as the plain GPL is linked into the program, so the school
does not have to publish the source code. (It is public on GitHub anyway.)

// AdminService.h - administrator functions (FR01 - FR04).
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "IClock.h"
#include "IPasswordHasher.h"
#include "Repositories.h"
#include "Session.h"

class AdminService {
public:
    AdminService(IUserRepository& users, ICourseRepository& courses, IPasswordHasher& hasher,
                 IClock& clock);

    int createUser(const Session& s, Role role, const std::string& username,
                   const std::string& name, const std::string& pw);
    void setUserActive(const Session& s, int userId, bool active);
    int createDepartment(const Session& s, const std::string& name, std::optional<int> hodId);
    int createCourse(const Session& s, const std::string& code, const std::string& title,
                     int departmentId);
    void assignLecturer(const Session& s, int courseId, int lecturerId);
    void enrolStudent(const Session& s, int studentId, int courseId);

    // Added during coding: edit users (FR01) and manage departments/courses (FR02).
    void editUser(const Session& s, int userId, const std::string& username,
                  const std::string& name);
    void resetPassword(const Session& s, int userId, const std::string& newPassword);
    void editCourse(const Session& s, int courseId, const std::string& code,
                    const std::string& title);
    void setDepartmentHod(const Session& s, int departmentId, std::optional<int> hodId);
    std::vector<std::unique_ptr<User>> listUsers(const Session& s);
    std::vector<Department> listDepartments(const Session& s);
    std::vector<Course> listCourses(const Session& s);

private:
    static void checkUsername(const std::string& username);
    void checkCanBeHod(int hodId, int allowedDepartmentId);

    IUserRepository& users;
    ICourseRepository& courses;
    IPasswordHasher& hasher;
    IClock& clock;
};

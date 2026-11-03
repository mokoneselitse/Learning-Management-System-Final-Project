// Menus.h - one menu per role. Each shows only that role's options (FR21);
// the services still check permissions again through Session::require().
#pragma once

#include <memory>

#include "RoleMenu.h"

class StudentMenu : public RoleMenu {
public:
    using RoleMenu::RoleMenu;

protected:
    void showOptions() override;
    bool handle(int choice) override;

private:  // one method per menu option (added during coding)
    void viewMyCourses();
    void viewMyAssignments();
    void submitAssignment();
    void viewMyResults();
};

class LecturerMenu : public RoleMenu {
public:
    using RoleMenu::RoleMenu;

protected:
    void showOptions() override;
    bool handle(int choice) override;

private:  // one method per menu option (added during coding)
    void viewMyCourses();
    void viewMyAssignments();
    void createAssignment();
    void grantExtension();
    void viewSubmissions();
    void markSubmission();
    void remarkSubmission();
};

class HodMenu : public RoleMenu {
public:
    using RoleMenu::RoleMenu;

protected:
    void showOptions() override;
    bool handle(int choice) override;

private:  // one method per menu option (added during coding)
    void reviewMarks();
    void approveMark();
    void adjustMark();
    void releaseMarks();
    void viewAuditLog();
};

class AdminMenu : public RoleMenu {
public:
    using RoleMenu::RoleMenu;

protected:
    void showOptions() override;
    bool handle(int choice) override;

private:  // one method per menu option (added during coding)
    void createUser();
    void editUser();
    void resetPassword();
    void setUserActive();
    void listUsers();
    void createDepartment();
    void changeDepartmentHod();
    void listDepartments();
    void createCourse();
    void editCourse();
    void listCourses();
    void assignLecturer();
    void enrolStudent();
    void viewAuditLog();
};

// Factory: picks the menu subclass from the logged-in user's role,
// so main() never has to switch on the role.
class MenuFactory {
public:
    static std::unique_ptr<RoleMenu> create(Session& session, ServiceRegistry& services,
                                            std::istream& in, std::ostream& out);
};

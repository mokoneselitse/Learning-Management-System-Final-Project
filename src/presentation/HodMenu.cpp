#include "Menus.h"

void HodMenu::showOptions() {
    out << "=== Head of Department menu (" << session.user().getFullName() << ") ===\n"
        << "1. Review marks waiting for moderation\n"
        << "2. Approve a mark\n"
        << "3. Adjust a mark\n"
        << "4. Release marks for an assignment\n"
        << "5. View the audit log of a submission\n"
        << "0. Log out\n";
}

// handle() only chooses the option; each option is its own small method.
bool HodMenu::handle(int choice) {
    switch (choice) {
        case 1: reviewMarks(); return true;
        case 2: approveMark(); return true;
        case 3: adjustMark(); return true;
        case 4: releaseMarks(); return true;
        case 5: viewAuditLog(); return true;
        case 0:
            return false;
        default:
            out << "Unknown option.\n";
            return true;
    }
}

// FR10
void HodMenu::reviewMarks() {
    std::vector<Submission> list = services.moderation.pendingForDepartment(session);
    if (list.empty()) out << "Nothing is waiting for moderation.\n";
    for (const Submission& s : list) {
        out << "  [" << s.getId() << "] assignment " << s.getAssignmentId()
            << ", " << services.auth.nameOf(session, s.getStudentId())
            << "  | raw " << s.getRawMark().value_or(0)
            << "  | days late " << s.getDaysLate()
            << "  | final " << s.getMark().value_or(0)
            << "\n      feedback: " << s.getFeedback() << "\n";
    }
}

void HodMenu::approveMark() {
    int id = readInt("Submission id: ");
    services.moderation.approve(session, id);
    out << "Mark approved.\n";
}

// FR11
void HodMenu::adjustMark() {
    int id = readInt("Submission id: ");
    double newMark = readDouble("New mark: ");
    std::string reason = readLine("Reason for the change: ");
    services.moderation.adjust(session, id, newMark, reason);
    out << "Mark adjusted. The change is recorded in the audit log.\n";
}

// FR12
void HodMenu::releaseMarks() {
    int assignmentId = readInt("Assignment id: ");
    int count = services.moderation.release(session, assignmentId);
    out << count << " mark(s) released.\n";
}

// FR22
void HodMenu::viewAuditLog() {
    int id = readInt("Submission id: ");
    printAuditLog(services.audit.history(session, id));
}

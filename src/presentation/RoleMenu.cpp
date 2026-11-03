#include "RoleMenu.h"

#include <charconv>
#include <system_error>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Errors.h"
#include "HiddenInput.h"

RoleMenu::RoleMenu(Session& session, ServiceRegistry& services, std::istream& in,
                   std::ostream& out)
    : session(session), services(services), in(in), out(out) {}

void RoleMenu::run() {
    bool keepGoing = true;
    while (keepGoing) {
        out << "\n";
        showOptions();
        int choice = readInt("Choice: ");
        try {
            keepGoing = handle(choice);
        } catch (const LmsError& e) {
            // Every rule the services break comes back as an LmsError,
            // so the user sees the message and the menu carries on.
            out << "Error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            // Anything unexpected (e.g. from the standard library) must not crash the program.
            out << "Unexpected error: " << e.what() << "\n";
        }
        if (!in) keepGoing = false;  // input closed (e.g. Ctrl+Z / Ctrl+D)
    }
}

std::string RoleMenu::readLine(const std::string& prompt) {
    out << prompt;
    std::string line;
    if (!std::getline(in, line)) return "";
    if (!line.empty() && line.back() == '\r') line.pop_back();  // Windows line endings
    return line;
}

std::string RoleMenu::readPassword(const std::string& prompt) {
    out << prompt;
    return readHiddenLine(in, out);
}

// Removes spaces and tabs at both ends, so " 5 " is read as 5.
static std::string trimSpaces(const std::string& text) {
    size_t first = text.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = text.find_last_not_of(" \t");
    return text.substr(first, last - first + 1);
}

int RoleMenu::readInt(const std::string& prompt) {
    while (true) {
        std::string line = trimSpaces(readLine(prompt));
        if (!in) return 0;
        // from_chars reports a bad number through its result instead of throwing.
        int value = 0;
        const char* end = line.data() + line.size();
        std::from_chars_result r = std::from_chars(line.data(), end, value);
        if (!line.empty() && r.ec == std::errc() && r.ptr == end) return value;
        out << "Please enter a whole number.\n";
    }
}

double RoleMenu::readDouble(const std::string& prompt) {
    while (true) {
        std::string line = trimSpaces(readLine(prompt));
        if (!in) return 0;
        double value = 0;
        const char* end = line.data() + line.size();
        std::from_chars_result r = std::from_chars(line.data(), end, value);
        if (!line.empty() && r.ec == std::errc() && r.ptr == end) return value;
        out << "Please enter a number, e.g. 42 or 42.5.\n";
    }
}

Timestamp RoleMenu::readDateTime(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        if (!in) return Timestamp{};
        std::tm tm{};
        std::istringstream text(line);
        text >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        if (!text.fail()) {
            tm.tm_isdst = -1;  // let mktime work out daylight saving
            std::time_t t = std::mktime(&tm);
            if (t != -1) return std::chrono::system_clock::from_time_t(t);
        }
        out << "Please use the format YYYY-MM-DD HH:MM, e.g. 2026-11-07 23:59.\n";
    }
}

std::string RoleMenu::formatTime(Timestamp t) {
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream text;
    text << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return text.str();
}

void RoleMenu::printAuditLog(const std::vector<AuditEntry>& entries) {
    if (entries.empty()) out << "No changes recorded for this submission.\n";
    for (const AuditEntry& e : entries) {
        out << "  " << formatTime(e.getChangedAt()) << "  " << toString(e.getAction())
            << " by " << services.auth.nameOf(session, e.getChangedBy()) << ": ";
        std::optional<double> oldMark = e.getOldMark();
        if (oldMark) out << *oldMark; else out << "-";
        out << " -> " << e.getNewMark() << "  (" << e.getReason() << ")\n";
    }
}

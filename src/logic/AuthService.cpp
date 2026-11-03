#include "AuthService.h"

#include "Errors.h"

AuthService::AuthService(IUserRepository& users, IPasswordHasher& hasher)
    : users(users), hasher(hasher) {}

Session AuthService::login(const std::string& username, const std::string& password) {
    std::unique_ptr<User> user = users.findByUsername(username);
    // Same message for a wrong username and a wrong password, so nobody can
    // find out which usernames exist.
    if (user == nullptr || !hasher.verify(password, user->getPasswordHash())) {
        throw AuthenticationError("Invalid username or password.");
    }
    if (!user->isActive()) {
        throw AuthenticationError("This account has been deactivated.");
    }
    return Session(std::move(user));
}

std::string AuthService::nameOf(const Session& s, int userId) {
    // Names are personal data (NFR06): only staff screens show other users' names.
    if (s.user().getRole() == Role::STUDENT) {
        throw AccessDeniedError("Students cannot look up other users.");
    }
    std::unique_ptr<User> user = users.findById(userId);
    if (user == nullptr) return "(unknown user " + std::to_string(userId) + ")";
    return user->getFullName();
}

// Validation.h - small input checks shared by the services.
#pragma once

#include <sstream>
#include <string>

#include "Errors.h"

// True if the text is empty or only spaces/tabs/newlines.
inline bool isBlank(const std::string& text) {
    return text.find_first_not_of(" \t\r\n") == std::string::npos;
}

// Formats a mark for messages: 50 -> "50", 37.5 -> "37.5" (not "50.000000").
inline std::string formatNumber(double value) {
    std::ostringstream text;
    text << value;
    return text.str();
}

inline void requireNotBlank(const std::string& text, const std::string& fieldName) {
    if (isBlank(text)) throw ValidationError(fieldName + " cannot be empty.");
}

// ClassExtension.h - a deadline extension for every student on an assignment's
// course (change request CR1, v1.1). At most one per assignment; granting a new
// one replaces the old one.
#pragma once

#include <string>
#include <utility>

#include "Types.h"

class ClassExtension {
public:
    ClassExtension(int assignmentId, Timestamp newDeadline, std::string reason, int grantedBy,
                   Timestamp grantedAt)
        : assignmentId(assignmentId),
          newDeadline(newDeadline),
          reason(std::move(reason)),
          grantedBy(grantedBy),
          grantedAt(grantedAt) {}

    int getAssignmentId() const { return assignmentId; }
    Timestamp getNewDeadline() const { return newDeadline; }
    std::string getReason() const { return reason; }
    int getGrantedBy() const { return grantedBy; }
    Timestamp getGrantedAt() const { return grantedAt; }

private:
    int assignmentId;
    Timestamp newDeadline;
    std::string reason;
    int grantedBy;
    Timestamp grantedAt;
};

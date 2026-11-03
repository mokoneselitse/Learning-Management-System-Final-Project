#include "Assignment.h"

#include <utility>

Assignment::Assignment(int id, int courseId, std::string title, std::string description,
                       Timestamp deadline, double maxMark, int createdBy)
    : id(id),
      courseId(courseId),
      title(std::move(title)),
      description(std::move(description)),
      deadline(deadline),
      maxMark(maxMark),
      createdBy(createdBy) {}

int Assignment::getId() const { return id; }
int Assignment::getCourseId() const { return courseId; }
std::string Assignment::getTitle() const { return title; }
std::string Assignment::getDescription() const { return description; }
Timestamp Assignment::getDeadline() const { return deadline; }
double Assignment::getMaxMark() const { return maxMark; }
int Assignment::getCreatedBy() const { return createdBy; }

Timestamp Assignment::effectiveDeadlineFor(const Extension* ext) const {
    return effectiveDeadlineFor(nullptr, ext);
}

Timestamp Assignment::effectiveDeadlineFor(const ClassExtension* classExt,
                                           const Extension* ext) const {
    Timestamp result = deadline;
    if (classExt != nullptr && classExt->getNewDeadline() > result) {
        result = classExt->getNewDeadline();
    }
    if (ext != nullptr && ext->getNewDeadline() > result) {
        result = ext->getNewDeadline();
    }
    return result;
}

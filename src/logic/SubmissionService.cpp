#include "SubmissionService.h"

#include <chrono>
#include <filesystem>
#include <system_error>
#include <utility>

#include "Errors.h"
#include "Validation.h"

SubmissionService::SubmissionService(IAssignmentRepository& assignments,
                                     ICourseRepository& courses,
                                     ISubmissionRepository& submissions, IClock& clock,
                                     std::string storageFolder)
    : assignments(assignments),
      courses(courses),
      submissions(submissions),
      clock(clock),
      storageFolder(std::move(storageFolder)) {}

std::string SubmissionService::storeFile(const std::string& sourcePath, int assignmentId,
                                         int studentId, int attemptNo) {
    namespace fs = std::filesystem;
    // e.g. submissions/a3_s12_attempt2.pdf - one name per attempt, so nothing is overwritten.
    std::string fileName = "a" + std::to_string(assignmentId) + "_s" + std::to_string(studentId) +
                           "_attempt" + std::to_string(attemptNo) +
                           fs::path(sourcePath).extension().string();
    fs::path target = fs::path(storageFolder) / fileName;
    std::error_code error;
    fs::create_directories(storageFolder, error);
    fs::copy_file(sourcePath, target, fs::copy_options::overwrite_existing, error);
    if (error) throw ValidationError("Could not save the file: " + error.message());
    return target.generic_string();
}

int SubmissionService::daysLate(Timestamp deadline, Timestamp at) {
    if (at <= deadline) return 0;
    long long secondsLate = std::chrono::duration_cast<std::chrono::seconds>(at - deadline).count();
    const long long secondsPerDay = 24LL * 60 * 60;
    return static_cast<int>((secondsLate + secondsPerDay - 1) / secondsPerDay);  // ceiling
}

// The latest of: the assignment deadline, a class extension (CR1, v1.1) and the
// student's own extension (FR06).
Timestamp SubmissionService::effectiveDeadline(const Assignment& a, int studentId) {
    std::optional<ClassExtension> classExt = assignments.findClassExtension(a.getId());
    std::optional<Extension> ext = assignments.findExtension(a.getId(), studentId);
    return a.effectiveDeadlineFor(classExt ? &(*classExt) : nullptr, ext ? &(*ext) : nullptr);
}

Submission SubmissionService::submit(const Session& s, int assignmentId,
                                     const std::string& filePath) {
    s.require(Action::SUBMIT_ASSIGNMENT);
    int studentId = s.user().getId();

    std::optional<Assignment> a = assignments.findById(assignmentId);
    if (!a) throw NotFoundError("No assignment with that id.");
    if (!courses.isEnrolled(studentId, a->getCourseId())) {
        throw AccessDeniedError("You are not enrolled on this course.");
    }
    requireNotBlank(filePath, "File path");
    std::error_code error;  // the error_code versions never throw, even for strange paths
    if (!std::filesystem::is_regular_file(filePath, error)) {
        throw ValidationError("File not found: " + filePath);
    }

    Timestamp t = clock.now();
    Timestamp deadline = effectiveDeadline(*a, studentId);
    std::optional<Submission> existing = submissions.find(assignmentId, studentId);

    // The four branches below are the four [alt] cases of the submit sequence diagram.
    if (!existing) {
        std::string stored = storeFile(filePath, assignmentId, studentId, 1);
        if (t <= deadline) {  // [no existing, on time]
            return insertAndReload(
                Submission(assignmentId, studentId, stored, t, SubmissionStatus::SUBMITTED, 0));
        }
        // [no existing, late] - late work is always accepted (C2)
        return insertAndReload(Submission(assignmentId, studentId, stored, t,
                                          SubmissionStatus::LATE, daysLate(deadline, t)));
    }
    if (existing->getStatus() == SubmissionStatus::SUBMITTED && t <= deadline) {  // [resubmit]
        std::string stored =
            storeFile(filePath, assignmentId, studentId, existing->getAttemptNo() + 1);
        existing->resubmit(stored, t);  // resubmission (FR17)
        submissions.update(*existing);
        return *existing;
    }
    // [else] Late work, or work that is already marked, cannot be resubmitted (O2).
    throw ValidationError("Resubmission is not allowed.");
}

Submission SubmissionService::insertAndReload(const Submission& sub) {
    int id = submissions.insert(sub);
    std::optional<Submission> saved = submissions.findById(id);
    if (!saved) throw DatabaseError("The submission could not be read back after saving.");
    return *saved;
}

std::vector<StudentResult> SubmissionService::resultsFor(const Session& s) {
    s.require(Action::VIEW_RESULTS);
    return submissions.resultsForStudent(s.user().getId());
}

// AssignmentService.h - assignments and extensions (FR05, FR06, FR14, FR15, FR23).
#pragma once

#include <string>
#include <vector>

#include "IClock.h"
#include "Repositories.h"
#include "Session.h"

class AssignmentService {
public:
    AssignmentService(IAssignmentRepository& assignments, ICourseRepository& courses,
                      IClock& clock);

    int createAssignment(const Session& s, int courseId, const std::string& title,
                         const std::string& desc, Timestamp deadline, double maxMark);
    void grantExtension(const Session& s, int assignmentId, int studentId, Timestamp newDeadline,
                        const std::string& reason);
    std::vector<Assignment> listForStudent(const Session& s);
    // byDeadline = true: sorted by deadline; false: in the order they were created.
    std::vector<Assignment> listForLecturer(const Session& s, bool byDeadline);
    std::vector<Course> coursesForStudent(const Session& s);

    // Added during coding: the lecturer's own courses, and the logged-in
    // student's extension for an assignment (shown on their assignment list).
    std::vector<Course> coursesForLecturer(const Session& s);
    std::optional<Extension> myExtension(const Session& s, int assignmentId);

private:
    IAssignmentRepository& assignments;
    ICourseRepository& courses;
    IClock& clock;
};

// AssignmentService.h - assignments and extensions (FR05, FR06, FR14, FR15, FR23).
#pragma once

#include <string>
#include <vector>

#include "IClock.h"
#include "Repositories.h"
#include "Session.h"

// v1.1 (CR1): the deadline a student sees on their assignment list.
struct DeadlineInfo {
    Timestamp original;   // the deadline the lecturer set
    Timestamp effective;  // the deadline that applies to this student
    std::string extendedBy;  // "" = no extension, "class" or "individual"
};

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

    // Added during coding: the lecturer's own courses.
    std::vector<Course> coursesForLecturer(const Session& s);

    // v1.1 (CR1): extend the deadline for every student on the course (FR24).
    void grantClassExtension(const Session& s, int assignmentId, Timestamp newDeadline,
                             const std::string& reason);
    // v1.1 (CR1): the logged-in student's deadline for an assignment (FR15).
    DeadlineInfo deadlineFor(const Session& s, int assignmentId);

private:
    IAssignmentRepository& assignments;
    ICourseRepository& courses;
    IClock& clock;
};

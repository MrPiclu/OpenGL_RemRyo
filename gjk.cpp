#include "gjk.h"
#include <cfloat>

// Support function for a single shape
glm::vec3 GJK::Support(const std::vector<glm::vec3>& vertices, const glm::vec3& direction) {
    float maxDot = -FLT_MAX;
    glm::vec3 bestVertex;

    for (const auto& vertex : vertices) {
        float dotProduct = glm::dot(vertex, direction);
        if (dotProduct > maxDot) {
            maxDot = dotProduct;
            bestVertex = vertex;
        }
    }

    return bestVertex;
}

// Support function for the Minkowski Difference of two shapes
glm::vec3 GJK::SupportMinkowski(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB, const glm::vec3& direction) {
    glm::vec3 supportA = Support(verticesA, direction);
    glm::vec3 supportB = Support(verticesB, -direction);
    return supportA - supportB;
}

// Main GJK collision detection function
bool GJK::Gjk(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB) {
    glm::vec3 direction(1.0f, 0.0f, 0.0f); // Initial arbitrary direction
    std::vector<glm::vec3> simplex;

    // Initial support point
    glm::vec3 point = SupportMinkowski(verticesA, verticesB, direction);
    simplex.push_back(point);

    direction = -point; // New direction towards the origin

    while (true) {
        glm::vec3 newPoint = SupportMinkowski(verticesA, verticesB, direction);

        if (glm::dot(newPoint, direction) <= 0) {
            return false; // No collision
        }

        simplex.push_back(newPoint);

        if (HandleSimplex(simplex, direction)) {
            return true; // Collision detected
        }
    }
}

// Handle the simplex based on its size
bool GJK::HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    if (simplex.size() == 2) {
        return LineCase(simplex, direction);
    }
    else if (simplex.size() == 3) {
        return TriangleCase(simplex, direction);
    }
    else if (simplex.size() == 4) {
        return TetrahedronCase(simplex, direction);
    }
    return false;
}

// Handle the line case
bool GJK::LineCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    glm::vec3 A = simplex[1];
    glm::vec3 B = simplex[0];
    glm::vec3 AB = B - A;
    glm::vec3 AO = -A;

    if (glm::dot(AB, AO) > 0) {
        direction = glm::cross(glm::cross(AB, AO), AB);
    }
    else {
        simplex = { A };
        direction = AO;
    }

    return false;
}

// Handle the triangle case
bool GJK::TriangleCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    glm::vec3 A = simplex[2];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[0];
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AO = -A;

    glm::vec3 ABC = glm::cross(AB, AC);

    // Check if origin is outside AB
    glm::vec3 ABPerp = glm::cross(ABC, AB);
    if (glm::dot(ABPerp, AO) > 0) {
        simplex = { A, B };
        return LineCase(simplex, direction);
    }

    // Check if origin is outside AC
    glm::vec3 ACPerp = glm::cross(AC, ABC);
    if (glm::dot(ACPerp, AO) > 0) {
        simplex = { A, C };
        return LineCase(simplex, direction);
    }

    // Origin is inside triangle
    if (glm::dot(ABC, AO) > 0) {
        direction = ABC;
    }
    else {
        direction = -ABC;
        std::swap(simplex[0], simplex[1]);
    }

    return false;
}

// Handle the tetrahedron case
bool GJK::TetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    glm::vec3 A = simplex[3];
    glm::vec3 B = simplex[2];
    glm::vec3 C = simplex[1];
    glm::vec3 D = simplex[0];
    glm::vec3 AO = -A;

    // Faces
    glm::vec3 ABC = glm::cross(B - A, C - A);
    glm::vec3 ACD = glm::cross(C - A, D - A);
    glm::vec3 ADB = glm::cross(D - A, B - A);

    bool aboveABC = glm::dot(ABC, AO) > 0;
    bool aboveACD = glm::dot(ACD, AO) > 0;
    bool aboveADB = glm::dot(ADB, AO) > 0;

    if (aboveABC) {
        simplex = { A, B, C };
        direction = ABC;
        return TriangleCase(simplex, direction);
    }

    if (aboveACD) {
        simplex = { A, C, D };
        direction = ACD;
        return TriangleCase(simplex, direction);
    }

    if (aboveADB) {
        simplex = { A, D, B };
        direction = ADB;
        return TriangleCase(simplex, direction);
    }

    return true; // Collision detected
}

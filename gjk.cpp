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
    //printf("Direction: (%f, %f, %f), BestVertex: (%f, %f, %f)\n",
        //direction.x, direction.y, direction.z, bestVertex.x, bestVertex.y, bestVertex.z);


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
    int iterationCount = 0;
    const int maxIterations = 100;

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

        if (++iterationCount > maxIterations) {
            printf("GJK: Maximum iterations reached. Possible infinite loop.\n");
            return false;
        }
    }
}

// Handle the simplex based on its size
bool GJK::HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    printf("Simplex 크기: %zu\n", simplex.size());
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

    printf("---0 3\n");
    if (glm::dot(AB, AO) > 0) {
        printf("---0 4\n");
        direction = glm::cross(glm::cross(AB, AO), AB);
    }
    else {
        printf("---0 5\n");
        simplex = { A };
        direction = AO;
    }
    // 방향 크기 확인 및 디버깅 출력
    if (glm::length(direction) < 1e-6f) {
        printf("LineCase: Direction magnitude too small. Resetting.\n");
        direction = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    printf("---0 7\n");


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

    printf("TriangleCase - Simplex Points: A(%f, %f, %f), B(%f, %f, %f), C(%f, %f, %f)\n",
        A.x, A.y, A.z, B.x, B.y, B.z, C.x, C.y, C.z);

    // Check if origin is outside AB
    glm::vec3 ABPerp = glm::cross(AB, ABC);
    printf("---0\n");
    if (glm::dot(ABPerp, AO) > 0) {
        printf("---0 1\n");
        simplex = { A, B };
        printf("---0 2\n");
        return LineCase(simplex, direction);
    }

    printf("---1\n");

    // Check if origin is outside AC
    glm::vec3 ACPerp = glm::cross(ABC, AC);
    if (glm::dot(ACPerp, AO) > 0) {
        simplex = { A, C };
        return LineCase(simplex, direction);
    }

    printf("---2\n");

    // Origin is inside triangle
    if (glm::dot(ABC, AO) > 0) {
        direction = ABC;
    }
    else {
        direction = -ABC;
        std::swap(simplex[0], simplex[1]);
    }

    printf("TriangleCase - New Direction: (%f, %f, %f)\n", direction.x, direction.y, direction.z);

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

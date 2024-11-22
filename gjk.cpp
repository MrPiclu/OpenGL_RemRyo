#include "gjk.h"

// 예시: 지원 함수
glm::vec3 Support(const std::vector<glm::vec3>& vertices, const glm::vec3& direction) {
    float maxDot = -FLT_MAX;
    glm::vec3 supportPoint;
    for (const auto& vertex : vertices) {
        float dot = glm::dot(vertex, direction);
        if (dot > maxDot) {
            maxDot = dot;
            supportPoint = vertex;
        }
    }
    return supportPoint;
}

glm::vec3 SupportMinkowski(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB, const glm::vec3& direction) {
    glm::vec3 supportA = Support(verticesA, direction);
    glm::vec3 supportB = Support(verticesB, -direction);
    return supportA - supportB;
}

bool GJK(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB) {
    // 초기 방향 벡터 선택
    glm::vec3 direction = glm::vec3(1, 0, 0);

    // 첫 번째 지원점 계산
    glm::vec3 support = SupportMinkowski(verticesA, verticesB, direction);
    std::vector<glm::vec3> simplex;
    simplex.push_back(support);

    // 방향을 원점에서 심플렉스로 향하도록 반전
    direction = -support;

    while (true) {
        support = SupportMinkowski(verticesA, verticesB, direction);
        if (glm::dot(support, direction) <= 0) {
            // 원점이 Minkowski 차집합 외부에 있음
            return false; // 충돌 없음
        }
        simplex.push_back(support);
        if (HandleSimplex(simplex, direction)) {
            return true; // 충돌 발생
        }
    }
}

bool HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    // 심플렉스의 형태에 따라 처리
    if (simplex.size() == 2) {
        // 선분 처리
        return LineCase(simplex, direction);
    }
    else if (simplex.size() == 3) {
        // 삼각형 처리
        return TriangleCase(simplex, direction);
    }
    else if (simplex.size() == 4) {
        // 사면체 처리
        return TetrahedronCase(simplex, direction);
    }
    return false;
}


bool LineCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    
}

bool TriangleCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {

}

bool TetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) {

}
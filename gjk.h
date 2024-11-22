#ifndef GJK_CLASS_H
#define GJK_CLASS_H

#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "VAO.h"

class GJK
{
public:
	std::vector<glm::vec3> vertice;

	struct simplex
	{
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;
		glm::vec3 d;
	};

	int width;
	int height;

	float origin_speed = 0.03f;
	float speed = 0.03f;
	float sensitivity = 100.0f;
	float Ratio = 1.0f;

	glm::vec3 Support(const std::vector<glm::vec3>& vertices, const glm::vec3& direction);
	glm::vec3 SupportMinkowski(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB, const glm::vec3& direction);
	bool Gjk(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB);
	bool HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction);

	bool LineCase(std::vector<glm::vec3>& simplex, glm::vec3& direction);
	bool TriangleCase(std::vector<glm::vec3>& simplex, glm::vec3& direction);
	bool TetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction);
};

#endif
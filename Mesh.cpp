#include "Mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)

{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	// Bind와 Unbind란? 특정 상태를 활성화 하거나 비활성화 하는 것 (VAO, VBO, EBO같은 객체들은 OpenGL의 상태머신에서 특정 상태로 설정되고 그 상태가 유지되는 동안 OpenGL은 해당 객체들을 참조하여 명령 수행함
	VAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	VAO.UnBind();
	VBO.UnBind();
	EBO.UnBind();
}

void Mesh::Draw(Shader& shader, Camera& camera)
{
	shader.Activate(); //해당 쉐이더를 활성화
	VAO.Bind(); // 현재 메쉬의 Vertex Array Object를 바인딩함 
	// 왜 VAO만 바인딩하는가? 
	//		A. Mesh 객체 생성 때 LinkAttrib로 VAO가 VBO, EBO를 기억하고 있어서 자동으로 사용함
	//		근데 EBO는 LinkAttrib 하지 않았는데 왜 되는가?
	//			A. EBO는 바인딩 할 때 자동으로 VAO에 바인딩 된다

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		textures[i].texUnit(shader, (type + num).c_str(), i); //각 텍스쳐에 대해 texUnit 매서드 호출해 쉐이더에서 사용할 유니폼 이름 설정, type은 diffuse나 specular가 될 것이고 num은 말 그대로 인덱스로 diffuse0이런 식으로 i 번째 텍스쳐에 유니폼으로 저장됨
		textures[i].Bind(); // 텍스쳐를 활성화해서 드로우 콜에서 이 텍스쳐를 사용할 수 있도록 만듦
	}

	// 카메라의 위치 및 뷰&투영 행렬을 쉐이더에 전달
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z); 
	camera.Matrix(shader, "camMatrix");
	// 왜 camPos에 값을 전달한 후 camMatrix에 전달해도 상관없나? fragment shader 계산 후에 vertex shader 계산이 이루어지는거 아닌가?
	//			A. 아니다. 해당 괒어은 실제로 저 순서대로 계산되지만 다음 드로우콜 호출(glDraw*)을 위해 셰이더에 데이터를 설정하는 준비 단계이다. (실제로 실행은 드로우 호출 시 이루어짐)

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); //다음 드로우 콜 호출
}
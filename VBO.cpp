#include "VBO.h"

VBO::VBO(std::vector<Vertex>& vertices)
{

	glGenBuffers(1, &ID); //정점 버퍼 생성 (실제 정점 데이터 저장)
	glBindBuffer(GL_ARRAY_BUFFER, ID); //바인딩 실행 (해당 유행의 객체를 수정하면 바인딩된 객체가 함께 수정됨)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW); //static, dynamic, stream이 있음 (DRAW, COPY)
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID); //바인딩 실행 (해당 유행의 객체를 수정하면 바인딩된 객체가 함께 수정됨)
}

void VBO::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); //바인딩 실행 (해당 유행의 객체를 수정하면 바인딩된 객체가 함께 수정됨)
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID); //VBO 삭제
}
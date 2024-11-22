#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &ID); //정점 배열 생성 (정점 속성 구성 저장)
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout , numComponents, type, GL_FALSE, stride, offset); // (속성위치, 정점 구성 요소 수, 데이터 타입, 정규화 여부
	glEnableVertexAttribArray(layout); //layout 위치의 속성을 활성화함
	VBO.UnBind();
}

void VAO::Bind()
{
	glBindVertexArray(ID); //VAO를 바인딩 하여 이후 설정될 정점 속성 구성을 VAO에 저장함
}

void VAO::UnBind()
{
	glBindVertexArray(0); //VAO를 바인딩 하여 이후 설정될 정점 속성 구성을 VAO에 저장함
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID); //VAO 삭제
}
#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh
{
	public:
		//array를 사용하지 않는 이유 : 초기화할 때 배열의 크기를 알 수 없기 때문임
		std::vector <Vertex> vertices;
		std::vector <GLuint> indices; 
		std::vector <Texture> textures; 

		const std::vector <Vertex>& getVertices() const {
			return vertices;
		}

		VAO VAO;

		Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

		void Draw(Shader& shader, Camera& camera) ;

};

#endif
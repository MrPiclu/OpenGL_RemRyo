//Execute vertex shader 
// 역할 : 정점위치 계산 (모델행렬, 뷰행렬, 투영행렬을 사용해서 로컬 좌표를 클립 공간 좌표로 변환함)
//				변수 전달 준비 (out 키워드로 다음 단계 (fragment shader)에 전달)

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform float scale; //사용하지 않을꺼면 아예 만들지 마셈 (사용안하면 알아서 삭제해서 오류날 수 있음)
uniform mat4 camMatrix;
uniform mat4 model;

void main()
{

	crntPos = vec3(model * vec4(aPos, 1.0f)); //월드 좌표계에서 현재 위치 계산
	Normal = aNormal;
	color = aColor;
	texCoord = aTex;

	gl_Position = camMatrix * vec4(crntPos, 1.0); //최종 정점 위치 (클립 공간) 계산 (vertex shading 최종 과정)
}
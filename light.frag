#version 330 core

out vec4 FragCoor;

uniform vec4 lightColor;

void main()
{
	FragCoor = lightColor;
}
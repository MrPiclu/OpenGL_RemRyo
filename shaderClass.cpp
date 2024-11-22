#include "shaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //unsigned integer (왜 uint를 반환하는가? OpenGL은 객체지향언어가 아니라 포인터 대신 정수형 ID를 사용함. return GLuint값은 현재 OpenGL 컨텍스트 내에서 고유한 셰이더 ID인 것임)
	glShaderSource(vertexShader, 1, &vertexSource, NULL); //셰이더 소스 코드를 정점 셰이더에 연결
	glCompileShader(vertexShader); //컴파일
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Fragment Shader는 각 픽셀의 색상을 결정하는 역할을 함
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(vertexShader, "FRAGMENT");

	ID = glCreateProgram(); //셰이더 프로그램 선언 (vertex, fragment 래핑을 위함) (각 vertex, fragment shader는 하나씩만 링크 가능함 (컴파일은 다중 가능))
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader); //두 셰이더 래핑
	glLinkProgram(ID); // 셰이더들을 링크하여 하나의 프로그램으로 만듦
	compileErrors(vertexShader, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader); //래핑 된 개별 객체들은 따로 삭제

}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled); //hasCompiled 변수에 shader compile 결과를 넣음
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << std::endl;
		}

	}
}
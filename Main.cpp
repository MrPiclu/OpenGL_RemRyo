#include<filesystem>
namespace fs = std::filesystem;

#include "Mesh.h"
#include "Model.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const  int width = 800;
const unsigned int height = 800;

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
	// 창 크기에 맞춰 전체 화면을 사용할 수 있도록 뷰포트 설정
	glViewport(0, 0, width, height);
	glfwGetFramebufferSize(window, &width, &height);
	
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	float aspectRatio = static_cast<float>(width) / height;
	camera->updateMatrix(45.0f, 0.1f, 100.0f, aspectRatio);
}

int main() {

	glfwInit(); //GLFW 라이브러리 초기화

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl 버전을 glfw에 제공함
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //opengl 프로필 설정 (CORE profile은 최신 opengl기능만 포함하고 구식 기능을 제거한 프로필임)

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL); //width, height, title, full screen, 공유 컨텍스트
	if (window == NULL) { //창 생성에 문제 생기면 바로 종료
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); //현재 OpenGL 컨텍스트를 window 창으로 설정
	gladLoadGL(); // Glad를 초기화하여 opengl에 필요한 configuration 불러옴 (함수 포인터같은거)

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
	glfwSetWindowUserPointer(window, &camera); // 카메라를 윈도우에 연결하여 콜백 함수에서 접근 가능하게 함

	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	OnFramebufferSizeChange(window, framebufferWidth, framebufferHeight);

	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string texPath = "/YoutubeOpenGL/";

	Texture face[]
	{
		//												IMAGE										TYPE		SLOT	FORMAT				PIXELTYPE	
		Texture((parentDir + texPath + "Models/Skin Base Color1.png").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture((parentDir + texPath + "Models/specular.jpg").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	Texture hair[]
	{
		Texture((parentDir + texPath + "Models/Hair_Blue1.png").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
	};

	Shader shaderProgram("default.vert", "default.frag");
	std::vector <Texture> mt_face(face, face + sizeof(face) / sizeof(Texture)); //std::vector <Texture> 는 Texture 타입의 객체를 담는 벡터
	std::vector <Texture> mt_hair(hair, hair + sizeof(hair) / sizeof(Texture)); //std::vector <Texture> 는 Texture 타입의 객체를 담는 벡터

	Shader lightShader("light.vert", "light.frag");
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex)); // 배열의 시작 포인터 lightVertices와 배열의 끝 포인터 (lightvertices + 배열 크기)로 std::vector 초기화하여 배열의 모든 요소를 벡터에 복사함
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	Mesh light(lightVerts, lightInd, mt_face);

	std::unique_ptr<Model> m_face = Model::Load((parentDir + texPath + "Models/Rem1.obj").c_str(), mt_face);
	if (!m_face) return false;
	std::unique_ptr<Model> m_hair = Model::Load((parentDir + texPath + "Models/Rem_Hair1.obj").c_str(), mt_hair);
	if (!m_hair) return false;

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f); // 조명 방향
	glm::mat4 lightModel = glm::mat4(1.0f); //mat4는 단위 행렬(identity matrix)를 생성하는 코드
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 modelTrans = glm::mat4(1.0f);
	modelTrans = glm::translate(modelTrans, modelPos);

	lightShader.Activate(); // shader 활성화해서 glUniform 호출들이 lightshader에 적용되도록 함
	// glUniformMatrix4fv(넘길 유니폼 변수, 행렬의 개수, 행렬 전치 필요 여부, 넘길 포인터 값)
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel)); //lightshader의 light.vert 의 유니폼 변수 model에 lightmodel 행렬 값을 넘김 (Matrix4fv를 넘김)
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w); //lightshader의 light.frag 의 유니폼 변수 lightColor에 lightColor vec4 값을 넘김

	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST); //3D 모델의 뎁스 설정

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

	while (!glfwWindowShouldClose(window)) //메인 렌더링 루프문
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //배경색을 다시 설정
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 버퍼를 초기화하여 배경색을 적용 및 뎁스 버퍼 초기화

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		camera.Inputs(window, io);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		light.Draw(lightShader, camera);
		m_face->Draw(shaderProgram, camera);
		m_hair->Draw(shaderProgram, camera);

		ImGui::Begin("My game is window, ImGui window");
		ImGui::Text("Hello");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window); // 버퍼를 교체하여 렌더링된 이미지를 표시
		glfwPollEvents(); //모든 pulled event를 처리하도록 glfw에 지시함 (이거 없으면 응답없음 상태로 진입할 수 있음)
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	shaderProgram.Delete();
	lightShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate(); //창 종료
	return 0;
}
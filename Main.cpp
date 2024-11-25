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


struct Object {
	glm::mat4 transform;   // 트랜스폼 행렬
	glm::vec3 position;    // 위치
	glm::vec3 scale;       // 크기
	Model* model;          // 모델
	Shader* shader;        // 각 오브젝트마다 다른 셰이더를 사용할 수 있도록 추가
	float transparency;    // 투명도
};

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
	// 창 크기에 맞춰 전체 화면을 사용할 수 있도록 뷰포트 설정
	glViewport(0, 0, width, height);
	glfwGetFramebufferSize(window, &width, &height);
	
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	float aspectRatio = static_cast<float>(width) / height;
	camera->updateMatrix(45.0f, 0.1f, 100.0f, aspectRatio);
}

void DrawJoystick(const char* label, glm::vec2& joystickPos, float radius, float maxValue) {
	// Joystick을 그릴 영역의 시작점
	ImVec2 p = ImGui::GetCursorScreenPos();
	float diameter = radius * 2.0f;

	// 현재 조이스틱 중심 좌표
	ImVec2 center = ImVec2(p.x + radius, p.y + radius);

	// 마우스 입력 상태
	ImGui::InvisibleButton(label, ImVec2(diameter, diameter));
	bool isActive = ImGui::IsItemActive();      // 드래그 중인지 체크
	bool isHovered = ImGui::IsItemHovered();    // 호버 상태인지 체크

	// 마우스 드래그 처리
	if (isActive) {
		ImVec2 ioMouse = ImGui::GetIO().MousePos;
		ImVec2 delta = ImVec2(ioMouse.x - center.x, ioMouse.y - center.y); // 마우스와 중심의 차이
		float length = sqrtf(delta.x * delta.x + delta.y * delta.y);

		// 조이스틱이 영역 밖으로 벗어나지 않도록 길이를 제한
		if (length > radius) {
			delta.x = (delta.x / length) * radius;
			delta.y = (delta.y / length) * radius;
		}

		// 조이스틱 위치 업데이트 ([-1, 1]의 정규화 값으로 변환)
		joystickPos.x = (delta.x / radius) * maxValue;
		joystickPos.y = -(delta.y / radius) * maxValue;

	}else {
		// 드래그를 멈췄으면 조이스틱 위치를 (0, 0)으로 리셋
		joystickPos = glm::vec2(0.0f, 0.0f);
	}

	// 조이스틱 바탕 원
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(center, radius, IM_COL32(100, 100, 100, 255)); // 배경 원
	drawList->AddCircle(center, radius, IM_COL32(200, 200, 200, 255), 32);  // 테두리

	// 조이스틱 핸들 원
	ImVec2 handlePos = ImVec2(
		center.x + joystickPos.x * radius,
		center.y - joystickPos.y * radius
	);
	drawList->AddCircleFilled(handlePos, radius * 0.3f, IM_COL32(255, 0, 0, 255)); // 조이스틱 핸들
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
	std::string texPath = "/OpenGL_RemRyo/";

	unsigned char grayTextureData[3] = { 128, 128, 128 }; // RGB 값이 모두 128인 회색

	Texture defaultTexture[]
	{
		Texture((parentDir + texPath + "Models/grey.png").c_str(), "diffuse", 0, GL_RED, GL_UNSIGNED_BYTE),
	};


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
	std::vector <Texture> mt_default(defaultTexture, defaultTexture + sizeof(defaultTexture) / sizeof(Texture));

	Shader lightShader("light.vert", "light.frag");
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex)); // 배열의 시작 포인터 lightVertices와 배열의 끝 포인터 (lightvertices + 배열 크기)로 std::vector 초기화하여 배열의 모든 요소를 벡터에 복사함
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	Mesh light(lightVerts, lightInd, mt_face);

	Shader convexShader("transparent.vert", "transparent.frag");

	std::unique_ptr<Model> m_face = Model::Load((parentDir + texPath + "Models/RemOriginFixed.obj").c_str(), mt_face);
	if (!m_face) return false;
	std::unique_ptr<Model> m_eyeL = Model::Load((parentDir + texPath + "Models/RemEyeLeftOriginFixed.obj").c_str(), mt_face);
	if (!m_eyeL) return false;
	std::unique_ptr<Model> m_eyeR = Model::Load((parentDir + texPath + "Models/RemEyeRightOriginFixed.obj").c_str(), mt_face);
	if (!m_eyeR) return false;
	std::unique_ptr<Model> m_hair = Model::Load((parentDir + texPath + "Models/RemHairOriginFixed.obj").c_str(), mt_hair);
	if (!m_hair) return false;
	std::unique_ptr<Model> m_convex = Model::Load((parentDir + texPath + "Models/Convex.obj").c_str(), mt_default);
	if (!m_convex) return false;

	std::vector<Object> objects; //오브젝트 관리

	Object obj1;
	obj1.transform = glm::mat4(1.0f);
	obj1.position = glm::vec3(0.0f, 0.0f, 0.0f);
	obj1.scale = glm::vec3(1.0f);
	obj1.transform = glm::translate(obj1.transform, obj1.position);
	obj1.transform = glm::scale(obj1.transform, obj1.scale);
	obj1.model = m_face.get();
	obj1.shader = &shaderProgram; // 기본 셰이더
	obj1.transparency = 1.0f; // 불투명

	Object obj5;
	obj5.transform = glm::mat4(1.0f);
	obj5.position = glm::vec3(0.0f, 0.0f, 0.0f);
	obj5.scale = glm::vec3(1.0f);
	obj5.transform = glm::translate(obj5.transform, obj5.position);
	obj5.transform = glm::scale(obj5.transform, obj5.scale);
	obj5.model = m_eyeL.get();
	obj5.shader = &shaderProgram; // 기본 셰이더
	obj5.transparency = 1.0f; // 불투명

	Object obj6;
	obj6.transform = glm::mat4(1.0f);
	obj6.position = glm::vec3(0.0f, 0.0f, 0.0f);
	obj6.scale = glm::vec3(1.0f);
	obj6.transform = glm::translate(obj6.transform, obj6.position);
	obj6.transform = glm::scale(obj6.transform, obj6.scale);
	obj6.model = m_eyeR.get();
	obj6.shader = &shaderProgram; // 기본 셰이더
	obj6.transparency = 1.0f; // 불투명

	Object obj2;
	obj2.transform = glm::mat4(1.0f); // 초기화
	obj2.position = obj1.position;
	obj2.scale = obj1.scale;
	obj2.transform = glm::translate(obj2.transform, obj2.position);
	obj2.transform = glm::scale(obj2.transform, obj2.scale);
	obj2.model = m_hair.get();
	obj2.shader = &shaderProgram; // 기본 셰이더
	obj2.transparency = 1.0f; // 불투명
	
	Object obj3;
	obj3.transform = glm::mat4(1.0f); // 초기화
	obj3.position = glm::vec3(0.0f, 0.0f, 0.0f);
	obj3.scale = glm::vec3(1.0f);
	obj3.transform = glm::translate(obj3.transform, obj3.position);
	obj3.transform = glm::scale(obj3.transform, obj3.scale);
	obj3.model = m_convex.get();
	obj3.shader = &shaderProgram; // 기본 셰이더
	obj3.transparency = 0.5f; // 불투명
	
	Object obj4;
	obj4.transform = glm::mat4(1.0f); // 초기화
	obj4.position = glm::vec3(3.0f, 0.0f, 5.0f);
	obj4.scale = glm::vec3(1.0f);
	obj4.transform = glm::translate(obj4.transform, obj4.position);
	obj4.transform = glm::scale(obj4.transform, obj4.scale);
	obj4.model = m_convex.get();
	obj4.shader = &shaderProgram; // 기본 셰이더
	obj4.transparency = 0.5f; // 불투명

	objects.push_back(obj1);
	objects.push_back(obj2);
	objects.push_back(obj3);
	objects.push_back(obj4);

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f); // 조명 방향
	glm::mat4 lightModel = glm::mat4(1.0f); //mat4는 단위 행렬(identity matrix)를 생성하는 코드
	lightModel = glm::translate(lightModel, lightPos);

	/*glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 modelTrans = glm::mat4(1.0f);
	modelTrans = glm::translate(modelTrans, modelPos);*/

	lightShader.Activate(); // shader 활성화해서 glUniform 호출들이 lightshader에 적용되도록 함
	// glUniformMatrix4fv(넘길 유니폼 변수, 행렬의 개수, 행렬 전치 필요 여부, 넘길 포인터 값)
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel)); //lightshader의 light.vert 의 유니폼 변수 model에 lightmodel 행렬 값을 넘김 (Matrix4fv를 넘김)
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w); //lightshader의 light.frag 의 유니폼 변수 lightColor에 lightColor vec4 값을 넘김

	shaderProgram.Activate();
	// 광원의 정보를 물체에 전달
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	convexShader.Activate();
	glUniform4f(glGetUniformLocation(convexShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(convexShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	glEnable(GL_DEPTH_TEST); //3D 모델의 뎁스 설정

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

	glm::vec2 joystickPosition(0.0f, 0.0f); // 조이스틱의 초기 위치 (중앙)
	while (!glfwWindowShouldClose(window)) //메인 렌더링 루프문
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //배경색을 다시 설정
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 버퍼를 초기화하여 배경색을 적용 및 뎁스 버퍼 초기화

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		camera.Inputs(window, io, obj1.transform);
		obj2.transform = obj1.transform;
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// 투명도 슬라이더 UI 추가
		ImGui::Begin("Transparency Control for Rem's Convex Collision");
		ImGui::SetNextWindowPos(ImVec2(100.0f, 100.0f), ImGuiCond_FirstUseEver); // 초기 위치 지정
		ImGui::SetNextWindowSize(ImVec2(300.0f, 100.0f), ImGuiCond_FirstUseEver); // 초기 크기 지정
		ImGui::SliderFloat("Transparency", &obj3.transparency, 0.0f, 1.0f);
		ImGui::End();

		// 투명도 슬라이더 UI 추가
		ImGui::Begin("Transparency Control for Ryo's Convex Collision");
		ImGui::SetNextWindowPos(ImVec2(100.0f, 100.0f), ImGuiCond_FirstUseEver); // 초기 위치 지정
		ImGui::SetNextWindowSize(ImVec2(300.0f, 100.0f), ImGuiCond_FirstUseEver); // 초기 크기 지정
		ImGui::SliderFloat("Transparency", &obj4.transparency, 0.0f, 1.0f);
		ImGui::End();

		//glm::vec3 position = glm::vec3(obj3.transform[3]); // 현재 위치
		//ImGui::Begin("Object Transform");
		//ImGui::SliderFloat3(("Object " + std::to_string(i)).c_str(), glm::value_ptr(position), -10.0f, 10.0f);
		//ImGui::End();

		//// 변경된 위치를 트랜스폼 행렬에 반영
		//obj3.transform = glm::translate(glm::mat4(1.0f), position);
		 // 현재 X, Y 값을 분리하여 저장 (Z 값은 고정)

		//glm::vec3 position = glm::vec3(obj3.transform[3]); // 현재 위치
		//glm::vec2 positionVec2 = glm::vec2(position.x, position.z);

		//// ImGui 슬라이더로 X, Y 값 조정
		//ImGui::Begin("Object Transform");
		//ImGui::SliderFloat2("Position (X, Y)", glm::value_ptr(positionVec2), -10.0f, 10.0f); // X, Y 값 슬라이더
		//ImGui::End();

		//// 변경된 X, Y 값을 position에 반영 (Z 값은 고정)
		//position.x = positionVec2.x;
		//position.z = positionVec2.y;

		//// 변경된 위치를 트랜스폼 행렬에 반영
		//obj3.transform = glm::translate(glm::mat4(1.0f), position);


		// 조이스틱 UI
		ImGui::Begin("Joystick Control");
		DrawJoystick("Joystick", joystickPosition, 50.0f, 0.1f);
		ImGui::Text("Joystick X: %.2f, Y: %.2f", joystickPosition.x, joystickPosition.y);
		ImGui::End();


		// 카메라나 오브젝트 이동
		obj4.position.x += joystickPosition.x * 0.1f; // 예: x 방향 이동
		obj4.position.z += joystickPosition.y * -0.1f; // 예: y 방향 이동
		obj4.transform = glm::translate(glm::mat4(1.0f), obj4.position);

		light.Draw(lightShader, camera);



		obj1.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj1.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj1.transform));
		// 모델 렌더링
		obj1.model->Draw(*obj1.shader, camera);

		obj2.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj2.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj2.transform));
		// 모델 렌더링
		obj2.model->Draw(*obj2.shader, camera);

		obj5.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj5.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj5.transform));
		// 모델 렌더링
		obj5.model->Draw(*obj5.shader, camera);

		obj6.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj6.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj6.transform));
		// 모델 렌더링
		obj6.model->Draw(*obj6.shader, camera);


		obj3.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj3.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj3.transform));
		// 투명도가 있는 경우 블렌딩 활성화
		if (obj3.transparency < 1.0f) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(glGetUniformLocation(obj3.shader->ID, "transparency"), obj3.transparency);
		}
		// 모델 렌더링
		obj3.model->Draw(*obj3.shader, camera);
		// 블렌딩 비활성화
		if (obj3.transparency < 1.0f) {
			glDisable(GL_BLEND);
		}

		obj4.shader->Activate();
		// 오브젝트의 트랜스폼 매트릭스 전달
		glUniformMatrix4fv(glGetUniformLocation(obj4.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(obj4.transform));
		// 투명도가 있는 경우 블렌딩 활성화
		if (obj4.transparency < 1.0f) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(glGetUniformLocation(obj4.shader->ID, "transparency"), obj4.transparency);
		}
		// 모델 렌더링
		obj4.model->Draw(*obj4.shader, camera);
		// 블렌딩 비활성화
		if (obj4.transparency < 1.0f) {
			glDisable(GL_BLEND);
		}

		//// 오브젝트별 렌더링
		//for (Object& object : objects) {
		//	object.shader->Activate();

		//	// 오브젝트의 트랜스폼 매트릭스 전달
		//	glUniformMatrix4fv(glGetUniformLocation(object.shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(object.transform));

		//	// 투명도가 있는 경우 블렌딩 활성화
		//	if (object.transparency < 1.0f) {
		//		glEnable(GL_BLEND);
		//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//		glUniform1f(glGetUniformLocation(object.shader->ID, "transparency"), object.transparency);
		//	}

		//	// 모델 렌더링
		//	object.model->Draw(*object.shader, camera);

		//	// 블렌딩 비활성화
		//	if (object.transparency < 1.0f) {
		//		glDisable(GL_BLEND);
		//	}
		//}

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
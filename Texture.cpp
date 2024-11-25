#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	// 텍스쳐링
	int widthImg, heightImg, numColCh; //numColCh은 색상 채널 수
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes) {
		std::cout << "Failed to load texture " << image << std::endl;
	}


	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID); //GL_TEXTURE_2D으로 생성한 텍스쳐 ID를 현재 활성화된 텍스처 슬롯에 바인딩

	//텍스처 필터링 방식
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);  //NEAREST : 가장 가까운 픽셀값 사용 LINEAR : 부드러운 필터링
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//텍스처 래핑 (텍스처 좌표가 범위를 넘어갔을 때 어떻게 처리할지)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // REPEAT : 텍스처를 반복해서 적용
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes); //텍스처 데이터를 GPU에 업로드 (GL_TEXTURE_2D, Mipmap level, 텍스처 내부 포멧, 이미지 너비 높이, 항상 0, 이미지 포멧, 픽셀 데이터 타입, 실제 이미지 데이터)
	glGenerateMipmap(GL_TEXTURE_2D); // 텍스처에 대해 mipmap 생성 (mipmap은 텍스처가 멀리있는 경우 자동으로 낮은 해상도의 텍스처를 사용하는 방식)

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

}

Texture::Texture( GLenum slot, GLenum format, GLenum pixelType)
{
	unsigned char grayTextureData[3] = { 128, 128, 128 }; // RGB 값이 모두 128인 회색

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID); //GL_TEXTURE_2D으로 생성한 텍스쳐 ID를 현재 활성화된 텍스처 슬롯에 바인딩

	//텍스처 필터링 방식
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);  //NEAREST : 가장 가까운 픽셀값 사용 LINEAR : 부드러운 필터링
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//텍스처 래핑 (텍스처 좌표가 범위를 넘어갔을 때 어떻게 처리할지)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // REPEAT : 텍스처를 반복해서 적용
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, format, pixelType, grayTextureData); //텍스처 데이터를 GPU에 업로드 (GL_TEXTURE_2D, Mipmap level, 텍스처 내부 포멧, 이미지 너비 높이, 항상 0, 이미지 포멧, 픽셀 데이터 타입, 실제 이미지 데이터)
	glGenerateMipmap(GL_TEXTURE_2D); // 텍스처에 대해 mipmap 생성 (mipmap은 텍스처가 멀리있는 경우 자동으로 낮은 해상도의 텍스처를 사용하는 방식)

	stbi_image_free(grayTextureData);
	glBindTexture(GL_TEXTURE_2D, ID);

}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit); //texUni 위치의 유니폼 변수를 텍스처 유닛 unit으로 설정함 (셰이더에서 이 텍스처 유니폼을 사용할 때 지정된 텍스처 슬롯의 데이터를 참조하게 됨)
}

void  Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);

}
void  Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
void  Texture::Delete()
{
	glDeleteTextures(1, &ID); //GPU 메모리에서 텍스처 삭제 및 자원 해제
}

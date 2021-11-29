#include "ImportManager.h"
#include "stb_image.h"

ImportManager::ImportManager()
{
}

ImportManager::~ImportManager()
{
}

Image ImportManager::LoadTextureFromFile(const char* filename)
{
	// Load from file
	int imageWidth = 0;
	int imageHeight = 0;
	unsigned char* imageData = stbi_load(filename, &imageWidth, &imageHeight, NULL, 4);

	if (imageData == NULL)
	{
		LOG("IMPORTER ERROR \\/");
		return Image(ImageTexture::IMG_NO_IMAGE);
	}

	// Create a OpenGL texture identifier
	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glBindTexture(GL_TEXTURE_2D, imageTexture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	return Image(imageTexture, imageWidth, imageHeight);
}
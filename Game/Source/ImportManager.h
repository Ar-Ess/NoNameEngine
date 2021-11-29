#ifndef __IMPORT_MANAGER_H__
#define __IMPORT_MANAGER_H__

#include "Globals.h"
#include <string>
#include "External/SDL/include/SDL_opengl.h"

enum class ImageTexture
{
	IMG_NO_IMAGE,
	IMG_FOLDER,
	IMG_FILE,
	IMG_CUSTOM
};

class Image
{
public:
	Image(ImageTexture imgTexture)
	{
		switch (imgTexture)
		{
		case ImageTexture::IMG_FOLDER:
			texture = imgTexture;
			break;

		case ImageTexture::IMG_FILE:
			texture = imgTexture;
			break;

		case ImageTexture::IMG_CUSTOM:
			LOG("ERROR: Image not initialized. Use another constructor to create an 'IMG_CUSTOM'");
			break;

		case ImageTexture::IMG_NO_IMAGE:
			LOG("ERROR: Image not initialized. Use another ImageTexture to generate an image");
			break;
		}

		return;
	}

	Image(GLuint imgTextureID, Point size)
	{
		textureID = imgTextureID;
		width = (int)size.x;
		height = (int)size.y;
		texture = ImageTexture::IMG_CUSTOM;
	}

	Image(GLuint imgTextureID, int imgWidth, int imgHeight)
	{
		textureID = imgTextureID;
		width = imgWidth;
		height = imgHeight;
		texture = ImageTexture::IMG_CUSTOM;
	}

public:

	ImageTexture GetTmageTexture() const
	{
		return texture;
	}

	GLuint GetTextureId() const
	{
		return textureID;
	}

	Point GetImageDimensions() const
	{
		return Point{width, height};
	}

private:

	int width = 0;
	int height = 0;
	GLuint textureID = 0;
	ImageTexture texture = ImageTexture::IMG_NO_IMAGE;
};

class ImportManager
{
public:

	ImportManager();

	~ImportManager();

	Image LoadTextureFromFile(const char* filename);

	bool ImageLoader(const char* filename, GLuint* out);
};

#endif // !__IMPORT_MANAGER_H__


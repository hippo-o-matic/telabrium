#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "hipponium/shader.h"
#include "hipponium/object.h"

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class Text {
public:
	Text(std::string txt, glm::vec2 pos = glm::vec2(0,0), float scl = 1.0) {
		content = txt;
		Position = pos;
		scale = scl;
	}
	Text(std::string txt, float x = 0, float y = 0, float scl = 1.0) {
		content = txt;
		Position = glm::vec2(x, y);
		scale = scl;
	}

	void Draw(Shader* shader) {

	}

	std::string content;
	glm::vec2 Position;
	float scale;

private:
	
};

class Text3d : public Object{

};

namespace Text{

	FT_Library _FT;
//	if (FT_Init_FreeType(&_FT))
//		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	void loadFont(std::string const* path, int size) {
		FT_Face face;
		if (FT_New_Face(_FT, "fonts/arial.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, size);
		std::map<GLchar, Character> Characters;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			FT_Done_Face(face);
			FT_Done_FreeType(_FT);
		}
	}
}

#endif

#include "../Externals/Include/Include.h"
#include <vector>
#include <string>


	GLuint state_link;
	unsigned int cubemapTexture;
	GLuint skyvao, skyvbo;
	GLuint sky_tex_location;
	int sky_state;


	float skyboxVertices[] = {
		// positions          
		-300.0f,  300.0f, -300.0f,
		-300.0f, -300.0f, -300.0f,
		300.0f, -300.0f, -300.0f,
		300.0f, -300.0f, -300.0f,
		300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,

		-300.0f, -300.0f,  300.0f,
		-300.0f, -300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f,  300.0f,
		-300.0f, -300.0f,  300.0f,

		300.0f, -300.0f, -300.0f,
		300.0f, -300.0f,  300.0f,
		300.0f,  300.0f,  300.0f,
		300.0f,  300.0f,  300.0f,
		300.0f,  300.0f, -300.0f,
		300.0f, -300.0f, -300.0f,

		-300.0f, -300.0f,  300.0f,
		-300.0f,  300.0f,  300.0f,
		300.0f,  300.0f,  300.0f,
		300.0f,  300.0f,  300.0f,
		300.0f, -300.0f,  300.0f,
		-300.0f, -300.0f,  300.0f,

		-300.0f,  300.0f, -300.0f,
		300.0f,  300.0f, -300.0f,
		300.0f,  300.0f,  300.0f,
		300.0f,  300.0f,  300.0f,
		-300.0f,  300.0f,  300.0f,
		-300.0f,  300.0f, -300.0f,

		-300.0f, -300.0f, -300.0f,
		-300.0f, -300.0f,  300.0f,
		300.0f, -300.0f, -300.0f,
		300.0f, -300.0f, -300.0f,
		-300.0f, -300.0f,  300.0f,
		300.0f, -300.0f,  300.0f
	};




	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				printf("%s\n", faces[i].c_str());
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
	

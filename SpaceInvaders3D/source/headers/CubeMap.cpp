#include "CubeMap.h"

CubeMap::CubeMap(){}	

CubeMap::CubeMap(vector<const GLchar*> texturePaths){ //load in textures
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	
	for (int i = 0; i < texturePaths.size(); i++) {
		image = IMG_Load(texturePaths[i]);
		if (image == NULL)
		{
			std::cout << "Image load failed...." << std::endl;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
		SDL_FreeSurface(image);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//wrapping & filtering methods
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	texture = textureID;	
}

void CubeMap::BindBuffers() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat), &this->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	doBuffer = false;
}

void CubeMap::Render(Shader shader, mat4 &projection, Vector3 &viewPosition, Vector3 &viewRotation) {
	if (doBuffer) BindBuffers();

	//glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);	//disable writing into the depth buffer.
	shader.Use();
	view = translate(mat4(), vec3(viewPosition.x, viewPosition.y, viewPosition.z));
	view = rotate(view, radians(viewRotation.x), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(viewRotation.y), vec3(0.0f, 1.0f, 0.0f));
	view = rotate(view, radians(viewRotation.z), vec3(0.0f, 0.0f, 1.0f));

	//set shader values
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(this->view))));

	glBindVertexArray(VAO);
	glUniform1i(glGetUniformLocation(shader.program, "cubeMap"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap() {}
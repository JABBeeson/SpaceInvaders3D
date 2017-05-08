#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, GLuint texture) {
//Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices) {
	//universal setup
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;

	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;

	scale.x = 1.0f;
	scale.y = 1.0f;
	scale.z = 1.0f;

	this->vertices = vertices;
	this->indices = indices;
	this->texture = texture;
}

void Mesh::BindMeshBuffers() {
	// taken from https://learnopengl.com/#!Model-Loading/Mesh
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);

	doBuffer = false;
}

void Mesh::Render(Shader &shader, mat4 &projection, Vector3 &viewPos, Vector3 &viewRot) {
	if (doBuffer) BindMeshBuffers();
	shader.Use();	// sets currct program object

	//adapt model to have specified values
	model = translate(mat4(), vec3(position.x, position.y, position.z));
	model = rotate(model, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	model = rotate(model, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, vec3(scale.x, scale.y, scale.z));

	//set view position and rotation
	view = translate(mat4(), vec3(viewPos.x, viewPos.y, viewPos.z));
	view = rotate(view, radians(viewRot.x), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(viewRot.y), vec3(0.0f, 1.0f, 0.0f));
	view = rotate(view, radians(viewRot.z), vec3(0.0f, 0.0f, 1.0f));
	
	//set light values
	glUniform3f(glGetUniformLocation(shader.program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
	glUniform3f(glGetUniformLocation(shader.program, "light.direction"), 1.0f, -0.5f, -1.0f);
	glUniform3f(glGetUniformLocation(shader.program, "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(shader.program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(shader.program, "light.specular"), 0.2f, 0.2f, 0.2f);

	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	//render
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(this->VAO);	//bind buffer, 
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
	glBindVertexArray(0);	//unbind buffer
	glBindTexture(GL_TEXTURE_2D, 0);

}

Mesh::~Mesh()
{
}

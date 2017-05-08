#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include "Includes.h"

#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"

using namespace glm;

class Model 
{
private:
	vector<Mesh>meshes;
	string directory;
	GLuint textureID;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	
public:
	float dyingTimer = 0.0f;
	bool isAlive = true;
	GLuint texture;
	Model();
	Model(GLchar* path, GLchar* texture);
	void SetPos(GLfloat x, GLfloat y, GLfloat z);
	void Move(GLfloat x, GLfloat y, GLfloat z);
	void Rotate(GLfloat x, GLfloat y, GLfloat z);
	void Scale(GLfloat v);
	void Render(Shader shader, mat4 &projectionMatrix, Vector3 viewMatrix, Vector3 viewRotation);
	void Remove(bool doRemove);
	~Model();
private:
	void loadModel(string path, const char* texture );
	void processNode(aiNode* node, const aiScene* scene, const char* texture);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const char* texture);
	GLuint loadTexture(const char* texturePath);
};


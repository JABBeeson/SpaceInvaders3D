#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include "Vertex.h"
#include "Vector.h"

#include "Includes.h"
#include "Shader.h"

using namespace glm;
using namespace std;

class Mesh 
{
private:
	GLuint VAO, VBO, EBO;
	bool doBuffer = true;
	mat4 _trans, _rot, _scale, model, view;	
public:
	vec3 position, rotation, scale;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint texture;

	Mesh(vector<Vertex> vertices, vector<GLuint> indicies, GLuint texture);
	void BindMeshBuffers();
	void Render(Shader &shader, mat4 &projection, Vector3 &viewPos, Vector3 &viewRot);
	~Mesh();
};


#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Includes.h"
using namespace std;

class Shader
{
private:
	const GLchar* vertexCode;
	const GLchar* fragmentCode;
public:
	GLuint vertex, fragment, program;
	GLint success;
	GLchar infoLog[512];
	Shader();
	Shader(string vertexPath, string fragmentPath);
	void Use();
	~Shader();
};


#include "Shader.h"

Shader::Shader() {}

Shader::Shader(std::string vertexPath, std::string fragmentPath) {
	
	std::string vertexSource, fragmentSource;
	std::ifstream vertexFile, fragmentFile;

	//error filtering
	vertexFile.exceptions(std::ifstream::failbit| std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try {	// read in vertex and fragment file data to vertex and fragment source.
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		stringstream vertexStream, fragmentStream;

		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexSource = vertexStream.str();
		fragmentSource = fragmentStream.str();
	}
	catch (std::ifstream::failure error) {	// error catchment
		std::cout << "FAILED TO READ SHADER FILE" << std::endl;
	}

	//convert string to consistent string. 
	//must be formated to be used in glShaderSource.
	vertexCode = vertexSource.c_str();
	fragmentCode = fragmentSource.c_str();
	
	//generate vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::vertex::COMPILATON_FAILED\n" << infoLog << endl;
	}

	//generate fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::fragment::COMPILATON_FAILED\n" << infoLog << endl;
	}

	//concatinate/LINK both fragment and vertex shaders
	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->program, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	cout << "okay" << endl;

	//delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use()
{
	glUseProgram(this->program);
}

Shader::~Shader()
{
}

#include "shader.h"
#include <iostream>
#include <fstream>

static string LoadShader(const string& shaderFile);
static GLuint CreateShader(const string& text, GLenum shaderType);

Shader::Shader(const string& vertex, const string& fragment)
{
	program = glCreateProgram();
	shaders[0] = CreateShader(LoadShader(vertex), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(fragment), GL_FRAGMENT_SHADER);

	for (int i = 0; i < 2; i++) {
		glCompileShader(shaders[i]);
		glAttachShader(program, shaders[i]);
	}


	glLinkProgram(program);
	glValidateProgram(program);

}


// Returns the the id of the created shader
static GLuint CreateShader(const string& text, GLenum shaderType) {

	//Create the shader
	GLuint shader = glCreateShader(shaderType);

	//Check if the shader was created successfully
	if (shader == 0)
		cerr << "Error creating shader type " << shaderType << endl;

	const GLchar* shaderSources[1];
	GLint ShaderSourceLengths[1];
	shaderSources[0] = text.c_str();
	ShaderSourceLengths[0] = text.length();

	glShaderSource(shader, 1, shaderSources, ShaderSourceLengths);
	return shader;
}

// Returns the shader program as is
static string LoadShader(const string& shaderFile) {
	ifstream file;
	file.open((shaderFile).c_str());

	string output;
	string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << shaderFile << std::endl;
	}

	return output;
}




Shader::~Shader()
{
	for (int i = 0; i < 2; i++) {
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	glDeleteProgram(program);
}


void Shader::Bind() {
	glUseProgram(program);
}

GLuint Shader::getProgramHandle() {
	return program;
}
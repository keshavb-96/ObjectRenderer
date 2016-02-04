#pragma once
#include <string>
#include <include_GLEW\GL\glew.h>
using namespace std;
class Shader
{
public:
	Shader(const string& vertex, const string& fragment);
	~Shader();
	void Bind();
	GLuint getProgramHandle();

private:
	enum {
		TRANSFORM_U,
		NUM_UNIFORMS
	};
	GLuint program;
	GLuint shaders[2];
	GLuint uniforms[NUM_UNIFORMS];
};


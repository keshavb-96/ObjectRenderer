#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <include_GLEW\GL\glew.h>
#include <include_GLM\glm\gtx\transform.hpp>

using namespace std;

struct Point {
	float x;
	float y;
	float z;
public:
	Point(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}
};

class Model
{
public:
	Model(const string& filename);
	void Draw();
	float circumRadius();
	void setStdTransform();
	void updateTransform(double scaleBy, double transX, double transY);
	void updateShaderUniforms(GLuint programHandle);
	~Model();
private:
	vector<Point> points; //The points in the mesh
	vector<GLuint> indices; // the vertices that make up the faces of the traingles in the final mesh, every three vertices make up a triangle
	vector<glm::vec3> normals; // normals for each vertex 
	glm::mat4 transform;
	glm::vec3 center;
	GLuint VAO;
};


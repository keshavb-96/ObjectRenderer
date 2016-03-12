#include "model.h"
#include <include_GLM\glm\gtx\quaternion.hpp>	
#include <include_GLM\glm\gtc\quaternion.hpp>
#include <math.h>


static glm::vec3 findPlaneNormal(Point a, Point b, Point c) {
	glm::vec3 first(a.x - b.x, a.y - b.y, a.z - b.z);
	glm::vec3 second(a.x - c.x, a.y - c.y, a.z - c.z);
	return glm::cross(first, second);
}

Model::Model(const string& filename) {

	// Obtaining model vertices and face indices information
	vector<GLfloat> coordinates;
	string line;
	ifstream objFile(filename.c_str());

	if (objFile.is_open()) {
		while (!objFile.eof())
		{
			getline(objFile, line);
			stringstream ss(line);
			string tmp;

			// Vertex Position data
			if (line[0] == 'v') {
				while (getline(ss, tmp, ' ')) {
					if (tmp[0] != 'v')
						coordinates.push_back(stof(tmp));
				}
				normals.push_back(glm::vec3(0, 0, 0));
			}

			// Face indices
			else if (line[0] == 'f') {
				while (getline(ss, tmp, ' ')) {
					if (tmp[0] != 'f') {
						indices.push_back((stoi(tmp) - 1)); //obj files index starting from 1, internally the GPU requires it to be indexed starting with 0
					}
				}
			}
		}
	}

	else {
		cerr << "Could not open file" << endl;
	}


	for (int i = 0; i < coordinates.size(); i = i + 3) {
		Point p(coordinates[i], coordinates[i + 1], coordinates[i + 2]);
		points.push_back(p);
	}

	glm::vec3 tmpNormal;
	for (int i = 0; i < indices.size(); i += 3) {
		//indices[i], indices[i+1], indices[i+2] form a triangle
		tmpNormal = findPlaneNormal(points[indices[i]], points[indices[i + 1]], points[indices[i + 2]]);
		normals[indices[i]] += tmpNormal;
		normals[indices[i + 1]] += tmpNormal;
		normals[indices[i + 2]] += tmpNormal;
	}


	// Sending vertex and index information to the GPU

	GLuint VBO, index_BO, NBO;

	//Generate and bind vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Send vertex information to the GPU
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * points.size()*sizeof(float), &points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Send normal information to the GPU
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	//Generate index buffer and send index data to the GPU
	glGenBuffers(1, &index_BO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_BO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
}



void Model::Draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}


float Model::circumRadius() {
	GLfloat xMin = points[0].x, xMax = points[0].x;
	GLfloat yMin = points[0].y, yMax = points[0].y;
	GLfloat zMin = points[0].z, zMax = points[0].z;


	for (int i = 0; i < points.size(); i++) {
		if (xMin > points[i].x)
			xMin = points[i].x;
		if (xMax < points[i].x)
			xMax = points[i].x;
		if (yMin > points[i].y)
			yMin = points[i].y;
		if (yMax < points[i].y)
			yMax = points[i].y;
		if (zMin > points[i].z)
			zMin = points[i].z;
		if (zMax < points[i].z)
			zMax = points[i].z;
	}

	center = glm::vec3((xMax + xMin) / 2, (yMax + yMin) / 2, (zMax + zMin) / 2);

	return glm::sqrt(glm::pow((xMax - xMin), 2) + glm::pow((yMax - yMin), 2) + glm::pow((zMax - zMin), 2)) / 2;
}

void Model::setStdTransform() {
	float scaleFactor = 1 / circumRadius();
	glm::mat4 transMat = glm::translate(-(this->center));
	glm::mat4 scaleMat = glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	glm::vec3 eulerAngles(0, 0, 0);
	glm::quat rotQuat(eulerAngles);
	glm::mat4 rotationMat = glm::toMat4(rotQuat);

	transform = rotationMat * scaleMat * transMat;
}

void Model::updateTransform(double scaleBy, double transX, double transY) {
	glm::mat4 scaleByMat = glm::scale(glm::vec3(scaleBy, scaleBy, scaleBy));
	glm::mat4 panMat = glm::translate(glm::vec3(-transX, -transY, 0));
	transform = scaleByMat * panMat * transform;
}

void Model::updateShaderUniforms(GLuint programHandle) {
	GLuint location = glGetUniformLocation(programHandle, "transform");
	glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);
}



Model::~Model()
{
	glDeleteVertexArrays(1, &VAO);
}

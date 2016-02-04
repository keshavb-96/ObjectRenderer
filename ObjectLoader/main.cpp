#include <iostream>
#include <include_GLEW\GL\glew.h>
#include <include_GLFW\GLFW\glfw3.h>
#include "shader.h"
#include "model.h"
using namespace std;

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 1200


GLFWwindow* window;
Model* m;
bool isLeftClicked = false;
double refX, refY;

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset < 0)
		m->updateTransform(1 / 1.15, 0, 0);
	else if (yoffset > 0)
		m->updateTransform(1.15, 0, 0);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		isLeftClicked = true;
		glfwGetCursorPos(window, &refX, &refY);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isLeftClicked = false;
	}
}

int main(int argc, char** argv) {
	if (!glfwInit()) {
		cout << "Glfw failed to initialize" << endl;
	}

	double xFin, yFin;
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "hello world", NULL, NULL);

	glfwMakeContextCurrent(window);

	glewInit();


	Shader shader("diffuseVert.txt", "diffuseFrag.txt");
	m = new Model("C:/Users/keshavb96/Desktop/ObjectFiles/bunny.obj");      // Pass obj file to be drawn here
	m->setStdTransform();

	//Callbacks
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Bind();

		if (isLeftClicked) {
			glfwGetCursorPos(window, &xFin, &yFin);
			m->updateTransform(1.0, (xFin - refX) / 10000, (refY - yFin) / 10000);
		}

		m->updateShaderUniforms(shader.getProgramHandle());
		m->Draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
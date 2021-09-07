#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

GLFWwindow *createWindow(int majorVersion, int minorVersion, int profile, int width, int height,
	const char *title, GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr) {

	glfwSetErrorCallback(
		[](int error, const char *description) {
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit()) {
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
	GLFWwindow *window = glfwCreateWindow(width, height, title, monitor, share);
	if (!window) {
		glfwTerminate();
		return nullptr;
	}

	return window;
}

void addHandlers(GLFWwindow *window) {
	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		}
	);
}

void configureGL(GLFWwindow *window) {
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);
}

void renderLoop(GLFWwindow *window) {
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void destroyWindow(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	
	// We request an OpenGL 4.6 context in a 1080p window
	GLFWwindow* window = createWindow(4, 6, GLFW_OPENGL_CORE_PROFILE, 1920, 1080, "Main window");
	if (!window) {
		exit(EXIT_FAILURE);
	}

	addHandlers(window);
	configureGL(window);
	renderLoop(window);
	destroyWindow(window);

	return 0;
}
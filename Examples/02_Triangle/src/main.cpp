#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static const char *vertexShaderCode = R"(
#version 460 core
layout (location=0) out vec3 color;
const vec2 pos[3] = vec2[3] (
	vec2(-0.6, -0.4),
	vec2(0.6, -0.4),
	vec2(0.0, 0.6)
);
const vec3 col[3] = vec3[3] (
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);
void main() {
	gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
	color = col[gl_VertexID];
}
)";
static const char* fragmentShaderCode = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main() {
	out_FragColor = vec4(color, 1.0);
}
)";

GLFWwindow* createWindow(int, int, int, int, int,
	const char*, GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr);
void addHandlers(GLFWwindow*);
GLuint createVAO();
GLuint createProgram(GLuint, GLuint);
GLuint createShader(const GLchar* const*, unsigned int);
void configureGL(GLFWwindow*);
void renderLoop(GLFWwindow*);
void resizeWindow(GLFWwindow*);
void clear(GLFWwindow*);
void draw(GLFWwindow*);
void destroyWindow(GLFWwindow*);
void destroyResources(GLuint, GLuint, GLuint, GLuint);

int main() {

	// We request an OpenGL 4.6 context in a 1080p window
	GLFWwindow* window = createWindow(4, 6, GLFW_OPENGL_CORE_PROFILE, 1920, 1080, "Main window");
	if (!window) {
		exit(EXIT_FAILURE);
	}

	addHandlers(window);
	configureGL(window);
	GLuint vaoId = createVAO();
	GLuint vsId = createShader(&vertexShaderCode, GL_VERTEX_SHADER);
	GLuint fsId = createShader(&fragmentShaderCode, GL_FRAGMENT_SHADER);
	GLuint programId = createProgram(vsId, fsId);
	renderLoop(window);
	destroyResources(vaoId, vsId, fsId, programId);
	destroyWindow(window);

	return 0;
}

GLFWwindow *createWindow(int majorVersion, int minorVersion, int profile, int width, int height,
	const char *title, GLFWmonitor *monitor, GLFWwindow *share) {

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

GLuint createVAO() {
	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	return vao;
}

GLuint createProgram(GLuint vsId, GLuint fsId) {
	const GLuint program = glCreateProgram();
	glAttachShader(program, vsId);
	glAttachShader(program, fsId);
	glLinkProgram(program);
	glUseProgram(program);
	return program;
}

GLuint createShader(const GLchar *const *source, unsigned int shaderType) {
	const GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, source, nullptr);
	glCompileShader(shader);
	return shader;
}

void configureGL(GLFWwindow *window) {
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);
}

void renderLoop(GLFWwindow *window) {
	while (!glfwWindowShouldClose(window)) {
		resizeWindow(window);
		clear(window);
		draw(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void resizeWindow(GLFWwindow *window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void clear(GLFWwindow *window) {
	glClearColor(.0f, .0f, .0f, .0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void draw(GLFWwindow* window) {
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void destroyResources(GLuint vaoID, GLuint vsId, GLuint fsId, GLuint progId) {
	glDeleteProgram(progId);
	glDeleteShader(vsId);
	glDeleteShader(fsId);
	glDeleteVertexArrays(1, &vaoID);
}

void destroyWindow(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
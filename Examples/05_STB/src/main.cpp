#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>

using glm::mat4;
using glm::vec3;

static const char* vertexShaderCode = R"(
#version 460 core
// We define a layout with the same data as the one in the buffer
// See https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
// for more information
layout (std140, location=0) uniform PerFrameData {
	uniform mat4 MVP;
	uniform int isWireframe;
	// We need to use padding because buffer offsets are 16 bit aligned
	uniform int padding1;
	uniform int padding2;
	uniform int padding3;
};
layout (location=0) out vec3 color;
const vec3 pos[8] = vec3[8] (
	vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, 1.0),
	vec3(1.0, 1.0, 1.0), vec3(-1.0, 1.0, 1.0),
	vec3(-1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0),
	vec3(1.0, 1.0, -1.0), vec3(-1.0, 1.0, -1.0)
);
const vec3 col[8] = vec3[8] (
	vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0)
);
const int indices[36] = int[36] (
	// front
	0, 1, 2, 2, 3, 0,
	// right
	1, 5, 6, 6, 2, 1,
	// back
	7, 6, 5, 5, 4, 7,
	// left
	4, 0, 3, 3, 7, 4,
	// bottom
	4, 5, 1, 1, 0, 4,
	// top
	3, 2, 6, 6, 7, 3
);
void main() {
	int index = indices[gl_VertexID];
	gl_Position = MVP * vec4(pos[index], 1.0);
	color = isWireframe > 0 ? vec3(1.0) : col[index];
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
void captureScreenshot(GLFWwindow*);
std::string getCurrentTimeString();
std::string timeToString(const std::tm*);
std::tm* getCurrentTime();
GLuint createVAO();
GLuint createProgram(GLuint, GLuint);
GLuint createShader(const GLchar* const*, unsigned int);
GLuint createBuffer();
void configureGL(GLFWwindow*);
void renderLoop(GLFWwindow*, GLuint);
float resizeWindow(GLFWwindow*);
void clear(GLFWwindow*);
void setup();
void draw(GLFWwindow*, GLuint, GLsizeiptr, const float);
void destroyWindow(GLFWwindow*);
void destroyResources(GLuint, GLuint, GLuint, GLuint, GLuint);

// Define a uniform buffer to pass data to the shader
struct PerFrameData
{
	mat4 mvp;
	int isWireframe;
	int padding1;
	int padding2;
	int padding3;
};

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
	GLuint perFrameDataBuffer = createBuffer();
	renderLoop(window, perFrameDataBuffer);
	destroyResources(vaoId, vsId, fsId, programId, perFrameDataBuffer);
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
			else if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
				captureScreenshot(window);
			}
		}
	);
}

void captureScreenshot(GLFWwindow *window) {
	int width, height;
	std::string now = getCurrentTimeString();
	glfwGetFramebufferSize(window, &width, &height);
	uint8_t* ptr = (uint8_t*)malloc(width * height * 4);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
	stbi_write_png((now + ".png").c_str(), width, height, 4, ptr, 0);
	free(ptr);
}

std::string getCurrentTimeString() {
	const std::tm* time = getCurrentTime();
	return timeToString(time);
}

std::string timeToString(const std::tm* time) {
	char buffer[9];
	strftime(buffer, 9, "%Y%m%d", time);
	return std::string(buffer);
}

std::tm* getCurrentTime() {
	auto start = std::chrono::system_clock::now();
	std::time_t now = std::chrono::system_clock::to_time_t(start);
	return std::localtime(&now);
}

void configureGL(GLFWwindow* window) {
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);
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
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar *errorLog = new GLchar[(int)maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);

		fprintf(stderr, "Error compiling shader: %s\n", errorLog);
		glDeleteShader(shader); // Don't leak the shader.
		delete[] errorLog;
		return -1;
	}
	return shader;
}

GLuint createBuffer() {
	const GLsizeiptr kBufferSize = sizeof(PerFrameData);

	// We use the Direct-State-Access (DSA) functions instead of the classic bind-to-edit approach
	GLuint perFrameDataBuffer;
	glCreateBuffers(1, &perFrameDataBuffer);
	// We use GL_DYNAMIC_STORAGE_BIT to tell OpenGL we might update the content of the data store
	// We'll set a double-sized buffer to account for each rendering state
	glNamedBufferStorage(perFrameDataBuffer, kBufferSize * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);
	return perFrameDataBuffer;
}

void renderLoop(GLFWwindow *window, GLuint perFrameDataBuffer) {
	const GLsizeiptr kBufferSize = sizeof(PerFrameData);

	while (!glfwWindowShouldClose(window)) {
		const float ratio = resizeWindow(window);
		clear(window);
		setup();
		draw(window, perFrameDataBuffer, kBufferSize, ratio);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

float resizeWindow(GLFWwindow *window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	return width / (float)height;
}

void clear(GLFWwindow *window) {
	glClearColor(.0f, .0f, .0f, .0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setup() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_LINE);
	// We use the polygon offset to render a wireframe on top of the solid image without z-fighting
	glPolygonOffset(-1.0f, -1.0f);
}

void draw(GLFWwindow* window, GLuint perFrameDataBuffer, GLsizeiptr kBufferSize, const float ratio) {
	// We rotate the cube on the (1, 1, 1) axis by glfwGetTime() and then we translate it backwards to see it
	const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.5f)), (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
	const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

	// Define the two instances of perFrameData that we'll use to render the cube and the wireframe
	PerFrameData perFrameData[2] = { {.mvp = p * m, .isWireframe = false }, {.mvp = p * m, .isWireframe = true } };
	glNamedBufferSubData(perFrameDataBuffer, 0, kBufferSize * 2, perFrameData);

	// Draw the cube
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, 0, kBufferSize);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Draw the wireframe
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, kBufferSize, kBufferSize);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void destroyResources(GLuint vaoID, GLuint vsId, GLuint fsId, GLuint progId, GLuint perFrameDataBuffer) {
	glDeleteBuffers(1, &perFrameDataBuffer);
	glDeleteProgram(progId);
	glDeleteShader(vsId);
	glDeleteShader(fsId);
	glDeleteVertexArrays(1, &vaoID);
}

void destroyWindow(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
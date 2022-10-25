#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint trf1loc, trf1loc1, prloc, prloc1;
float a;

glm::mat4 trf;
GLuint vbo, ibo;
GLuint shp, shp1;
GLuint texColorBuffer, otherTex;
GLuint texloc;
unsigned int fbo;

struct VertexData
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texcoord;
	float weight1;
};

std::vector<glm::vec3> verts;
std::vector<unsigned int> indices;

void display()
{
	glFrontFace(GL_CCW);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glUseProgram(shp);
	glClearColor(0, 1, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glUseProgram(shp1);

	//glFrontFace(GL_CW);
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(texloc, 0);
	//glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	//glClearColor(1, 1, 1, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}

void timer(int)
{
	trf = glm::mat4(1);
	a += 30;
	trf = glm::rotate(trf, a * 0.0005f, glm::vec3(1, 0, 1));
	trf = glm::translate(trf, glm::vec3(-0.5));

	auto cpos = glm::vec3(1.2);
	glm::mat4 proj = glm::perspective(45.f, 1.0f, 0.1f, 100.f);
	glm::mat4 lookAt = glm::lookAt(cpos, glm::vec3(0), glm::vec3(0, 1, 0));

	proj = proj * lookAt;

	glUseProgram(shp);
	glUniformMatrix4fv(trf1loc, 1, GL_FALSE, glm::value_ptr(trf));
	glUniformMatrix4fv(prloc, 1, GL_FALSE, glm::value_ptr(proj));

	glUseProgram(shp1);
	glUniformMatrix4fv(trf1loc1, 1, GL_FALSE, glm::value_ptr(trf));
	glUniformMatrix4fv(prloc1, 1, GL_FALSE, glm::value_ptr(proj));

	glutPostRedisplay();
	glutTimerFunc(30, timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);

	glewInit();

	shp = glCreateProgram();

	GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vshCode =
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		"out vec3 out_pos;"
		"uniform mat4 transf1;"
		"uniform mat4 proj;"
		"void main(){"
		"out_pos = pos;"
		"gl_Position = proj*transf1*vec4(pos, 1);}";
	GLint vshlen = strlen(vshCode);
	glShaderSource(vsh, 1, &vshCode, &vshlen);
	glAttachShader(shp, vsh);
	glCompileShader(vsh);

	GLint isCompiled = 0;
	glGetShaderiv(vsh, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vsh, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vsh, maxLength, &maxLength, &errorLog[0]);

		std::cout << "VS" << std::string(errorLog.data());
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(vsh); // Don't leak the shader.
		return 0;
	}
	GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fshCode =
		"#version 330 core\n"
		"in vec3 out_pos;"
		"out vec4 FragColor;"
		"void main(){"
		"FragColor = vec4(out_pos, 1);}";
	GLint fshlen = strlen(fshCode);
	glShaderSource(fsh, 1, &fshCode, &fshlen);
	glAttachShader(shp, fsh);
	glCompileShader(fsh);

	isCompiled = 0;
	glGetShaderiv(fsh, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fsh, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fsh, maxLength, &maxLength, &errorLog[0]);


		std::cout << "FS" << std::string(errorLog.data());
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(fsh); // Don't leak the shader.
		return 0;
	}
	glValidateProgram(shp);
	glLinkProgram(shp);
	glUseProgram(shp);

	glEnable(GL_DEPTH_TEST);

	verts.push_back(glm::vec3(0, 0, 0));
	verts.push_back(glm::vec3(1, 0, 0));
	verts.push_back(glm::vec3(1, 1, 0));
	verts.push_back(glm::vec3(0, 1, 0));
	verts.push_back(glm::vec3(0, 0, 1));
	verts.push_back(glm::vec3(1, 0, 1));
	verts.push_back(glm::vec3(1, 1, 1));
	verts.push_back(glm::vec3(0, 1, 1));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(4);
	indices.push_back(4);
	indices.push_back(7);
	indices.push_back(6);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(0);

	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(2);
	indices.push_back(1);

	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(4);
	indices.push_back(0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);

	GLuint posloc = 0;
	glEnableVertexAttribArray(posloc);
	glVertexAttribPointer(posloc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	trf1loc = glGetUniformLocation(shp, "transf1");
	prloc = glGetUniformLocation(shp, "proj");

	trf = glm::mat4(1);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	shp1 = glCreateProgram();
	GLuint vsh1 = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vshCode1 =
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		"out vec3 out_pos;"
		"uniform mat4 transf1;"
		"uniform mat4 proj;"
		"void main(){"
		"out_pos = pos;"
		"gl_Position = proj*transf1*vec4(pos, 1);}";
	GLint vshlen1 = strlen(vshCode1);
	glShaderSource(vsh1, 1, &vshCode1, &vshlen1);
	glAttachShader(shp1, vsh1);
	glCompileShader(vsh1);

	isCompiled = 0;
	glGetShaderiv(vsh1, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vsh1, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vsh1, maxLength, &maxLength, &errorLog[0]);

		std::cout << "VS" << std::string(errorLog.data());
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(vsh1); // Don't leak the shader.
		return 0;
	}
	GLuint fsh1 = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fshCode1 =
		"#version 330 core\n"
		"in vec3 out_pos;"
		"uniform sampler2D opposite_side;"
		"out vec4 FragColor;"
		"float func(vec3 pos){"
		"return 0.002f/(0.1f + (pos.x-0.5f)*(pos.x-0.5f) + (pos.y-0.5f)*(pos.y-0.5f) + (pos.z-0.5f)*(pos.z-0.5f));}"
		"void main(){"
		"vec3 ray_in = out_pos;"
		"vec3 ray_out = texture(opposite_side, gl_FragCoord.xy/vec2(512)).xyz;"
		"float delta = 0.01, sum = 0;"
		"int i = 0;"
		"for(i = 0; i < length(ray_out - ray_in)/delta; i++){"
		"sum += func(ray_in + i*delta*normalize(ray_out - ray_in));}"
		"FragColor = vec4(vec3(sum), 1);}"; //"FragColor = vec4(vec3(sum), 1);}"
	GLint fshlen1 = strlen(fshCode1);
	glShaderSource(fsh1, 1, &fshCode1, &fshlen1);
	glAttachShader(shp1, fsh1);
	glCompileShader(fsh1);

	isCompiled = 0;
	glGetShaderiv(fsh1, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fsh1, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fsh1, maxLength, &maxLength, &errorLog[0]);


		std::cout << "FS" << std::string(errorLog.data());
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(fsh1); // Don't leak the shader.
		return 0;
	}

	glValidateProgram(shp1);
	glLinkProgram(shp1);
	glUseProgram(shp1);
	texloc = glGetUniformLocation(shp1, "opposite_side");
	trf1loc1 = glGetUniformLocation(shp1, "transf1");
	prloc1 = glGetUniformLocation(shp1, "proj");

	int width, height, channels;
	unsigned char* img = stbi_load("sky.jpg", &width, &height, &channels, 0);


	glGenTextures(1, &otherTex);
	glBindTexture(GL_TEXTURE_2D, otherTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();
}
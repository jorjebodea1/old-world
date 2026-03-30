//
//  main.cpp
//  OpenGL_Shader_Example_step1
//
//  Created by CGIS on 02/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Utilities/stb_image_writer.h"

#if defined (__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
#else
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <algorithm>
#include "Utilities/Shader.hpp"
#include "Entities/Camera.hpp"
#include "Models/Model3DOBJ.hpp"
#include "Models/Model3DGLB.hpp"
#include "InputHandling/Keyboard.hpp"
#include "InputHandling/Mouse.hpp"
#include "Utilities/WorldLoader.hpp"
#include <filesystem>
#include "Utilities/Transform.hpp"
#include "CollisionSystem/Collision.hpp"
#include "Entities/StaticEntity.hpp"
#include "Skybox/SkyBox.hpp"
#include "Shadow/Shadow.hpp"
#include "Cinematic/CinematicCamera.hpp"
#include "Chat/Chat.hpp"

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

gps::Shader skyboxShader;
gps::Shader worldShader;
gps::Shader myCustomShader;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;
gps::Model3DOBJ screenQuad;


double lastFrameTime = 0;
float delta;

gps::WorldLoader *WorldLoader=gps::WorldLoader::getInstance();
gps::CinematicCamera cinematicCamera;
int activeFog = 0;

bool initOpenGLWindow() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for multisampling/antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Texturing", NULL, NULL);

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

    glfwSetKeyCallback(glWindow, gps::Keyboard::keyboardCallback);
	glfwSetCharCallback(glWindow, gps::Keyboard::characterCallback);
	glfwSetMouseButtonCallback(glWindow, gps::Mouse::mouseButtonCallback);
	glfwSetCursorPosCallback(glWindow, gps::Mouse::mouseMovementCallback);
	glfwSetScrollCallback(glWindow, gps::Mouse::scrollCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(glWindow);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

double getCurrentTime() {
	auto now = std::chrono::high_resolution_clock::now();
	auto ticks = now.time_since_epoch().count();
	auto resolution = std::chrono::high_resolution_clock::period::den;
	return static_cast<double>(ticks)*1000 / resolution;
}
void renderScene(gps::Camera& myCamera,gps::Entity& myPlayer,glm::mat4 projection,gps::Shadow shadow)
{
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(shadow.computeLightSpaceTrMatrix(myPlayer.getPosition())));
	glViewport(0, 0, 16384, 16384);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, myPlayer.getPosition());
	model = glm::rotate(model, glm::radians(myPlayer.getRotY()), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(100.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	myPlayer.getModel().Draw(depthMapShader);
	for (size_t i = 0; i < WorldLoader->entities.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, WorldLoader->entities[i]->getPosition());
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(WorldLoader->entities[i]->getScale()));
		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		WorldLoader->entities[i]->getModel()->Draw(depthMapShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, retina_width, retina_height);

	glClear(GL_COLOR_BUFFER_BIT);

	//screenQuadShader.useShaderProgram();

	////bind the depth map
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, shadow.depthMapTexture);
	//glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

	//glDisable(GL_DEPTH_TEST);
	//screenQuad.Draw(screenQuadShader);
	//glEnable(GL_DEPTH_TEST);

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, retina_width, retina_height);

	myCustomShader.useShaderProgram();
	GLint viewLoc;
	glm::mat4 view;
	if (cinematicCamera.isActive) {
		view = cinematicCamera.getViewMatrix(delta);
		viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
	else {
		//initialize the view matrix
		view = myCamera.getViewMatrix();
		//send matrix data to shader
		viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create player model
	model = glm::translate(model, myPlayer.getPosition());
	model = glm::rotate(model, glm::radians(myPlayer.getRotY()), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	std::vector<glm::mat4> jointTransforms=myPlayer.updateAnimation(delta);
	glBindBuffer(GL_TEXTURE_BUFFER, myPlayer.getModel().getBoneBuffer());
	glBufferSubData(GL_TEXTURE_BUFFER, 0, jointTransforms.size() * sizeof(glm::mat4), &jointTransforms[0]);
	
	////bind the shadow map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadow.depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(shadow.computeLightSpaceTrMatrix(myPlayer.getPosition())));
	myPlayer.getModel().Draw(myCustomShader);


	worldShader.useShaderProgram();
	////bind the shadow map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadow.depthMapTexture);
	glUniform1i(glGetUniformLocation(worldShader.shaderProgram, "shadowMap"), 3);
	glUniformMatrix4fv(glGetUniformLocation(worldShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(shadow.computeLightSpaceTrMatrix(myPlayer.getPosition())));
	glUniformMatrix4fv(glGetUniformLocation(worldShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	for (size_t i = 0; i < WorldLoader->entities.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, WorldLoader->entities[i]->getPosition());
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(WorldLoader->entities[i]->getRotation().z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(WorldLoader->entities[i]->getScale()));
		glUniformMatrix4fv(glGetUniformLocation(worldShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(worldShader.shaderProgram, "objectType"), WorldLoader->entities[i]->getObjectType());
		glUniform1i(glGetUniformLocation(worldShader.shaderProgram, "activeFog"), activeFog);
		WorldLoader->entities[i]->getModel()->Draw(worldShader);
	}

	WorldLoader->skyBox.Draw(skyboxShader, view, projection);
}
void cleanup() {


    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	//glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cinematicCamera.LoadCinematicCamera();
	screenQuad.LoadModel((std::filesystem::path)"objects/quad/quad.obj");

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
	worldShader.loadShader("shaders/worldShader.vert", "shaders/worldShader.frag");
	myCustomShader.useShaderProgram();

	//initialize the projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	std::filesystem::path p2 = "textures/maps/newracestartzone";
	WorldLoader->initializeTiles(p2);
	WorldLoader->initSkybox();
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLightsPosition"), WorldLoader->lightsPosition.size(), glm::value_ptr(WorldLoader->lightsPosition[0]));


		
	worldShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(worldShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(worldShader.shaderProgram, "pointLightsPosition"), WorldLoader->lightsPosition.size(), glm::value_ptr(WorldLoader->lightsPosition[0]));

	gps::Collision CollisionSystem(*WorldLoader);//glm::vec3(3587.7f, 143.15f, 770.86f)
	gps::Player myPlayer(gps::Transform(glm::vec3(3616.9f, 96.231f, 1250.2f), glm::vec3(0.0f, 90.0f, 0.0f), 1.f),
		gps::AABB(glm::vec3(-0.74f, 0.06f, -0.54f), glm::vec3(0.42f, 2.43f, 0.59f)), CollisionSystem);
	gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 0.0f), myPlayer.getEntity());
	

	lastFrameTime = getCurrentTime();
	gps::Shadow shadow = gps::Shadow();
	cinematicCamera.playCinematic("Action");
	gps::Chat chat;
	while (!glfwWindowShouldClose(glWindow)) {
		double currentFrameTime = getCurrentTime();
		delta = (currentFrameTime - lastFrameTime) / 1000;
		lastFrameTime = currentFrameTime;

		if (!cinematicCamera.isActive) {
			chat.update();
			if (!chat.chatText.empty())
				std::cout << chat.chatText << "\n";
			if (chat.isActive == false && !chat.chatText.empty()) {
				myPlayer.getEntity().playAnimation(chat.chatText);
				chat.chatText.clear();
			}
			myPlayer.update(delta,chat.isActive);
			myCamera.update();
		}
		if (gps::Keyboard::isKeyDown(GLFW_KEY_Z)) {
			if (activeFog == 0) activeFog = 1;
			else activeFog = 0;
		}
		renderScene(myCamera, myPlayer.getEntity(), projection,shadow);
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}

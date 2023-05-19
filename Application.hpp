#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "Engine.hpp"
#include "Scene.hpp"

class Application {

private:

	Engine* graphics_engine;
	GLFWwindow* window;
	Scene* scene;

	double last_time, current_time;
	int num_frames;
	float frame_time;

	void buildGlfwWindow(const bool& debug_mode, int width, int height);

	void calculateFrameRate();

public:

	Application(const bool& debug, int width, int height);
	~Application();
	void runApplication();

};
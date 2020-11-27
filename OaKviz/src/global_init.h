#pragma once
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/Camera.h"
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr auto M_PI = 3.14159265359;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 1280.0f / 2.0;
float lastY = 720.0f / 2.0;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float m_lastMousePosX = 1280.0f / 2.0;
float m_lastMousePosY = 720.0f / 2.0;
bool arcball_on = false;
bool pan_on = false;
Camera m_camera(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 m_upVector(0.0f, 1.0f, 0.0f);

float pan_x = 0;
float pan_y = 0;
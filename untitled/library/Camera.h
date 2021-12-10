#pragma once
#include <glm/glm.hpp>

class Camera
{
	glm::vec3 position;
	glm::vec3 viewDirection;
	glm::vec3 UP;
	glm::vec2 oldMousePosition;
	static const float MOVEMENT_SPEED;
	glm::vec3 strafeDirection;

public:
	bool firstClick = true;
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 direction = glm::vec3(0, 0, 1), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	void mouseUpdate(const glm::vec2 &newMousePosition);
	glm::mat4 getWorldToViewMatrix() const;

	void moveForward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
	glm::vec3 getPosition() const { return position; }
};

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

const float Camera::MOVEMENT_SPEED = 0.1f;

Camera::Camera(glm::vec3 Position, glm::vec3 direction, glm::vec3 up)
{
	viewDirection = direction;
	// viewDirection = glm::vec3(0.663740635f, -0.492421985f, 0.562995136f);
	// position = glm::vec3(-3.11094999f, 2.23070025f, -2.86016989f);
	position = Position;
	UP = up;
	// oldMousePosition = glm::vec2(0,0);
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
    if(firstClick==true)
    {
        oldMousePosition = newMousePosition;
        firstClick = false;
    }
    glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
    if (glm::length(mouseDelta) > 500.0f)
    {
        oldMousePosition = newMousePosition;
        return;
    }
    const float ROTATIONAL_SPEED = 0.05f;
    strafeDirection = glm::cross(viewDirection, UP);
    glm::mat4 rotator =	glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED/5, UP) *
                        glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED/5, strafeDirection);

    viewDirection = glm::mat3(rotator) * viewDirection;

    oldMousePosition = newMousePosition;
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveForward()
{
	position += MOVEMENT_SPEED * viewDirection;
}

void Camera::moveBackward()
{
	position += -MOVEMENT_SPEED * viewDirection;
}

void Camera::strafeLeft()
{
	position += -MOVEMENT_SPEED * strafeDirection;
}

void Camera::strafeRight()
{
	position += MOVEMENT_SPEED * strafeDirection;
}

void Camera::moveUp()
{
	position += MOVEMENT_SPEED * UP;
}

void Camera::moveDown()
{
	position += -MOVEMENT_SPEED * UP;
}

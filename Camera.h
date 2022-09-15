#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//Define camera's possible movements

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

//Abstract camera class for vector and matrices calculations

class Camera
{
public:
	//camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//euler angles
	float Yaw;
	float Pitch;
	//camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	//constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	//constructor with scalar vectors
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}


	glm::mat4 lookAt(glm::vec3 Position, glm::vec3 Target, glm::vec3 Upward)
	{
		glm::mat4 translation = glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4(1.0f);

		//update translation matrix
		translation[3][0] = -Position.x;
		translation[3][1] = -Position.y;
		translation[3][2] = -Position.z;

		//recalculate forward (AKA negative z-axis), upward, and right vectors
		glm::vec3 cameraForward = glm::normalize(Position - Target);
		glm::vec3 cameraRight = glm::normalize(glm::cross(Upward, cameraForward));
		glm::vec3 cameraUpward = glm::normalize(glm::cross(cameraForward, cameraRight));

		//update rotation matrix
		rotation[0][0] = cameraRight.x;
		rotation[1][0] = cameraRight.y;
		rotation[2][0] = cameraRight.z;
		rotation[0][1] = cameraUpward.x;
		rotation[1][1] = cameraUpward.y;
		rotation[2][1] = cameraUpward.z;
		rotation[0][2] = cameraForward.x;
		rotation[1][2] = cameraForward.y;
		rotation[2][2] = cameraForward.z;

		return rotation * translation;

	}

	//return view matrix calculated by euler angles
	glm::mat4 GetViewMatrix()
	{
		return lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyBoard(Camera_Movement direction, float delta_time)
	{
		float velocity = MovementSpeed * delta_time;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	
	}

	//processinput received from a mouse cursor to input system. Expects offset in x and y directions
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constraintPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constraintPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}
private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		//recalculate right and up vectors accordingly
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif
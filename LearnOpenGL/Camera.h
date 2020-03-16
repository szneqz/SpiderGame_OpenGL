

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MapGenerator.h"

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 prevTargPos;
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	MapGenerator* map = nullptr;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	glm::vec3 target = glm::vec3(0.0f);

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		prevTargPos = position;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		prevTargPos = glm::vec3(posX, posY, posZ);
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 myLookAt(glm::vec3 cameraPos, glm::vec3 targetPos, glm::vec3 WorldUpVec)
	{	//prywatny lookAt
		glm::vec3 targetVec = glm::normalize(cameraPos - targetPos);							//wektor kierunku do celu
		glm::vec3 rightVec = glm::normalize(glm::cross(glm::normalize(WorldUpVec), targetVec));	//wektor prawy kamery
		glm::vec3 upVec = glm::cross(targetVec, rightVec);										//wektor gory kamery
		glm::mat4 lookAt = glm::mat4(1.0f);														//macierz poszczegolnych obrotow
		lookAt[0][0] = rightVec.x;	//pierwsza kolumna, pierwszy rzad
		lookAt[1][0] = rightVec.y;	//druga kolumna, pierwszy rzad
		lookAt[2][0] = rightVec.z;	//trzecia kolumna, pierwszy rzad
		lookAt[0][1] = upVec.x;		//pierwsza kolumna, drugi rzad
		lookAt[1][1] = upVec.y;
		lookAt[2][1] = upVec.z;
		lookAt[0][2] = targetVec.x;	//pierwsza kolumna trzeci rzad
		lookAt[1][2] = targetVec.y;
		lookAt[2][2] = targetVec.z;
		glm::mat4 posMatrix = glm::mat4(1.0f);													//macierz translacji
		posMatrix[3][0] = -cameraPos.x;
		posMatrix[3][1] = -cameraPos.y;
		posMatrix[3][2] = -cameraPos.z;

		return lookAt * posMatrix;
	}

	inline glm::vec3 easyLerp(glm::vec3 start, glm::vec3 end, float factor, float staticDist, float deltaTime)
	{	//przejscie z wektora do wektora zaleznie od wspolczynnika
		glm::vec3 result = glm::vec3(0.0f);
		staticDist = staticDist * deltaTime;

		if (glm::length(end - start) < staticDist)
			return end;

		result = start + glm::normalize(end - start) * staticDist;
		result = result + ((end - result) * factor * deltaTime);

		return result;
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix(float deltaTime)
	{
		//plynne przejscie kamery z punktu w punkt
		prevTargPos = easyLerp(prevTargPos, target, 2.5f, 0.1f, deltaTime);
		glm::mat4 tmpMatrix = myLookAt(prevTargPos, prevTargPos + Front, glm::vec3(0.0f, 1.0f, 0.0f));

		float tmpDist = map->isHit(prevTargPos, -Front, 5.0f);	//niewchodzenie pod mape
		if (tmpDist > 3.0f)	//ograniczenie od 0.8f do 3.0f odleglosci od postaci
			tmpDist = 3.0f;

		if (tmpDist < 0.8f)
			tmpDist = 0.8f;

		Position = prevTargPos - Front * tmpDist;
		return glm::translate(tmpMatrix, Front * tmpDist);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//Im wieksze zblizenie tym wolniej rusza sie kamera, wiec mozna lepiej sie przygladac
		Yaw += xoffset * Zoom / ZOOM;
		Pitch += yoffset * Zoom / ZOOM;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
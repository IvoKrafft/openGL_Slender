//
//  Camera.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 22.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "PhongShader.h"

Camera::Camera(GLFWwindow* pWin) : m_Position(0.0f, 2.0f, 3.0f), m_Target(0.0f, 2.0f, -1.0f), m_Up(0.0f, 1.0f, 0.0f),
m_LastMouseX(320), m_LastMouseY(240), m_Panning(0, 0, 0), m_Zoom(0, 0, 0),
m_Rotation(0, 0, 0), WindowWidth(640), WindowHeight(480), pWindow(pWin),
m_Yaw(0.0f), m_Pitch(0.0f), firstMouse(true)
{
    if(pWindow)
        glfwGetWindowSize(pWindow, &WindowWidth, &WindowHeight);
    
    m_ViewMatrix.identity();
    m_ProjMatrix.perspective((float)M_PI*65.0f/180.0f, (float)WindowWidth/(float)WindowHeight, 0.045f, 1000.0f);
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Vector Camera::position() const
{
    return m_Position;
}
Vector Camera::target() const
{
    return m_Target + m_Panning;
}
Vector Camera::up() const
{
    return m_Up;
}

void Camera::setPosition( const Vector& Pos)
{
    m_Position = Pos;
    m_Panning = m_Rotation = m_Zoom = Vector(0,0,0);


}
void Camera::setTarget( const Vector& Target)
{
    m_Target = Target;
    m_Panning = Vector(0,0,0);
}
void Camera::setUp( const Vector& Up)
{
    m_Up = Up;
}

void Camera::mouseInput( double xposIn, double yposIn)
{
    float x = static_cast<float>(xposIn);
    float y = static_cast<float>(yposIn);

    if (firstMouse)
    {
        m_LastMouseX = x;
        m_LastMouseY = y;
        firstMouse = false;
    }

    float xoffset = x - m_LastMouseX;
    float yoffset = m_LastMouseY - y;
    m_LastMouseX = x;
    m_LastMouseY = y;


    static const float sensitivity = 0.5f;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

            m_Yaw += xoffset;
            m_Pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;
            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;

            float bogYaw = m_Yaw * static_cast<float>(M_PI) / 180.0f;
            float bogPitch = m_Pitch * static_cast<float>(M_PI) / 180.0f;
            // Berechne die neue Blickrichtung der Kamera
            Vector front;
            front.X = cos(bogYaw) * cos(bogPitch);
            front.Y = sin(bogPitch);
            front.Z = sin(bogYaw) * cos(bogPitch);

            m_Target = front.normalize();
}

void Camera::keyboardMovment(){

    if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, true);

    float moveSpeed = 0.25f; // Geschwindigkeit der Bewegung anpassen

    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
        m_Position += m_Target * moveSpeed;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
        m_Position += -(m_Target).cross(m_Up) * moveSpeed;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
        m_Position += -(m_Target * moveSpeed);
    }
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
        m_Position += (m_Target).cross(m_Up) * moveSpeed;
    }

    //karten raender
    if (m_Position.X < -312.0f) {
	m_Position.X = -312.0f;
    }
    if (m_Position.Z < -353.0f) {
        m_Position.Z = -353.0f;
    }
    if (m_Position.X > 410.0f) {
        m_Position.X = 410.0f;
    }
    if (m_Position.Z > 330.0f) {
        m_Position.Z = 330.0f;
    }

    m_Position.Y = 1.0f;
}

const Matrix& Camera::getViewMatrix() const
{
    return m_ViewMatrix;
}

const Matrix& Camera::getProjectionMatrix() const
{
    return m_ProjMatrix;
}



Vector Camera::getVSpherePos( float x, float y)
{
    Vector p( 1.0f*x/(float)WindowWidth*2.0f - 1.0f, 1.0f*y/(float)WindowHeight*2.0f -1.0f, 0);
    p.Y = -p.Y;
    
    float sqrLen = p.lengthSquared();

    if( sqrLen <= 1.0f)
    {
        p.Z = sqrt( 1- sqrLen);
    }
    else
        p.normalize();
    
    return p;
    
}

void Camera::update()
{
    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);
    keyboardMovment();
    mouseInput(xpos, ypos);
    //std::cout<<m_Position.X<<" "<<m_Position.Y<<" "<<m_Position.Z<<std::endl;
    m_ViewMatrix.lookAt(m_Position+m_Target,m_Up,m_Position);
}
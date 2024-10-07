//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "ShaderLightmapper.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048), ShaderIndex(1), FirstUsed(false), SecondUsed(false), ThirdUsed(false), FourthUsed(false)
{
	createScene();
}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime)
{
    Cam.update(dtime);
	if (Cam.position().X <= -198 && Cam.position().X >= -204 && Cam.position().Z <= -202 && Cam.position().Z >= -208 && !FirstUsed) {

		if (glfwGetKey(pWindow, GLFW_KEY_F) == GLFW_PRESS) {      

			shaderSwitch(ShaderIndex);
			ShaderIndex++;
			FirstUsed = true;
		}
	}
	if (Cam.position().X <= -196 && Cam.position().X >= -202 && Cam.position().Z <= 194 && Cam.position().Z >= 188 && !SecondUsed) {

		if (glfwGetKey(pWindow, GLFW_KEY_F) == GLFW_PRESS) {      

			shaderSwitch(ShaderIndex);
			ShaderIndex++;
			SecondUsed = true;
		}
	}
	if (Cam.position().X <= 296 && Cam.position().X >= 290 && Cam.position().Z <= 211 && Cam.position().Z >= 205 && !ThirdUsed) {

		if (glfwGetKey(pWindow, GLFW_KEY_F) == GLFW_PRESS) {      

			shaderSwitch(ShaderIndex);
			ShaderIndex++;
			ThirdUsed = true;
		}
	}
	if (Cam.position().X <= 311 && Cam.position().X >= 305 && Cam.position().Z <= -257 && Cam.position().Z >= -263 && !FourthUsed) {

		if (glfwGetKey(pWindow, GLFW_KEY_F) == GLFW_PRESS) {      

			shaderSwitch(ShaderIndex);
			ShaderIndex++;
			FourthUsed = true;
		}
	}
	float moveSpeed = 10.0f * dtime;
	Vector dirCam = Cam.position() - pSlender->transform().translation();
	Matrix movement= pSlender->transform();
	float length = dirCam.length();
	dirCam.normalize();
	movement.m03 += dirCam.X * moveSpeed;
	//movement.m13 += dirCam.Y;
	movement.m23 += dirCam.Z * moveSpeed;
	
	//movement.translation(dirCam);
	//movement.scale(0.02f);
	
	
	/*
	std::cout << dirCam.X << " " << dirCam.Y << " " << dirCam.Z << std::endl;
	std::cout << movement.m00 << " " << movement.m01 << " " << movement.m02 << " " << movement.m03 << std::endl;
	std::cout << movement.m11 << " " << movement.m11 << " " << movement.m12 << " " << movement.m13 << std::endl;
	std::cout << movement.m20 << " " << movement.m21 << " " << movement.m22 << " " << movement.m23 << std::endl;
	std::cout << movement.m30 << " " << movement.m31 << " " << movement.m32 << " " << movement.m33 << std::endl << std::endl;
	//dynamic_cast<Model*>(pSlender->transform(movement));
	//movement.translation(dirCam);
	*/
	pSlender->transform(movement);
	//pSlender->transform().lookAt(dirCam,Vector(0.0f,1.0f,0.0f),pSlender->transform().translation());
	//std::cout << pSlender->transform() << std::endl;
	if (length <= 2.0f) {
		glfwSetWindowShouldClose(pWindow, true);
	}
}

void Application::shaderSwitch(int i) {
	switch (i)
	{
	default:
		break;
	case 1:
		dynamic_cast<PhongShader*>(pModel->shader())->auraBool(true);
		break;
	case 2:
		dynamic_cast<PhongShader*>(pModel->shader())->rimBool(true);
		break;
	case 3:
		dynamic_cast<PhongShader*>(pModel->shader())->invertBool(true);
		break;
	case 4:
		glfwSetWindowShouldClose(pWindow, true);
		break;
	}
}

void Application::draw()
{
	ShadowGenerator.generate(Models);
	
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderLightMapper::instance().activate();
    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
	ShaderLightMapper::instance().deactivate();
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

void Application::createScene()
{
	Matrix m,n;
	
		
	
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	m.translation(0, 0, 0);
	m.scale(5);
	pModel->transform(m);
	pModel->shadowCaster(false);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "Slender.fbx", false);
	pModel->shader(new PhongShader(), true);
	m.translation(150,0,150);
	m.scale(0.03f);
	pModel->transform(m);
	Models.push_back(pModel);
	pSlender = dynamic_cast<Model*>(pModel);
	
	pModel = new Model(ASSET_DIRECTORY "forest_map.obj", false);
	pModel->shader(new PhongShader(), true);
	m.translation(10, 0, -10);
	pModel->transform(m);
	Models.push_back(pModel);
	
	

	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.5, 0.5, 0.75));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	Color c = Color(1.0f, 0.7f, 1.0f);
	Vector a = Vector(1, 0, 0.1f);
	float innerradius = 45;
	float outerradius = 60;
	
	// point lights
	PointLight* pl = new PointLight();
	pl->position(Vector(-200, 3, 190));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-200, 3, -205));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(290, 3, 210));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(310, 3, -260));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-10, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(10.0f, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);
	
	
	// spot lights
	SpotLight* sl = new SpotLight();
	sl->position(Vector(-200, 3, 190));
	sl->color(c);
	sl->direction(Vector(1,-4,0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(-200, 3, -205));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(290, 3, 210));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(310, 3, -260));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(-10, 3, -8));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(10.0f, 3, -8));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
}
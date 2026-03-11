#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include <glm/glm.hpp>
#include <sstream>
#include "helper/texture.h"
#include <GLFW/glfw3.h>
using glm::vec3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f,10.0f,100,100) ,angle(0.0f),tPrev(0.0f),rotSpeed(glm::pi<float>()/8.0f), sky(100.0f)
{
    mesh = ObjMesh::load("media/Statue.obj", true,true);  
}



void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    setupFBO();
    view = glm::lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(70.0f), 1.0f, 0.10f, 100.0f);
    
    prog.setUniform("lights[0].Position", view * glm::vec4(2.5f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("lights[1].Position", view * glm::vec4(-1.25f, 1.0f, 2.165f, 1.0f));
    prog.setUniform("lights[2].Position", view * glm::vec4(-1.25f, 1.0f, -2.165f, 1.0f));

    prog.setUniform("lights[3].Position", view * glm::vec4(0, 1.2f, 0, 1.0f));
    topLightPos = glm::vec4(0, 1.2f, 0, 1.0f);
    prog.setUniform("lights[4].Position", view * glm::vec4(0.0f, 5.0f, 0.0f, 1.0f));

    prog.setUniform("lights[0].L", lightL[0]);
    prog.setUniform("lights[0].La", lightLa[0]);
   
    prog.setUniform("lights[1].L", lightL[1]);
    prog.setUniform("lights[1].La", lightLa[1]);
   
    prog.setUniform("lights[2].L", lightL[2]);
    prog.setUniform("lights[2].La", lightLa[2]);
    
    prog.setUniform("lights[3].L", lightL[3]);
    prog.setUniform("lights[3].La", lightLa[3]);
    
    prog.setUniform("lights[4].L", glm::vec3(0.0f,0.0f,0.0f));
    prog.setUniform("lights[4].La", glm::vec3(0.07f,0.09f,0.15f));

    statueTexID = Texture::loadTexture("media/Gold.jpg");
    statueNormID = Texture::loadTexture("media/Gold_NormalMap.jpg");
    blankMaskID = Texture::loadTexture("media/BlankMask.png");
    graffitiID = Texture::loadTexture("media/Graffiti.png");

    floorTexID = Texture::loadTexture("media/Floor.jpg");
    floorNormID = Texture::loadTexture("media/Floor_NormalMap.jpg");
    mossTexID = Texture::loadTexture("media/Moss.png");
    puddleMaskID = Texture::loadTexture("media/PuddleMask.png");
    

    prog.setUniform("Tex1", 0);
    prog.setUniform("NormalMapTex", 1); 
    prog.setUniform("Tex2", 2);
    prog.setUniform("puddleMask", 3);

    prog.setUniform("Spot.L", lightL[4]);
    prog.setUniform("Spot.La", lightLa[4]);
    prog.setUniform("Spot.Exponent", 10.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(30.0f));

    graffitiProg.setUniform("Tex1", 0);
    graffitiProg.setUniform("Spot.L", lightL[4]);
    graffitiProg.setUniform("Spot.La", lightLa[4]);
    graffitiProg.setUniform("Spot.Exponent", 25.0f);
    graffitiProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    angle = glm::radians(90.0f);
    cubeTexID = Texture::loadCubeMap("media/sky/sky");



    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, statueTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, statueNormID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, statueTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blankMaskID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, floorTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, floorNormID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, mossTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, puddleMaskID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, graffitiID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();

        graffitiProg.compileShader("shader/graffiti.vert");
        graffitiProg.compileShader("shader/graffiti.frag");
        graffitiProg.link();

        wireFrameProg.compileShader("shader/wireframe.vert");
        wireFrameProg.compileShader("shader/wireframe.gs");
        wireFrameProg.compileShader("shader/wireframe.frag");
        wireFrameProg.link();

        GLint loc = glGetUniformLocation(graffitiProg.getHandle(), "ModelViewMatrix");
        std::cout << "ModelViewMatrix location = " << loc << std::endl;
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    if (animating()) {
        lightAngle += 1.0f * deltaTime;

        topLightPos.x = 0.0f + 2.5f * cosf(lightAngle);
        topLightPos.z = 0.0f + 2.5f * sinf(lightAngle);
        topLightPos.y = 1.0f;
    }
    
}

void SceneBasic_Uniform::render()
{
    if (!wireframe) {


        prog.use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::lookAt(position, position + front, up);

        glm::vec4 spotPosView = view * glm::vec4(position, 1.0f);
        glm::vec3 spotDirView = glm::mat3(view) * front;

        prog.setUniform("Spot.Position", spotPosView);
        prog.setUniform("Spot.Direction", glm::normalize(spotDirView));

        prog.setUniform("lights[3].Position", view * topLightPos);

        prog.setUniform("Material.Kd", glm::vec3(0.8f, 0.65f, 0.2f));
        prog.setUniform("Material.Ka", glm::vec3(0.1f, 0.07f, 0.02f));
        prog.setUniform("Material.Ks", glm::vec3(1.0f, 0.85f, 0.4f));
        prog.setUniform("Material.Shininess", 160.0f);

        prog.setUniform("Fog.color", glm::vec3(0.5f, 0.5f, 0.5f));
        prog.setUniform("Fog.density", 0.04f);
        prog.setUniform("Fog.start", 0.5f);
        prog.setUniform("Fog.end", 2.0f);
        prog.setUniform("Fog.enabled", fog);


        for (int i = 0; i < 4; i++) {
            if (toggles[i] == 0) {
                std::stringstream nameL;
                nameL << "lights[" << i << "].L";
                prog.setUniform(nameL.str().c_str(), glm::vec3(0.0f, 0.0f, 0.0f));
                std::stringstream nameLa;
                nameLa << "lights[" << i << "].La";
                prog.setUniform(nameLa.str().c_str(), glm::vec3(0.0f, 0.0f, 0.0f));
            }
            else {
                std::stringstream nameL;
                nameL << "lights[" << i << "].L";
                prog.setUniform(nameL.str().c_str(), lightL[i]);
                std::stringstream nameLa;
                nameLa << "lights[" << i << "].La";
                prog.setUniform(nameLa.str().c_str(), lightLa[i]);
            }
        }
        if (toggles[4] == 0) {
            prog.setUniform("Spot.L", glm::vec3(0.0f, 0.0f, 0.0f));
            prog.setUniform("Spot.La", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else {
            prog.setUniform("Spot.L", lightL[4]);
            prog.setUniform("Spot.La", lightLa[4]);
        }

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 0.3f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        setMatrices();


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, statueTexID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, statueNormID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, statueTexID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blankMaskID);

        mesh->render();

        prog.setUniform("Material.Kd", glm::vec3(0.5f, 0.5f, 0.5f));
        prog.setUniform("Material.Ka", glm::vec3(0.1f, 0.07f, 0.02f));
        prog.setUniform("Material.Ks", glm::vec3(0.1f, 0.1f, 0.1f));
        prog.setUniform("Material.Shininess", 50.0f);

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

        setMatrices();


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorNormID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mossTexID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, puddleMaskID);

        plane.render();

        //graffiti 
        graffitiProg.use();
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 0.3f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.001f));

        graffitiProg.setUniform("Material.Kd", glm::vec3(1.0f, 1.0f, 1.0f));
        graffitiProg.setUniform("Material.Ka", glm::vec3(0.03f, 0.03f, 0.03f));
        graffitiProg.setUniform("Material.Ks", glm::vec3(0.3f, 0.3f, 0.3f));
        graffitiProg.setUniform("Material.Shininess", 64.0f);


        if (toggles[4] == 0) {
            graffitiProg.setUniform("Spot.L", glm::vec3(0.0f, 0.0f, 0.0f));
            graffitiProg.setUniform("Spot.La", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else {
            graffitiProg.setUniform("Spot.L", lightL[4]);
            graffitiProg.setUniform("Spot.La", lightLa[4]);
        }

        spotPosView = view * glm::vec4(position, 1.0f);
        spotDirView = glm::mat3(view) * front;
        graffitiProg.setUniform("Spot.Position", spotPosView);
        graffitiProg.setUniform("Spot.Direction", glm::normalize(spotDirView));
        graffitiProg.setUniform("Spot.Exponent", 25.0f);
        graffitiProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

        mat4 mv = view * model;
        graffitiProg.setUniform("ModelViewMatrix", mv);
        graffitiProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
        graffitiProg.setUniform("MVP", projection * mv);

        if (graffitiID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, graffitiID);

        }


        mesh->render();

        //skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxProg.use();

        model = mat4(1.0f);
        mat4 skyBoxView = mat4(glm::mat3(view));
        mat4 skyBoxmv = skyBoxView * model;

        skyboxProg.setUniform("ModelViewMatrix", skyBoxmv);
        skyboxProg.setUniform("NormalMatrix", glm::mat3(vec3(skyBoxmv[0]), vec3(skyBoxmv[1]), vec3(skyBoxmv[2])));
        skyboxProg.setUniform("MVP", projection * skyBoxmv);

        skyboxProg.setUniform("Fog.color", glm::vec3(0.5f, 0.5f, 0.5f));
        skyboxProg.setUniform("Fog.density", 0.04f);
        skyboxProg.setUniform("Fog.start", 0.5f);
        skyboxProg.setUniform("Fog.end", 2.0f);
        skyboxProg.setUniform("Fog.enabled", fog);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexID);

        sky.render();

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

    }
    else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float w2 = width / 2.0f;
        float h2 = height / 2.0f;

        viewport = mat4(glm::vec4(w2, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, h2, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(w2, h2, 0.0f, 1.0f));


        wireFrameProg.use();
        wireFrameProg.setUniform("Line.Width", 0.1f);
        wireFrameProg.setUniform("Line.Color", glm::vec4(0.9f,0.2f,1.0f,1.0f));
        view = glm::lookAt(position, position + front, up);
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 0.3f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));

        mat4 mv = view * model;
        wireFrameProg.setUniform("ModelViewMatrix", mv);
        wireFrameProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
        wireFrameProg.setUniform("MVP", projection* mv);
        wireFrameProg.setUniform("ViewportMatrix", viewport);

        mesh->render();

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
        mv = view * model;
        wireFrameProg.setUniform("ModelViewMatrix", mv);
        wireFrameProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
        wireFrameProg.setUniform("MVP", projection* mv);
        wireFrameProg.setUniform("ViewportMatrix", viewport);

        plane.render();
    }

    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    glFinish();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    

}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv =view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP",  projection* mv);

}

void SceneBasic_Uniform::toggleLight(int index) {
    if (index >= 0 && index < 5) {
        toggles[index] = !toggles[index];
    }
}
void SceneBasic_Uniform::handleInput(int key) {
    float vel = 0.5f * deltaTime;

    if (key == GLFW_KEY_W)
        position += front * vel;
    if (key == GLFW_KEY_S)
        position -= front * vel;
    if (key == GLFW_KEY_A)
        position -= glm::normalize(glm::cross(front, up)) * vel;
    if (key == GLFW_KEY_D)
        position += glm::normalize(glm::cross(front, up)) * vel;

    
}
void SceneBasic_Uniform::handleMouseInput(double mouseX, double mouseY) {
    if (firstMouse) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    double offsetX = mouseX - lastX;
    double offsetY = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    yaw += offsetX * sensitivity * deltaTime;
    pitch += offsetY * sensitivity * deltaTime;

    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
}
void SceneBasic_Uniform::toggleFog() {
    if (fog == 0.0f) {
        fog = 1.0f;
        return;
    }
    else if(fog == 1.0f) {
        fog = 0.0f;
    }
}

void SceneBasic_Uniform::toggleWireFrame() {
    wireframe = !wireframe;
}

void SceneBasic_Uniform::setupFBO() {
    GLuint depthBuf;

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &hdrTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTex, 0);

    GLenum drawBuffers[] = { GL_NONE,GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(2, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
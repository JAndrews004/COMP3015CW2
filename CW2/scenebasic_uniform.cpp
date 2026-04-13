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

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f,10.0f,100,100) ,angle(0.0f),tPrev(0.0f),rotSpeed(glm::pi<float>()/8.0f), sky(100.0f),textQuad(1600,800),
            time(0),particleLifetime(8.5f), nParticles(800),emitterPos(1,0,0),emitterDir(0,3,-1), wall(10.0f,5.0f,100,50)
{
    mesh = ObjMesh::load("media/Statue.obj", true,true); 
    plinth = ObjMesh::load("media/Plinth.obj", true, false);
    buttonObj = ObjMesh::load("media/Button.obj", true, false);
    gameManager = new GameManager();
    buttons = { Button(glm::vec3(-3.0f,0.0f,4.5f),0.5f),Button(glm::vec3(-1.0f,0.0f,4.5f),0.5f),Button(glm::vec3(1.0f,0.0f,4.5f),0.5f) ,Button(glm::vec3(3.0f,0.0f,4.5f),0.5f) };
    
}



void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    setupFBO();

   

    view = glm::lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(70.0f), 1.0f, 0.10f, 100.0f);
    
    prog.setUniform("lights[0].Position", view * glm::vec4(-3.0f, 0.5f, 4.5f, 1.0f));
    prog.setUniform("lights[1].Position", view * glm::vec4(-1.0f, 0.5f, 4.5f, 1.0f));
    prog.setUniform("lights[2].Position", view * glm::vec4(1.0f, 0.5f, 4.5f, 1.0f));
    //prog.setUniform("lights[3].Position", view * glm::vec4(0.0f, 5.0f, 0.0f, 1.0f));

    prog.setUniform("Spot[1].Position", view * glm::vec4(0, 1.2f, 0, 1.0f));
    topLightPos = glm::vec4(0, 1.2f, 0, 1.0f);
    prog.setUniform("lights[4].Position", view * glm::vec4(0.0f, 5.0f, 0.0f, 1.0f));

    prog.setUniform("lights[0].L", lightL[0]);
    prog.setUniform("lights[0].La", lightLa[0]);
   
    prog.setUniform("lights[1].L", lightL[1]);
    prog.setUniform("lights[1].La", lightLa[1]);
   
    prog.setUniform("lights[2].L", lightL[2]);
    prog.setUniform("lights[2].La", lightLa[2]);
    
    prog.setUniform("Spot[1].L", lightL[3]);
    prog.setUniform("Spot[1].La", lightLa[3]);
    prog.setUniform("Spot[1].Exponent", 1.0f);
    prog.setUniform("Spot[1].Cutoff", glm::radians(30.0f));
    
    prog.setUniform("lights[3].L", glm::vec3(0.0f,0.0f,0.0f));
    prog.setUniform("lights[3].La", glm::vec3(0.0f,0.0f,0.0f));

    statueTexID = Texture::loadTexture("media/Gold.jpg");
    statueNormID = Texture::loadTexture("media/Gold_NormalMap.jpg");
    blankMaskID = Texture::loadTexture("media/BlankMask.png");
    graffitiID = Texture::loadTexture("media/Graffiti.png");

    floorTexID = Texture::loadTexture("media/Floor.jpg");
    floorNormID = Texture::loadTexture("media/Floor_NormalMap.jpg");
    mossTexID = Texture::loadTexture("media/Moss.png");
    puddleMaskID = Texture::loadTexture("media/PuddleMask.png");
    
    plinthTex = Texture::loadTexture("media/PlinthTex.png");
    buttonTex = Texture::loadTexture("media/ButtonTex.png");

    wallTexID = Texture::loadTexture("media/bricks.jpg");
    wallNormID = Texture::loadTexture("media/bricks_norm.png");

    prog.setUniform("Tex1", 0);
    prog.setUniform("NormalMapTex", 1); 
    prog.setUniform("Tex2", 2);
    prog.setUniform("puddleMask", 3);

    //prog.setUniform("Spot.L", lightL[4]);
    //prog.setUniform("Spot.La", lightLa[4]);
    prog.setUniform("Spot[0].Exponent", 10.0f);
    prog.setUniform("Spot[0].Cutoff", glm::radians(30.0f));

    /*
    graffitiProg.setUniform("Tex1", 0);
    graffitiProg.setUniform("Spot.L", lightL[4]);
    graffitiProg.setUniform("Spot.La", lightLa[4]);
    graffitiProg.setUniform("Spot.Exponent", 25.0f);
    graffitiProg.setUniform("Spot.Cutoff", glm::radians(30.0f));*/

    angle = glm::radians(90.0f);
    cubeTexID = Texture::loadCubeMap("media/sky/sky");

    //particle fountain
    emitterAngle = glm::half_pi<float>();
    initBuffers();
    
    particleTexID = Texture::loadTexture("media/bluewater.png");

    particleShader.use();
    particleShader.setUniform("ParticleTex",0);
    particleShader.setUniform("ParticleLifetime",particleLifetime);
    particleShader.setUniform("ParticleSize",0.01f);
    particleShader.setUniform("Gravity",glm::vec3(0.0f,-0.2f,0.0f));
    particleShader.setUniform("EmitterPos", emitterPos);

    flatProg.use();
    flatProg.setUniform("Colour", glm::vec4(0.2f, 0.2f, 0.8f, 1.0f));

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

    glBindTexture(GL_TEXTURE_2D, plinthTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, buttonTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, wallTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallNormID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    

    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR: Could not initialize FreeType" << std::endl;
    }
    else {
        std::cout << "FreeType initialized successfully!" << std::endl;
    }
    if (FT_New_Face(ft, "C:/Windows/Fonts/Arial.ttf", 0, &face)) {
        std::cout << "ERROR: Could not load font" << std::endl;
    }
    else {
        std::cout << "Font loaded successfully!" << std::endl;
    }
    FT_Set_Pixel_Sizes(face, 0, 72);


    glGenTextures(1, &textTex);
    glBindTexture(GL_TEXTURE_2D, textTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED, face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    float xpos = 100; // screen X
    float ypos = 100; // screen Y
    float w = face->glyph->bitmap.width;
    float h = face->glyph->bitmap.rows;

    
}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
        prog.printActiveUniforms();
    }
    catch (GLSLProgramException& e) {
  
        std::cerr << "[ERROR] Basic Uniform Shader: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    try {
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();
        skyboxProg.printActiveUniforms();
    }
        catch (GLSLProgramException& e) {
            std::cerr << "[ERROR] Skybox Shader: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            graffitiProg.compileShader("shader/graffiti.vert");
            graffitiProg.compileShader("shader/graffiti.frag");
            graffitiProg.link();
            graffitiProg.printActiveUniforms();
        }
        catch (GLSLProgramException& e) {
            std::cerr << "[ERROR] Graffiti Shader: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            wireFrameProg.compileShader("shader/wireframe.vert");
            wireFrameProg.compileShader("shader/wireframe.gs");
            wireFrameProg.compileShader("shader/wireframe.frag");
            wireFrameProg.link();
            wireFrameProg.printActiveUniforms();
        }
        catch (GLSLProgramException& e) {
            std::cerr << "[ERROR] Wireframe Shader: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            textShader.compileShader("shader/text.vert");
            textShader.compileShader("shader/text.frag");
            textShader.link();
            textShader.printActiveUniforms();
        }
        catch (GLSLProgramException& e) {
            std::cerr << "[ERROR] Text Shader: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            particleShader.compileShader("shader/particleFountain.vert");
            particleShader.compileShader("shader/particleFountain.frag");
            particleShader.link();
            particleShader.printActiveUniforms();
        }
        catch (GLSLProgramException& e) {
            std::cerr << "[ERROR] Particle Shader: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            flatProg.compileShader("shader/flat_frag.glsl");
            flatProg.compileShader("shader/flat_vert.glsl");
            flatProg.link();
            flatProg.printActiveUniforms();
        }
        
	catch (GLSLProgramException &e) {
        std::cerr << "[ERROR] Flat Shader: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
    try  {
        depthProg.compileShader("shader/shadow.vert");
        depthProg.compileShader("shader/shadow.frag");
        depthProg.link();
    }
    catch (GLSLProgramException& e) {
        std::cerr << "[ERROR] Shadow Shader: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

}

void SceneBasic_Uniform::update( float t )
{
    time = t;
    emitterAngle = std::fmod(emitterAngle + 0.01f, glm::two_pi<float>());
    buttonProximity = false;
    if (animating()) {
        lightAngle += 1.0f * deltaTime;

        topLightPos.x = 0.0f + 0.0f * cosf(lightAngle);
        topLightPos.z = 0.0f + 0.0f * sinf(lightAngle);
        topLightPos.y = 3.5f;
    }
    
    for (int i = 0; i < buttons.size();i++) {
        if (buttons[i].proximityCheck(position)) {
            buttonProximity = true;
        }
    }
    gameManager->puzzleCheck();

    if (gameManager->state == Solved) {
        std::cout << "Puzzle Solved!" << std::endl;
    }
}

void SceneBasic_Uniform::render()
{
    if (!wireframe) {
        //shadows
        prog.use();
        float sceneHalfSize = 1.5f; // slightly bigger than your AABB
        glm::mat4 lightProjection = glm::ortho(
            -sceneHalfSize, sceneHalfSize,
            -sceneHalfSize, sceneHalfSize,
            0.1f, 5.0f
        );
        glm::mat4 lightView = glm::lookAt(
            glm::vec3(topLightPos),
            glm::vec3(topLightPos) + glm::vec3(0,-1,0),
            glm::vec3(0, 0, 1)
        );

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        prog.setUniform("LightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        depthProg.use();
        depthProg.setUniform("LightSpaceMatrix", lightSpaceMatrix);
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 0.3f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        depthProg.setUniform("ModelMatrix", model);
        mesh->render();

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
        depthProg.setUniform("ModelMatrix", model);
        plane.render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);

        ///////

        prog.use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, shadowTex);

        prog.setUniform("shadowMap", 4);
        prog.setUniform("LightSpaceMatrix", lightSpaceMatrix);
        prog.setUniform("normScale", 2.0f);
        view = glm::lookAt(position, position + front, up);

        glm::vec4 spotPosView = view * glm::vec4(position, 1.0f);
        glm::vec3 spotDirView = glm::mat3(view) * front;

        prog.setUniform("Spot[0].Position", spotPosView);
        prog.setUniform("Spot[0].Direction", glm::normalize(spotDirView));

        prog.setUniform("Spot[1].Position", view * topLightPos);
        prog.setUniform("Spot[1].Direction", glm::mat3(view)*glm::vec3(0,-1,0));

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
            if (i == 3) {
                if (toggles[i] == 0) {
                    prog.setUniform("Spot[1].L", glm::vec3(0.0f));
                    prog.setUniform("Spot[1].La", glm::vec3(0.0f));
                }
                else {
                    prog.setUniform("Spot[1].L", lightL[3]);
                    prog.setUniform("Spot[1].La", lightLa[3]);
                }
                break;
            }
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
            prog.setUniform("Spot[0].L", glm::vec3(0.0f, 0.0f, 0.0f));
            prog.setUniform("Spot[0].La", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else {
            prog.setUniform("Spot[0].L", lightL[4]);
            prog.setUniform("Spot[0].La", lightLa[4]);
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

        ///wall///

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 0.0f, 5.0f));
        model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));

        prog.setUniform("normScale", 8.0f);

        setMatrices();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wallNormID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, wallTexID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blankMaskID);

        prog.setUniform("Material.Kd", glm::vec3(0.5f, 0.5f, 0.5f));
        prog.setUniform("Material.Ka", glm::vec3(0.1f, 0.07f, 0.02f));
        prog.setUniform("Material.Ks", glm::vec3(0.1f, 0.1f, 0.1f));
        prog.setUniform("Material.Shininess", 50.0f);

        wall.render();

        ///plinth and button///
        for (int i = 0; i < 4; i++) {
            model = mat4(1.0f);
            model = glm::translate(model, vec3(-5.0f + 2.0f * (i + 1), 0.08f, 4.5f));
            model = glm::scale(model, glm::vec3(25.0f));
            setMatrices();
            prog.setUniform("Material.Kd", glm::vec3(0.5f, 0.5f, 0.5f));
            prog.setUniform("Material.Ka", glm::vec3(0.1f, 0.07f, 0.02f));
            prog.setUniform("Material.Ks", glm::vec3(0.1f, 0.1f, 0.1f));
            prog.setUniform("Material.Shininess", 50.0f);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, plinthTex);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, floorNormID);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, plinthTex);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, blankMaskID);

            plinth->render();

            model = mat4(1.0f);
            model = glm::translate(model, vec3(-5.0f + 2.0f * (i+1), 0.7f, 4.5f));
            model = glm::scale(model, glm::vec3(0.04f));
            setMatrices();
            prog.setUniform("Material.Kd", glm::vec3(0.5f, 0.5f, 0.5f));
            prog.setUniform("Material.Ka", glm::vec3(0.1f, 0.07f, 0.02f));
            prog.setUniform("Material.Ks", glm::vec3(0.4f, 0.4f, 0.4f));
            prog.setUniform("Material.Shininess", 280.0f);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buttonTex);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, floorNormID);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, buttonTex);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, blankMaskID);

            buttonObj->render();
        }
        

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

    // --- switch to orthographic projection for 2D text (Core OpenGL) ---
    glm::mat4 orthoProj = glm::ortho(0.0f, float(width), 0.0f, float(height));

    // Text model matrix: position your text
    glm::mat4 textModel = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 0.0f));
    glm::mat4 textMVP = orthoProj * textModel;


    //Text UI
    if (buttonProximity) {
        glDisable(GL_DEPTH_TEST);
        renderText("E", 775.0f, 50.0f, 0.002f);
        textQuad.render();
        glEnable(GL_DEPTH_TEST);
    }


    //check if puzzle is solved
    gameManager->puzzleCheck();
    if (gameManager->state == Solved) {
        //particle fountain
        flatProg.use();
        model = mat4(1.0f);

        mat4 mv = view * model;
        flatProg.setUniform("ProjectionMatrix", projection);
        flatProg.setUniform("ModelViewMatrix", mv);

        glDepthMask(GL_FALSE);
        //glDisable(GL_DEPTH_TEST);
        particleShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTexID);

        particleShader.setUniform("ProjectionMatrix", projection);
        particleShader.setUniform("ModelViewMatrix", mv);
        particleShader.setUniform("Time", time);

        glBindVertexArray(particles);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
        glBindVertexArray(0);

    }

    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


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

    textQuad = TextQuad(w, h);

}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv =view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP",  projection* mv);
    prog.setUniform("ModelMatrix", model);

}

void SceneBasic_Uniform::toggleLight(int index) {
    if (index >= 0 && index < 5) {
        toggles[index] = !toggles[index];

        if (index <= 3) {
            gameManager->lightToggles[index] = toggles[index];
        }
    }
}
void SceneBasic_Uniform::checkButtonInput()
{
    for (int i = 0; i < buttons.size();i++) {
        if (buttons[i].proximityCheck(position)) {
            toggleLight(i);
        }
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
    
    GLfloat border[] = { 1.0f,0.0f,0.0f,0.0f };

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, SHADOW_SIZE, SHADOW_SIZE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete." << std::endl;

    }
    else {
        cerr << "Framebuffer not complete." << std::endl;
    }

    


}

void SceneBasic_Uniform::initBuffers()
{
    glGenBuffers(1, &initVel);
    glGenBuffers(1, &startTime);

    int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);
    for (uint32_t i = 0; i < nParticles;i++) {

        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        velocity = glm::mix(1.25f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v)*velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;

    }
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());

    float rate = particleLifetime / nParticles;
    for (int i = 0;i < nParticles;i++) {
        data[i] = rate * i;
    }
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

}

float SceneBasic_Uniform::randFloat()
{
    return rand.nextFloat();
}

void SceneBasic_Uniform::renderText(const std::string& text, float x, float y, float scale)
{
    glUseProgram(textShader.getHandle());
    glm::mat4 orthoProj = glm::ortho(0.0f, float(width), 0.0f, float(height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textTex);

    for (char c : text) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph " << c << "\n";
            continue;
        }


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        float xpos = x + face->glyph->bitmap_left * scale;
        float ypos = y - (face->glyph->bitmap.rows - face->glyph->bitmap_top) * scale;
        float w = face->glyph->bitmap.width * scale;
        float h = face->glyph->bitmap.rows * scale;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));
        glm::mat4 mvp = orthoProj * model;
        textShader.setUniform("MVP", mvp);
        textShader.setUniform("textColor", glm::vec3(1.0f, 0.0f, 0.0f));
        textQuad.render();

        x += (face->glyph->advance.x >> 6) * scale;
    }
}
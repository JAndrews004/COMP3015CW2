#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "GameManager.cpp"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/random.h"
#include "helper/particleutils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct TextQuad {
    GLuint VAO, VBO;

    TextQuad(float w, float h) {
        float vertices[] = {
            // pos(x,y,z)       // tex(u,v)
            0.0f, 0.0f, 0.0f,  0.0f, 1.0f, // BL
            w,    0.0f, 0.0f,  1.0f, 1.0f, // BR
            0.0f, h,    0.0f,  0.0f, 0.0f, // TL

            w,    0.0f, 0.0f,  1.0f, 1.0f, // BR
            w,    h,    0.0f,  1.0f, 0.0f, // TR
            0.0f, h,    0.0f,  0.0f, 0.0f  // TL
        };


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
};

struct Button {
    glm::vec3 position;
    float triggerDistance;

    Button(glm::vec3 pos, float dist) {
        position = pos;
        triggerDistance = dist;
    }

    bool proximityCheck(glm::vec3 playerPos) {
        
        float distance = (abs(position.x - playerPos.x) * abs(position.x - playerPos.x) + abs(position.z - playerPos.z) * abs(position.z - playerPos.z));
        distance = sqrtf(distance);

        if (distance <= triggerDistance) {
            return true;
        }

        return false;

    }
};

class SceneBasic_Uniform : public Scene
{
private:
    GameManager* gameManager;

    FT_Library ft;
    FT_Face face;
    GLuint textTex;
    TextQuad textQuad;

    GLSLProgram prog;
    GLSLProgram skyboxProg;
    GLSLProgram graffitiProg;
    GLSLProgram wireFrameProg;
    GLSLProgram textShader;
    GLSLProgram particleShader,flatProg;
    glm::mat4 viewport;

    Random rand;

    GLuint initVel, startTime, particles, nParticles;
    glm::vec3 emitterPos, emitterDir;
    float emitterAngle, time, particleLifetime;

    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    GLuint programHandle;
    GLuint location;

    GLuint hdrTex, avgTex;
    GLuint hdrFBO;

    SkyBox sky;
    GLuint cubeTexID;
    float angle, tPrev, rotSpeed;

    float fog = 0.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float lastTime = 0.0f;
    float deltaTime;

    float pitch = 0.0f;
    float yaw = -90.0f;
    float sensitivity = 90.0f;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0, 1, 0);
    double lastX = width / 2.0;
    double lastY = height / 2.0;
    bool firstMouse = true;

    bool wireframe = false;
    glm::vec4 topLightPos;
    float lightAngle =0;
    bool movingPositive = true;
    int toggles[5] = { 1, 1, 1, 1 ,0 };
    glm::vec3 lightL[5] = { glm::vec3(0.0f, 0.7f, 0.9f),glm::vec3(0.9f, 0.1f, 0.8f),glm::vec3(0.7f, 0.9f, 0.1f),glm::vec3(0.6f, 0.6f, 0.9f) ,glm::vec3(0.6f, 0.0f, 0.9f) };

    glm::vec3 lightLa[5] = { glm::vec3(0.3f, 0.3f, 0.3f),glm::vec3(0.3f, 0.3f, 0.3f),glm::vec3(0.3f, 0.3f, 0.3f),glm::vec3(0.4f, 0.4f, 0.4f) , glm::vec3(0.1f, 0.1f, 0.1f)};

    GLuint statueTexID, statueNormID, blankMaskID, graffitiID;
    GLuint floorTexID, mossTexID, floorNormID, puddleMaskID;
    GLuint particleTexID;

    bool buttonProximity = true;
    std::vector<Button> buttons = { Button(glm::vec3(0.0f),0.5f),Button(glm::vec3(1.0f),0.5f),Button(glm::vec3(-1.0f),0.5f) ,Button(glm::vec3(1.0f,0.0f,-1.0f),0.5f) };

    void compile();
    void setMatrices();
    void setupFBO();
    void initBuffers();
    float randFloat();
    void renderText(const std::string& text, float x, float y, float scale);
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void toggleLight(int index) override;
    void checkButtonInput() override;
    void handleInput(int key) override;
    void handleMouseInput(double mouseX, double mouseY) override;
    void toggleFog() override;
    void toggleWireFrame() override;

    
};

#endif // SCENEBASIC_UNIFORM_H

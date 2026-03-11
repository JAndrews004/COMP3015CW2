#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    GLSLProgram skyboxProg;
    GLSLProgram graffitiProg;
    GLSLProgram wireFrameProg;
    glm::mat4 viewport;


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

    void compile();
    void setMatrices();
    void setupFBO();
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void toggleLight(int index) override;
    void handleInput(int key) override;
    void handleMouseInput(double mouseX, double mouseY) override;
    void toggleFog() override;
    void toggleWireFrame() override;
};

#endif // SCENEBASIC_UNIFORM_H

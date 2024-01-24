#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "shadow_map.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
    void DispatchImGui();
    void DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;

    MeshUPtr m_box;

    MaterialPtr m_planeMaterial;
    MaterialPtr m_box1Material;
    MaterialPtr m_box2Material;

    //shadow map
    ShadowMapUPtr m_shadowMap;
    ProgramUPtr m_lightingShadowProgram;


    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};

    // animation
    bool m_animation { true };

    bool m_blinn { true };
    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    struct Light {
        glm::vec3 position { glm::vec3(0.330f, 2.240f, -1.10f) };
        glm::vec3 direction { glm::vec3(-0.02f, -1.0f, 0.38f) };
        glm::vec2 cutoff { glm::vec2(36.0f, 5.0f) };
        float distance { 256.0f };
        glm::vec3 ambient { glm::vec3(1.0f,  1.0f, 1.0f) };
        glm::vec3 diffuse { glm::vec3(1.0f,  1.0f, 1.0f) };
        glm::vec3 specular { glm::vec3(0.0f, 1.0f, 0.0f) };
    };
    Light m_light;


    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { -63.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(-0.63f, 13.5f, 8.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
    

};

#endif // __CONTEXT_H__
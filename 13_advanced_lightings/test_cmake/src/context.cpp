#include "context.h"
#include "image.h"
#include "framebuffer.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::DispatchImGui() {
    if (ImGui::Begin("ui window")) {
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::Checkbox("l.blinn", &m_blinn);    

            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.1f, 0.0f, 1000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
        }

        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat("m.shininess", &m_planeMaterial->shininess, 1.0f, 1.0f, 256.0f);
        }

        ImGui::Checkbox("animation", &m_animation);

        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(),
                    ImVec2(256, 256), 
                    ImVec2(0, 1), 
                    ImVec2(1, 0));
    }
    ImGui::End();
}

void Context::DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program) 
{

    program->Use();
    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(program);
    m_box->Draw(program);

    // modelTransform =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // transform = projection * view * modelTransform;
    // program->SetUniform("transform", transform);
    // program->SetUniform("modelTransform", modelTransform);
    // m_box1Material->SetToProgram(program);
    // m_box->Draw(program);

    // modelTransform =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // transform = projection * view * modelTransform;
    // program->SetUniform("transform", transform);
    // program->SetUniform("modelTransform", modelTransform);
    // m_box2Material->SetToProgram(program);
    // m_box->Draw(program);

    // modelTransform =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // transform = projection * view * modelTransform;
    // program->SetUniform("transform", transform);
    // program->SetUniform("modelTransform", modelTransform);
    // m_box2Material->SetToProgram(program);
    // m_box->Draw(program);
}


void Context::Render() {


    DispatchImGui();


    auto lightView = glm::lookAt(m_light.position, 
                                 m_light.position + m_light.direction, 
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    auto lightProjection = glm::perspective(
                                glm::radians((m_light.cutoff[0] + m_light.cutoff[1]) * 2.0f),
                                1.0f, 1.0f, 20.0f);

    m_shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 
                0,
                m_shadowMap->GetShadowMap()->GetWidth(),
                m_shadowMap->GetShadowMap()->GetHeight());
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawScene(lightView, lightProjection, m_simpleProgram.get());

    Framebuffer::BindToDefault();
    glViewport(0, 0, m_width, m_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height, 0.01f, 100.0f);

    auto view = glm::lookAt(
        m_cameraPos,
        m_cameraPos + m_cameraFront,
        m_cameraUp);


    auto lightModelTransform =
        glm::translate(glm::mat4(1.0), m_light.position) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
    m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    m_box->Draw(m_simpleProgram.get());

    m_lightingShadowProgram->Use();
    m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
    m_lightingShadowProgram->SetUniform("light.position", m_light.position);
    m_lightingShadowProgram->SetUniform("light.direction", m_light.direction);
    m_lightingShadowProgram->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_lightingShadowProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_lightingShadowProgram->SetUniform("light.ambient", m_light.ambient);
    m_lightingShadowProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_lightingShadowProgram->SetUniform("light.specular", m_light.specular);
    m_lightingShadowProgram->SetUniform("blinn", (m_blinn ? 1 : 0));
    m_lightingShadowProgram->SetUniform("lightTransform", lightProjection * lightView);

    glActiveTexture(GL_TEXTURE3);
    m_shadowMap->GetShadowMap()->Bind();
    m_lightingShadowProgram->SetUniform("shadowMap", 3);
    
    glActiveTexture(GL_TEXTURE0);

    DrawScene(view, projection, m_lightingShadowProgram.get());

}

bool Context::Init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_box = Mesh::CreateBox();

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;

    m_lightingShadowProgram = Program::Create("./shader/lighting_shadow.vs", "./shader/lighting_shadow.fs");
    if (!m_lightingShadowProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());


    TexturePtr darkGrayTexture = Texture::CreateFromImage(
                                    Image::CreateSingleColorImage(4, 4,
                                    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
                                Image::CreateSingleColorImage(4, 4,
                                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
        
    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(
                                Image::Load("./image/wood.png").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 256.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(
                                Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(
                                Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(
                                Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;
 

    m_shadowMap = ShadowMap::Create(1024, 1024);

    return true;
}
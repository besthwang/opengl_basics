## shadow mapp

- 가장 많이 사용되는 그림자 렌더링 알고리즘
- 구현 난이도가 어렵지 안은 편이면서 성능을 크게 잡아먹지 않는 편
- 고급 알고리즘으로의 확장이 편하다.
    - omnidiractional shadow map
    - cascaded shadow map

- Idea: 빛의 시점에서 렌더링을 해본다.
    - 빛이 볼 수 있는 부분은 그림자가 지지 않는다.
    - 빛이 볼 수 없는 부분은 그림자가 진다.


<img src=images/13_shadow_map_idea.png>    
    

<br><br>
  
- **알고리즘**       
    - light source의 위체에서 물체들의 depth map만 렌더링한다. (shadow map)
    - shadow map을 바탕으로 지금 그리고 있는 픽셀이 빛을 받는지 아닌지 판별한다.

<img src=images/13_shadow_map_algorithm.png>

## SHADOW MAP SCENE

-장면 준비
    - Context::DrawScene() 함수를 정의하여 장면을 그리는 그리는 부분을 분리함
    - Context::Render() 함수에서 Context::DrawScene() 함수를 호출



`context.h`

```cpp
void DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program);
```

`context.cpp`

```cpp
void Context::Render() {
  ...

    DrawScene(view, projection, m_program.get());
  ...

}
```

```cpp

//바닥1개와 박스 1개를 그림
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

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(program);
    m_box->Draw(program);

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);
}
```

<img src=images/13_shadow_map_scene.JPG>

## Texture 클래스 코드 리팩토링
- pixel type의 파라미터화
    - 현재 Texture 클래스는 모두 픽셀을 GL_UNSIGNED_BYTE로 처리
    - depth map texture를 사용하기 위해서는 GL_FLOAT 타입을 사용해야함
    - pixel type의 파라미터화

`texture.h`

```cpp
// texture.h
static TextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
// ...
uint32_t GetType() const { return m_type; }
// ...
void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
// ...
uint32_t m_type { GL_UNSIGNED_BYTE };
```

`texture.cpp`

```cpp
TextureUPtr Texture::Create(int width, int height, uint32_t format, uint32_t type) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

void Texture::SetTextureFormat(int width, int height, uint32_t format, uint32_t type) {
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;

    GLenum imageFormat = GL_RGBA;
    if (m_format == GL_DEPTH_COMPONENT) {
        imageFormat = GL_DEPTH_COMPONENT;        
    }
    else if (m_format == GL_RGB ||
        m_format == GL_RGB16F ||
        m_format == GL_RGB32F) {
        imageFormat = GL_RGB;
    }
    else if (m_format == GL_RG ||
        m_format == GL_RG16F ||
        m_format == GL_RG32F) {
        imageFormat = GL_RG;
    }
    else if (m_format == GL_RED ||
        m_format == GL_R ||
        m_format == GL_R16F ||
        m_format == GL_R32F) {
        imageFormat = GL_RED;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        imageFormat, m_type,
        nullptr);
}

```

SetTextureFormat는 
[glTexImage2D](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml)의 `type`  인자를 설정함.

* glTexImage2D의 type 인자에 대한 설명이다.

    * Specifies the data type of the pixel data. 
    * The following symbolic values are accepted: <br>
    * 적용가능한 픽셀타입이다. <br>
    GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

* depth 값을 텍스처에 장하기 위해서 `format` 인자를 `GL_DEPTH_COMPONENT` 로 설정함. <br>
    GL_DEPTH_COMPONENT: <br>
    Each element is a single depth value. The GL converts it to floating point and clamps to the range [0,1].

```cpp
void glTexImage2D(	GLenum target,
 	GLint level,
 	GLint internalformat,
 	GLsizei width,
 	GLsizei height,
 	GLint border,
 	GLenum format,
 	GLenum type,
 	const void * data);
```

## shadow_map 클래스 추가

- ShadowMap 클래스 디자인
    - **m_shdowmap:** depth map 저장을 위한 텍스처 멤버
    - **m_framebuffer:**  depth map에 렌더링을 하기위한 프레임버퍼
    - shadow map의 크기를 지정하면 해당 크기의 depth 텍스처를 만들어서 framebuffer에 바인딩하자
    - color/ stencil은 사용하지 않음


`shadow_map.h`

```cpp
#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "texture.h"

CLASS_PTR(ShadowMap);
class ShadowMap {
public:
  static ShadowMapUPtr Create(int width, int height);
  ~ShadowMap();

  const uint32_t Get() const { return m_framebuffer; }
  void Bind() const;
  const TexturePtr GetShadowMap() const { return m_shadowMap; }

private:
  ShadowMap() {}
  bool Init(int width, int height);

  uint32_t m_framebuffer { 0 };
  TexturePtr m_shadowMap;
};

#endif // __SHADOW_MAP_H__
```



- ShadowMap 클래스 구현
    - Init() 함수는 FrameBuffer(InitWithColorAttachment)와 크게 다르지 않음.
    - 여기서는 depth 텍스처를 생성하여 framebuffer에 바인딩.
    ```cpp
    //depth 텍스쳐 생성 및 바이딩
    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_REPEAT, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    ```

    - glDrawBuffer(GL_NONE), glReadBuffer(GL_NONE)을 명시적으로 호출 color attachment가 없음을 OpenGL에게 알려줌
 
    
    `framebuffer.cpp`
    ```cpp
    bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment) {
        m_colorAttachment = colorAttachment;
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                colorAttachment->Get(), 0);

        glGenRenderbuffers(1, &m_depthStencilBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
        glRenderbufferStorage(
                            GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                            colorAttachment->GetWidth(), colorAttachment->GetHeight());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(
                            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, m_depthStencilBuffer);

        auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (result != GL_FRAMEBUFFER_COMPLETE) {
            SPDLOG_ERROR("failed to create framebuffer: {}", result);
            return false;
        }

        BindToDefault();

        return true;
    }

    ```

`shadow_map.cpp`

```cpp
#include "shadow_map.h"

ShadowMapUPtr ShadowMap::Create(int width, int height) {
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(width, height))
        return nullptr;
    return std::move(shadowMap);
}

ShadowMap::~ShadowMap() {
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void ShadowMap::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height) {
    glGenFramebuffers(1, &m_framebuffer);
    Bind();

    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_REPEAT, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                        GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}
````

## SHADOW MAP -FIRST PASS

- Context 클래스에 ShadowMap 멤버 추가

`context.h`

```cpp
//shadow map
ShadowMapUPtr m_shadowMap;
```

- Context::Init()에서 ShadowMap 초기화

```cpp
m_shadowMap = ShadowMap::Create(1024, 1024);
```

- Context::Render()에서 ShadowMap 프레임버퍼에 렌더링
    - 가장 단순한 형태의 shader를 사용, depthbuffer만 채우기
    - light의 위치 및 방향을 사용하여 view/projection 행렬 계산
    - lighting shader를 이용하기 전에 그리도록 함. <br>
    
    $ aspect = \frac{width}{height} $   

`contex::Render() 안에서`

```cpp
  //aspect ratio가 1로 설정됨
  //shadowMap의 framebuffer의 크기가 1024x1024로 설정됨
  auto lightView = glm::lookAt(m_light.position, m_light.position + m_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));

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
```

- ImGui를 이용하여 그려진 shadow map을 비주얼라이징

```cpp
  ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(),
    ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
```


## SHADOW MAP - SECOND PASS

- shader/lighting_shadow.vs 추가
- vertex shader 구현 이슈
    - 대부분은 lighting.vs와 동일함.
    - 픽셀의 3D 공간 상의 위치를 light를 기준으로 계산한 fragPosLight가 추가

`lighting_shadow.vs`
```cpp
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoord;
  vec4 fragPosLight;
} vs_out;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  vs_out.fragPos = vec3(modelTransform * vec4(aPos, 1.0));
  vs_out.normal = transpose(inverse(mat3(modelTransform))) * aNormal;
  vs_out.texCoord = aTexCoord;
  vs_out.fragPosLight = lightTransform * vec4(vs_out.fragPos, 1.0);
}

```

- fragment shader 구현 이슈
    - 대부분의 코드는 lighting.fs와 동일
    - shadowMap 멤버 추가
    - ShadowCalclation()을 호출하여 그림자가 있을 경우 (1.0)    
      diffuse/specualr 컬러를 적용하지 않도록함.
    - 현재 ShadowCalculation() 부분은 미구현

`lighting_shadow.fs`
```cpp
#version 330 core

out vec4 fragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} fs_in;

uniform vec3 viewPos;
struct Light {
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;
uniform int blinn;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLight) {
  return 0.0;
}

void main() {
  vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
  vec3 ambient = texColor * light.ambient;

  float dist = length(light.position - fs_in.fragPos);
  vec3 distPoly = vec3(1.0, dist, dist*dist);
  float attenuation = 1.0 / dot(distPoly, light.attenuation);
  vec3 lightDir = (light.position - fs_in.fragPos) / dist;

  vec3 result = ambient;
  float theta = dot(lightDir, normalize(-light.direction));
  float intensity = clamp(
      (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
      0.0, 1.0);

  if (intensity > 0.0) {
    vec3 pixelNorm = normalize(fs_in.normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;

    vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
    float spec = 0.0;
    if (blinn == 0) {
        vec3 viewDir = normalize(viewPos - fs_in.fragPos);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    else {
        vec3 viewDir = normalize(viewPos - fs_in.fragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
    }
    vec3 specular = spec * specColor * light.specular;
    float shadow = ShadowCalculation(fs_in.fragPosLight);

    result += (diffuse + specular) * intensity * (1.0 - shadow);
  }

  result *= attenuation;
  fragColor = vec4(result, 1.0);
}

```

- Context 클래스에 shadow shader 멤버 추가
```cpp
// shadow map
  ShadowMapUPtr m_shadowMap;
  ProgramUPtr m_lightingShadowProgram;
```
- Context::Init() 함수에서 shader 초기화

```cpp
  m_lightingShadowProgram = Program::Create(
    "./shader/lighting_shadow.vs", "./shader/lighting_shadow.fs");
```

- Context::Render() 함수에서 기본 lighting shader 대신 lighting shadow shader 사용

```cpp
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
```

- 빌드 및 실행
    - 이전의 lighting shader와 결과가 동일한 것을 먼저 확인
    - ShadowCalculation()을 리턴값을 1.0로 변경하면 전부 어둡게 렌더링 되는 것을 확인
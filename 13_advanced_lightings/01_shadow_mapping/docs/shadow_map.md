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
- 정리된 장면
<img src=images/13_shadow_map_scene.JPG>
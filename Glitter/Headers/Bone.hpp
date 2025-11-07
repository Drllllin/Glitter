#pragma once
#include<string>
#include<vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Shader.hpp"

class Bone {
public:
    Model* model;
    glm::vec3 initialOffset;    // 模型的初始偏移
    glm::mat4 localTransform;   // 包含動畫旋轉/縮放的本地變換
    glm::mat4 globalTransform;  // 世界座標最終矩陣
    Bone* parent;
    std::vector<Bone*> children;

    Bone(Model* m, glm::vec3 offset = glm::vec3(1.0f))
        : model(m), initialOffset(offset), localTransform(1.0f), globalTransform(1.0f), parent(nullptr){}

    void addChild(Bone* child) {
        child->parent = this;
        children.push_back(child);
    }

    void resetLocalTransform() {
        // use body
        this->localTransform = 1.0f;
        for (auto* c : children)
            c->resetLocalTransform();
    }

    void updateGlobalTransform() {

        glm::mat4 localWithOffset = glm::translate(glm::mat4(1.0f), initialOffset) * localTransform;
        
        if (parent)         
            globalTransform = parent->globalTransform * glm::translate(glm::mat4(1.0f), -parent->initialOffset) * glm::translate(glm::mat4(1.0f), initialOffset) * localTransform;
        else
            globalTransform = localTransform;

        for (auto* c : children)
            c->updateGlobalTransform();
    }

    void Draw(Shader& shader) {
        if (model) {
            shader.setMat4("model", globalTransform);
            model->Draw(shader);
        }
        for (auto* c : children)
            c->Draw(shader);
    }
};

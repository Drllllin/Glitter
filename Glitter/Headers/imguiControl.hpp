#pragma once

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
class IMGUI
{
public:
	IMGUI(GLFWwindow* window) {
        // -------------------------------------------
        // ImGui
        // -------------------------------------------
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark(); // 使用暗色風格，可改成 Light

        // 初始化 ImGui 的後端（GLFW + OpenGL3）
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430 core");

        // 讓滑鼠顯示出來（ImGui 要用）
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	~IMGUI() {

	}

    void newframe() {
        // 開始新的 ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void genInterface() {
        // -------------------------------------------
       // ImGui 介面區域
       // -------------------------------------------
        ImGui::Begin("Control Board");

        //ImGui::Text("模型控制");
        //static float scale = 1.0f;
        //ImGui::SliderFloat("模型縮放", &scale, 0.1f, 3.0f);
        ////修改模型大小
        //model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(scale));
        //ourShader.setMat4("model", model);
       ImGui::SliderFloat("fffff", &x, 0.0f, 10.0f);
        ImGui::Checkbox("Polygon Mode", &PolygonMode);

        const char* items[] = { "Idle", "Walk", "Jumping Jacks", "Squat", "Sit-ups", "Push-ups",
                                "Moonwalk"};
        ImGui::Combo("Action Mode", &actionMode, items, IM_ARRAYSIZE(items));

        //light
        ImGui::Checkbox("Direct", &DirLight_checkbox);
        ImGui::Checkbox("Point", &PointLight_checkbox);
        ImGui::Checkbox("Spot", &SpotLight_checkbox);

        //// 顯示 FPS
        //ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);


        ImGui::End();


        // -------------------------------------------
        // 渲染 ImGui 畫面
        // -------------------------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void end() {
        // -------------------------------------------
        // 結束 ImGui
        // -------------------------------------------
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    bool getPolygonMode() { return PolygonMode; }


    int getActionMode() { return actionMode; }



    bool getDirLight() { return DirLight_checkbox; }
    bool getPointLight() { return PointLight_checkbox; }
    bool getSpotLight() { return SpotLight_checkbox; }
    float x = 0.4;
private:
    bool PolygonMode = false;
    int actionMode = 0; // 目前選中的 index
    bool DirLight_checkbox = true;
    bool PointLight_checkbox = true;
    bool SpotLight_checkbox = true;
};
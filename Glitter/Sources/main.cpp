#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Bone.hpp"

#include "imguiControl.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

enum ACTION {
    Idle = 0, 
    Walk = 1, 
    JumpingJacks = 2,
    Squat = 3,
    Situps = 4,
    Pushups = 5
};
// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG", NULL, NULL);
    //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //GLFWwindow* window = glfwCreateWindow(
    //    mode->width, mode->height,
    //    "My OpenGL Window",
    //    monitor, 
    //    NULL
    //);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI imgui(window);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader modelShader("D:/ROBOT/Glitter/Shaders/model_Shader.vert", "D:/ROBOT/Glitter/Shaders/model_Shader.frag");
    Shader lightCubeShader("D:/ROBOT/Glitter/Shaders/light_cube.vert", "D:/ROBOT/Glitter/Shaders/light_cube.frag");
    Shader skyboxShader("D:/ROBOT/Glitter/Shaders/skybox.vert", "D:/ROBOT/Glitter/Shaders/skybox.frag");

    // load models
    // -----------
    Model bodyModel("D:/ROBOT/Glitter/Obj/botJoint/body.obj");
    Model headModel("D:/ROBOT/Glitter/Obj/botJoint/head.obj");
    //hand
    Model hand_rightModel("D:/ROBOT/Glitter/Obj/botJoint/hand_right.obj");
    Model arm_up_rightModel("D:/ROBOT/Glitter/Obj/botJoint/arm_up_right.obj");
    Model arm_down_rightModel("D:/ROBOT/Glitter/Obj/botJoint/arm_down_right.obj");
    Model hand_leftModel("D:/ROBOT/Glitter/Obj/botJoint/hand_left.obj");
    Model arm_up_leftModel("D:/ROBOT/Glitter/Obj/botJoint/arm_up_left.obj");
    Model arm_down_leftModel("D:/ROBOT/Glitter/Obj/botJoint/arm_down_left.obj");
    //foot
    Model leg_down_leftModel("D:/ROBOT/Glitter/Obj/botJoint/leg_down_left.obj");
    Model leg_up_leftModel("D:/ROBOT/Glitter/Obj/botJoint/leg_up_left.obj");
    Model foot_leftModel("D:/ROBOT/Glitter/Obj/botJoint/foot_left.obj");

    Model leg_up_rightModel("D:/ROBOT/Glitter/Obj/botJoint/leg_up_right.obj");
    Model leg_down_rightModel("D:/ROBOT/Glitter/Obj/botJoint/leg_down_right.obj");
    Model foot_rightModel("D:/ROBOT/Glitter/Obj/botJoint/foot_right.obj");

    glm::vec3 body_offset(0.0f, 0.0f, 0.0f);
    glm::vec3 head_offset(-0.009f, 0.587f, -0.079f);

    // 右手
    glm::vec3 arm_up_right_offset(-0.255f, 0.253f, 0.005f);
    glm::vec3 arm_down_right_offset(-0.373f, 0.003f, 0.005f);
    glm::vec3 hand_right_offset(-0.486f, -0.340f, 0.004f);

    // 左手
    glm::vec3 arm_up_left_offset(0.255f, 0.253f, 0.005f);
    glm::vec3 arm_down_left_offset(0.373f, 0.003f, 0.005f);
    glm::vec3 hand_left_offset(0.486f, -0.340f, 0.004f);

    // 左腿
    glm::vec3 leg_up_left_offset(0.198f, -0.289f, 0.005f);
    glm::vec3 leg_down_left_offset(0.183f, -0.630f, -0.072f);
    glm::vec3 foot_left_offset(0.195f, -1.082f, -0.081f);

    // 右腿
    glm::vec3 leg_up_right_offset(-0.197f, -0.289f, 0.005f);
    glm::vec3 leg_down_right_offset(-0.181f, -0.630f, -0.072f);
    glm::vec3 foot_right_offset(-0.195f, -1.082f, -0.081f);




    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f,  2.0f,  0.0f),
    };


    //onfigure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO, lightCubeVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vector<std::string> faces
    {
        "D:/ROBOT/Glitter/skybox/right.jpg",
        "D:/ROBOT/Glitter/skybox/left.jpg",
        "D:/ROBOT/Glitter/skybox/top.jpg",
        "D:/ROBOT/Glitter/skybox/bottom.jpg",
        "D:/ROBOT/Glitter/skybox/front.jpg",
        "D:/ROBOT/Glitter/skybox/back.jpg"
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    modelShader.use();
    modelShader.setInt("material.diffuse", 0);
    modelShader.setInt("material.specular", 1);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("D:/ROBOT/Glitter/Obj/botJoint/red_robot_Diffuse.png");
    unsigned int specularMap = loadTexture("D:/ROBOT/Glitter/Obj/botJoint/red_robot_Reflection.png");

    //Bind bone, root is body
    // ========== 建立骨架結構 ==========
    // root
    // 身體
    Bone body(&bodyModel, body_offset);
    // 頭部
    Bone head(&headModel, head_offset);

    // 右手
    Bone arm_up_right(&arm_up_rightModel, arm_up_right_offset);
    Bone arm_down_right(&arm_down_rightModel, arm_down_right_offset);
    Bone hand_right(&hand_rightModel, hand_right_offset);

    // 左手
    Bone arm_up_left(&arm_up_leftModel, arm_up_left_offset);
    Bone arm_down_left(&arm_down_leftModel, arm_down_left_offset);
    Bone hand_left(&hand_leftModel, hand_left_offset);

    // 左腿
    Bone leg_up_left(&leg_up_leftModel, leg_up_left_offset);
    Bone leg_down_left(&leg_down_leftModel, leg_down_left_offset);
    Bone foot_left(&foot_leftModel, foot_left_offset);

    // 右腿
    Bone leg_up_right(&leg_up_rightModel, leg_up_right_offset);
    Bone leg_down_right(&leg_down_rightModel, leg_down_right_offset);
    Bone foot_right(&foot_rightModel, foot_right_offset);

    // ========== 建立父子關係 ==========
    body.addChild(&head);
    body.addChild(&arm_up_left);
    body.addChild(&arm_up_right);
    body.addChild(&leg_up_left);
    body.addChild(&leg_up_right);

    arm_up_left.addChild(&arm_down_left);
    arm_up_right.addChild(&arm_down_right);
    leg_up_left.addChild(&leg_down_left);
    leg_up_right.addChild(&leg_down_right);

    arm_down_left.addChild(&hand_left);
    arm_down_right.addChild(&hand_right);
    leg_down_left.addChild(&foot_left);
    leg_down_right.addChild(&foot_right);

    body.updateGlobalTransform();
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        imgui.newframe();

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw in wireframe
        if (imgui.getPolygonMode()) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // LINE MODE
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // FILL MODE
        }

        body.resetLocalTransform();
        // action control
        if (imgui.getActionMode() == Idle) {
            //Idle
            foot_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        }
        else if (imgui.getActionMode() == Walk) {
            // Walk
            // 腳步擺動
            float bodyBounce = sin(4.0f * currentFrame) * 0.05f;  // 振幅0.05，可依實際模型大小調整
            float legSwing = glm::radians(30.0f) * sin(2.0f * currentFrame);  // 左右腿擺動
            float armSwing = glm::radians(50.0f) * sin(2.0f * currentFrame + glm::pi<float>()); // 手臂與腿相反擺動
            
            body.localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, bodyBounce, 0.0f));
            // 左腿
            leg_up_left.localTransform = glm::rotate(glm::mat4(1.0f), legSwing, glm::vec3(1, 0, 0));
            leg_down_left.localTransform = glm::rotate(glm::mat4(1.0f), -legSwing, glm::vec3(1, 0, 0)); // 膝蓋略微折
            // 右腿
            leg_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -legSwing, glm::vec3(1, 0, 0));
            leg_down_right.localTransform = glm::rotate(glm::mat4(1.0f), legSwing, glm::vec3(1, 0, 0));

            // 左手
            arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(1, 0, 0));
            if (-armSwing <= 0) arm_down_left.localTransform = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(1, 0, 0));
            else arm_down_left.localTransform = glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(1, 0, 0));
            
            // 右手
            arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(1, 0, 0));
            if(armSwing <= 0) arm_down_right.localTransform = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(1, 0, 0));
            else arm_down_right.localTransform = glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(1, 0, 0));
        }
        else if (imgui.getActionMode() == JumpingJacks) {


            float bodyBounce = -0.15f * abs(sin(2.0f * currentFrame));  // 振幅0.05
            float legSwing = abs(glm::radians(45.0f) * sin(2.0f * currentFrame));
            float armSwing = abs(glm::radians(120.0f) * sin(2.0f * currentFrame));


            body.localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, bodyBounce, 0.0f));
            // 左腿
            leg_up_left.localTransform = glm::rotate(glm::mat4(1.0f), legSwing, glm::vec3(0, 0, 1));
            // 右腿
            leg_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -legSwing, glm::vec3(0, 0, 1));
            // 左手
            arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(0, 0, 1));
            // 右手
            arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(0, 0, 1));




            //float g = -9.8f;
            //float jumpHeight = imgui.x;
            //float duration = 2.0f;
            //float timeInCycle = fmod(currentFrame * 0.5f, duration);

            //float half = duration / 2.0f;
            //float v0 = sqrtf(-2.0f * g * jumpHeight);

            //float y;
            //if (timeInCycle < half) {
            //    y = v0 * timeInCycle + 0.5f * g * timeInCycle * timeInCycle;
            //}
            //else {
            //    float t2 = timeInCycle - half;
            //    y = jumpHeight + (v0 * t2 + 0.5f * g * t2 * t2);
            //}
            //if (y < 0) y *= -1;

            //float legSwing = glm::radians(45.0f) * fabs(sin(currentFrame));
            //float armSwing = glm::radians(120.0f) * fabs(sin(currentFrame));

            //body.localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y, 0.0f));
            //leg_up_left.localTransform = glm::rotate(glm::mat4(1.0f), legSwing, glm::vec3(0, 0, 1));
            //leg_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -legSwing, glm::vec3(0, 0, 1));
            //arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(0, 0, 1));
            //arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(0, 0, 1));

        }
        else if (imgui.getActionMode() == Squat) {
            float t = fmod(currentFrame * 0.5f, 2.0f);  // 0~2
            if (t > 1.0f) t = 2.0f - t;
            
            float bodyBounce = 0.4f * t;
            float bodySwing = glm::radians(45.0f) * t;
            float bodySwingoffset = glm::radians(20.0f) * t;

            glm::vec3 legUpPosWorld(0, -0.289, 0.005);
            glm::vec3 _legUpPosWorld(0, -0.289- bodyBounce, 0.005);
            glm::vec3 legDownPosWorld(0, -0.630, -0.08147872411715451);
            glm::vec3 footPosWorld(0, -1.0815307858274943, -0.099);


            float LU = glm::length(legUpPosWorld - legDownPosWorld);
            float LD = glm::length(legDownPosWorld - footPosWorld);

            // 髖到腳的距離 (考慮身體下移)
            float D = glm::length(_legUpPosWorld - footPosWorld);

            D = glm::clamp(D, 0.0001f, LU + LD - 0.0001f);

            float cosKnee = (LU * LU + LD * LD - D * D) / (2 * LU * LD);
            float kneeAngle = glm::pi<float>() - acos(cosKnee);

            float cosThigh = (LU * LU + D * D - LD * LD) / (2 * LU * D);
            float thighAngle = acos(cosThigh);

            body.localTransform =
                glm::translate(glm::mat4(1.0f), footPosWorld)
                * glm::rotate(glm::mat4(1.0f), -bodySwingoffset, glm::vec3(1, 0, 0))
                * glm::translate(glm::mat4(1.0f), -footPosWorld)
                * glm::translate(glm::mat4(1.0f), glm::vec3(0.001f, -0.289, 0.005f))
                * glm::translate(glm::mat4(1.0f), glm::vec3(0, -bodyBounce, 0))
                * glm::rotate(glm::mat4(1.0f), bodySwing, glm::vec3(1, 0, 0))
                * glm::translate(glm::mat4(1.0f), glm::vec3(-0.001f, 0.289, -0.005f));


            // 左手
            arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1, 0, 0));
            arm_down_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
            // 右手
            arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1, 0, 0));
            arm_down_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

            leg_up_left.localTransform = glm::rotate(glm::mat4(1.0f), -bodySwing - thighAngle, glm::vec3(1, 0, 0));
            leg_down_left.localTransform = glm::rotate(glm::mat4(1.0f),  kneeAngle, glm::vec3(1, 0, 0));
            foot_left.localTransform = glm::rotate(glm::mat4(1.0f), bodySwingoffset + thighAngle - kneeAngle, glm::vec3(1, 0, 0));

            leg_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -bodySwing - thighAngle, glm::vec3(1, 0, 0));
            leg_down_right.localTransform = glm::rotate(glm::mat4(1.0f), kneeAngle, glm::vec3(1, 0, 0));
            foot_right.localTransform = glm::rotate(glm::mat4(1.0f), bodySwingoffset + thighAngle - kneeAngle, glm::vec3(1, 0, 0));

        }
        else if (imgui.getActionMode() == Situps) {
            //Sit - ups
            float t = fmod(currentFrame * 0.5f, 2.0f);  // 0~2
            if (t > 1.0f) t = 2.0f - t;

            float bodySwing = glm::radians(90.0f) * t;
            //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, bodyBounce, 0.0f)) * glm::rotate(glm::mat4(1.0f), bodySwing, glm::vec3(1, 0, 0))
            body.localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.001f, -0.289, 0.005f)) 
                * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f) + bodySwing, glm::vec3(1, 0, 0)) 
                * glm::translate(glm::mat4(1.0f), glm::vec3(-0.001f, 0.289, -0.005f));

            // 左手
            arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1, 0, 0));
            arm_down_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
            // 右手
            arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1, 0, 0));
            arm_down_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

            // 左腿
            leg_up_left.localTransform = glm::rotate(glm::mat4(1.0f), -glm::radians(60.0f) - bodySwing, glm::vec3(1, 0, 0));
            leg_down_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
            // 右腿
            leg_up_right.localTransform = glm::rotate(glm::mat4(1.0f), -glm::radians(60.0f) - bodySwing, glm::vec3(1, 0, 0));
            leg_down_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
        }
        else if (imgui.getActionMode() == Pushups) {
            //Pushups
            float t = fmod(currentFrame * 0.5f, 2.0f);  // 0~2
            if (t > 1.0f) t = 2.0f - t;

            float bodySwing = glm::radians(30.0f) * t;
            float armSwing = glm::radians(90.0f) * t;

            body.localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.001f, -1.150, 0.171f)) * glm::rotate(glm::mat4(1.0f), glm::radians(60.0f) + bodySwing, glm::vec3(1, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.001f, 1.150, -0.171f));

            // 左手
            arm_up_left.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(0, 0, 1));
            arm_down_left.localTransform = glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(0, 0, 1));
            // 右手
            arm_up_right.localTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), -armSwing, glm::vec3(0, 0, 1));;
            arm_down_right.localTransform = glm::rotate(glm::mat4(1.0f), armSwing, glm::vec3(0, 0, 1));
        }
        body.updateGlobalTransform();


        // don't forget to enable shader before setting uniforms
        modelShader.use();
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        
        modelShader.setBool("dirLight.enabled", imgui.getDirLight());
        modelShader.setBool("pointLight.enabled", imgui.getPointLight());
        modelShader.setBool("spotLight.enabled", imgui.getSpotLight());

        // directional light
        modelShader.setVec3("dirLight.direction", -0.2f, -0.3f, -0.8f);
        modelShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
        modelShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("dirLight.specular", 1.2f, 1.2f, 1.2f);


        // point light 
        modelShader.setVec3("pointLight.position", pointLightPositions[0]);
        modelShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLight.constant", 1.0f);
        modelShader.setFloat("pointLight.linear", 0.014f);
        modelShader.setFloat("pointLight.quadratic", 0.0007f);

        // spotLight
        modelShader.setVec3("spotLight.position", camera.Position);
        modelShader.setVec3("spotLight.direction", camera.Front);
        modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("spotLight.constant", 1.0f);
        modelShader.setFloat("spotLight.linear", 0.09f);
        modelShader.setFloat("spotLight.quadratic", 0.032f);
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));




        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        modelShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        body.Draw(modelShader);


        if (imgui.getPointLight() == true) {
            //draw the lamp object(s)
            lightCubeShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(lightCubeVAO);
            for (unsigned int i = 0; i < 1; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.05f)); // Make it a smaller cube
                lightCubeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            glBindVertexArray(0);
        }
        
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        imgui.genInterface();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &lightCubeVBO);

    imgui.end();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        firstMouse = true;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            std::cout << "Cubemap texture success to load at path: " << faces[i] << std::endl;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
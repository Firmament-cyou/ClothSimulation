#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"
#include "Cloth.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 9.0f));
float clastX = SCR_WIDTH / 2.0f;
float clastY = SCR_HEIGHT / 2.0f;
bool cameraFirstMouse = true;
bool cameraCanRotate = false;

// cloth
Cloth* cloth;

// timing
float deltaTime = 0.0f;
float totalTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ClothSimulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 线框模式（Debug挺好用的）
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 开启更新深度缓冲区的功能
    glEnable(GL_DEPTH_TEST);

    // 开启剔除面的功能
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 修复图片奇数宽度的问题
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Shader floor_shader("floor.vert", "floor.frag");
    Shader ball_shader("ball.vert", "ball.frag");
    Shader cloth_top_shader("clothTop.vert", "clothTop.frag");
    Shader cloth_bottom_shader("clothBottom.vert", "clothBottom.frag");

    float vertices[] = {
        // positions         // texture coords
         6.4f, -5.0f, -6.4f,  1.0f, 1.0f, // top right
        -6.4f, -5.0f, -6.4f,  1.0f, 0.0f, // bottom right
        -6.4f, -5.0f,  6.4f,  0.0f, 0.0f, // bottom left
         6.4f, -5.0f,  6.4f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int floor_VBO, floor_VAO, floor_EBO;
    glGenVertexArrays(1, &floor_VAO);
    glGenBuffers(1, &floor_VBO);
    glGenBuffers(1, &floor_EBO);

    glBindVertexArray(floor_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(5);
    // texture coord attribute
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(6);

    // 初始化布料质点
    const int grids = 20;
    float* particle_positions = new float[(grids + 1) * (grids + 1) * 3];
    unsigned int* top_indices = new unsigned int[grids * grids * 6];
    unsigned int* bottom_indices = new unsigned int[grids * grids * 6];
    cloth = new Cloth(particle_positions, top_indices, bottom_indices, grids);
    
    // 绘制布料
    unsigned int cloth_VBO, cloth_top_VAO, cloth_bottom_VAO, cloth_top_EBO, cloth_bottom_EBO;
    glGenVertexArrays(1, &cloth_top_VAO);
    glGenVertexArrays(1, &cloth_bottom_VAO);
    glGenBuffers(1, &cloth_VBO);
    glGenBuffers(1, &cloth_top_EBO);
    glGenBuffers(1, &cloth_bottom_EBO);

    glBindVertexArray(cloth_top_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
    glBufferData(GL_ARRAY_BUFFER, (grids + 1) * (grids + 1) * 3 * sizeof(float), particle_positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth_top_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, grids * grids * 6 * sizeof(unsigned int), top_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(7);

    glBindVertexArray(cloth_bottom_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
    glBufferData(GL_ARRAY_BUFFER, (grids + 1) * (grids + 1) * 3 * sizeof(float), particle_positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth_bottom_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, grids * grids * 6 * sizeof(unsigned int), bottom_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(7);

    // 动态数组可不能用 sizeof 呀，下面输出两个 4 太真实了吧（笑）
    // std::cout << sizeof(particle_positions) << " | " << sizeof(bottom_indices) << std::endl;

    // position attribute
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(7);

    // load and create a texture 
    unsigned int texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 垂直翻转
    
    unsigned char* data = stbi_load("textures/floor.tga", &width, &height, &nrChannels, 0);
    if (data)
    {
        // 利用图片生成纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // 释放内存

    floor_shader.use(); // activate/use the shader before setting uniforms
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(floor_shader.ID, "texture1"), 0);
    // or set it via the texture class
    // floor_shader.setInt("texture2", 1);

    // Model
    Model ball("models/ball.obj");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // 移动并更新 GPU 中的数据
        if ((totalTime += deltaTime) > 0.02f) {
            cloth->drop(totalTime);
            totalTime = 0.0f;
            glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (grids + 1) * (grids + 1) * 3 * sizeof(float), particle_positions);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render container
        floor_shader.use();
        floor_shader.setMat4("projection", projection);
        floor_shader.setMat4("view", view);

        glBindVertexArray(floor_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ball_shader.use();
        ball_shader.setMat4("projection", projection);
        ball_shader.setMat4("view", view);
        ball_shader.setVec3("viewPos", camera.Position);

        ball.Draw(ball_shader);

        cloth_top_shader.use();
        cloth_top_shader.setMat4("projection", projection);
        cloth_top_shader.setMat4("view", view);

        glBindVertexArray(cloth_top_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
        glDrawElements(GL_TRIANGLES, grids * grids * 6, GL_UNSIGNED_INT, 0);

        cloth_bottom_shader.use();
        cloth_bottom_shader.setMat4("projection", projection);
        cloth_bottom_shader.setMat4("view", view);

        glBindVertexArray(cloth_bottom_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cloth_VBO);
        glDrawElements(GL_TRIANGLES, grids * grids * 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteVertexArrays(1, &floor_VAO);
    glDeleteBuffers(1, &floor_VBO);
    glDeleteBuffers(1, &floor_EBO);
    glDeleteVertexArrays(1, &cloth_top_VAO);
    glDeleteVertexArrays(1, &cloth_bottom_VAO);
    glDeleteBuffers(1, &cloth_VBO);
    glDeleteBuffers(1, &cloth_top_EBO);
    glDeleteBuffers(1, &cloth_bottom_EBO);

    delete[] particle_positions;
    delete[] bottom_indices;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) cloth->fix1 = false;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) cloth->fix2 = false;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) cloth->fix3 = false;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) cloth->fix4 = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cloth->restore();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraCanRotate) {
        if (cameraFirstMouse)
        {
            clastX = xpos;
            clastY = ypos;
            cameraFirstMouse = false;
        }

        float xoffset = xpos - clastX;
        float yoffset = clastY - ypos; // reversed since y-coordinates go from bottom to top

        clastX = xpos;
        clastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) cameraCanRotate = true;
    } else if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            cameraCanRotate = false;
            cameraFirstMouse = true;
        }
    }
    return;
}
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stb_image.h>

#include "Shader.hpp"

struct SpriteFrame
{
    float x, y, w, h;
};

unsigned int LoadTexture(const char* filepath, GLint wrapS = GL_CLAMP_TO_EDGE)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 4);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error al cargar la textura!" << std::endl;
    }

    return textureID;
}

int main()
{
    if(!glfwInit())
    {
        std::cerr << "Error al iniciar GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
    if(!window)
    {
        std::cerr << "Error al crear la ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (!version)
    {
        std::cerr << "Error al iniciar GLAD" << std::endl;
        return -1;
    }
    std::cout << "OpenGL cargado con exito. Version: "
                  << GLAD_VERSION_MAJOR(version) << "."
                  << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    float vertices[] = {
        // X      Y      Z      U      V
        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    auto basicShader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    auto proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    float posX = 400.0f;
    float posY = 300.0f;
    float velY = 0.0f;
    float speed = 300.0f;
    const float gravity = 1200.0f;
    const float jumpForce = -600.0f;
    bool isGrounded = false;
    float facingDirection = 1.0f;

    float coinX = 600.0f;
    float coinY = 450.0f;
    bool coinCollected = false;

    float lastFrameTime = 0.0f;

    stbi_set_flip_vertically_on_load(false);

    auto bgCloudsTextureID    = LoadTexture("assets/textures/background_clouds.png", GL_REPEAT);
    auto bgTreesTextureID     = LoadTexture("assets/textures/background_color_trees.png", GL_REPEAT);
    auto blockMiddleTextureID = LoadTexture("assets/textures/terrain_grass_horizontal_middle.png");
    auto blockLeftTextureID   = LoadTexture("assets/textures/terrain_grass_horizontal_left.png");
    auto blockRightTextureID  = LoadTexture("assets/textures/terrain_grass_horizontal_right.png");
    auto coinTextureID        = LoadTexture("assets/textures/coin_gold.png");

    auto characterTextureID = LoadTexture("assets/textures/spritesheet-characters-default.png");
    const float sheetWidth  = 902.0f;
    const float sheetHeight = 902.0f;

    SpriteFrame frameIdle  = { 0.0f,   258.0f, 128.0f, 128.0f };
    SpriteFrame frameJump  = { 129.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame frameWalkA = { 258.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame frameWalkB = { 387.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame currentFrame = frameIdle;

    float animTimer = 0.0f;
    const float animSpeed = 0.15f;
    int walkStep = 0;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrameTime = (float)glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float moveX = 0.0f;
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveX -= 1.0f;
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveX += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
        {
            velY = jumpForce;
            isGrounded = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        posX += moveX * speed * deltaTime;
        velY += gravity * deltaTime;
        posY += velY * deltaTime;

        const float groundY = 486.0f;
        if (posY >= groundY)
        {
            posY = groundY;
            velY = 0.0f;
            isGrounded = true;
        }

        if (moveX != 0.0f)
        {
            facingDirection = moveX;
            animTimer += deltaTime;
            walkStep = (int)(animTimer / animSpeed) % 2;
        }
        else
        {
            animTimer = 0.0f;
            walkStep = 0;
        }

        if (!isGrounded)
            currentFrame = frameJump;
        else if (moveX != 0)
            currentFrame = (walkStep == 0) ? frameWalkA : frameWalkB;
        else
            currentFrame = frameIdle;

        float camX = posX - 400.0f;
        float camY = 186.0f;

        auto view = glm::translate(glm::mat4(1.0f), glm::vec3(-camX, -camY, 0.0f));
        auto vp   = proj * view;

        basicShader->Bind();
        basicShader->SetUniform1i("u_Texture", 0);
        basicShader->SetUniform2f("u_UVScale", 1.0f, 1.0f);
        basicShader->SetUniform2f("u_UVOffset", 0.0f, 0.0f);
        glBindVertexArray(VAO);

        const float bgTileSize = 300.0f;
        glBindTexture(GL_TEXTURE_2D, bgCloudsTextureID);
        basicShader->SetUniform2f("u_UVScale", 800.0f / bgTileSize, 1.0f);
        basicShader->SetUniform2f("u_UVOffset", (camX * 0.1f) / bgTileSize, 0.0f);

        auto modelClouds = glm::mat4(1.0f);
        modelClouds = glm::translate(modelClouds, glm::vec3(400.0f, 150.0f, 0.0f));
        modelClouds = glm::scale(modelClouds, glm::vec3(800.0f, bgTileSize, 1.0f));
        basicShader->SetUniformMat4f("u_MVP", proj * modelClouds);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, bgTreesTextureID);
        basicShader->SetUniform2f("u_UVOffset", (camX * 0.25f) / bgTileSize, 0.0f);

        auto modelTrees = glm::mat4(1.0f);
        modelTrees = glm::translate(modelTrees, glm::vec3(400.0f, 450.0f, 0.0f));
        modelTrees = glm::scale(modelTrees, glm::vec3(800.0f, bgTileSize, 1.0f));
        basicShader->SetUniformMat4f("u_MVP", proj * modelTrees);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        basicShader->SetUniform2f("u_UVScale", 1.0f, 1.0f);
        basicShader->SetUniform2f("u_UVOffset", 0.0f, 0.0f);

        for (int i = -5; i < 15; i++)
        {
            if(i == -5)
                glBindTexture(GL_TEXTURE_2D, blockLeftTextureID);
            else if(i == 14)
                glBindTexture(GL_TEXTURE_2D, blockRightTextureID);
            else
                glBindTexture(GL_TEXTURE_2D, blockMiddleTextureID);

            float blockX = i * 64.0f;
            float blockY = 568.0f;

            auto modelBlock = glm::mat4(1.0f);
            modelBlock = glm::translate(modelBlock, glm::vec3(blockX, blockY, 0.0f));
            modelBlock = glm::scale(modelBlock, glm::vec3(64.0f, 64.0f, 1.0f));

            auto mvpBlock = vp * modelBlock;
            basicShader->SetUniformMat4f("u_MVP", mvpBlock);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        if(!coinCollected)
        {
            bool overlapX = std::abs(posX - coinX) < (60.0f / 2.0f + 40.0f / 2.0f);
            bool overlapY = std::abs(posY - coinY) < (60.0f / 2.0f + 40.0f / 2.0f);

            if(overlapX && overlapY)
            {
                coinCollected = true;
                std::cout << "Moneda recogida!" << std::endl;
            }
        }

        if(!coinCollected)
        {
            glBindTexture(GL_TEXTURE_2D, coinTextureID);

            auto modelCoin = glm::mat4(1.0f);
            modelCoin = glm::translate(modelCoin, glm::vec3(coinX, coinY, 0.0f));
            modelCoin = glm::scale(modelCoin, glm::vec3(48.0f, 48.0f, 1.0f));

            auto mvpCoin = vp * modelCoin;
            basicShader->SetUniformMat4f("u_MVP", mvpCoin);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        basicShader->SetUniform2f("u_UVScale", currentFrame.w / sheetWidth, currentFrame.h / sheetHeight);
        basicShader->SetUniform2f("u_UVOffset", currentFrame.x / sheetWidth, currentFrame.y / sheetHeight);
        glBindTexture(GL_TEXTURE_2D, characterTextureID);

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f * facingDirection, 100.0f, 1.0f));

        auto mvp = vp * model;
        basicShader->SetUniformMat4f("u_MVP", mvp);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    basicShader->Unbind();
    basicShader.reset();

    glDeleteTextures(1, &characterTextureID);
    glDeleteTextures(1, &blockMiddleTextureID);
    glDeleteTextures(1, &blockLeftTextureID);
    glDeleteTextures(1, &blockRightTextureID);
    glDeleteTextures(1, &coinTextureID);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

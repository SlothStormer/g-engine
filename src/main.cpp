#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Shader.hpp"
#include "SpriteRenderer.hpp"
#include "Texture2D.hpp"
#include "Window.hpp"

int main()
{
    Window w;

    auto basicShader = Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    SpriteRenderer renderer(basicShader);

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

    auto bgCloudsTexture    = Texture2D("assets/textures/background_clouds.png", GL_REPEAT);
    auto bgTreesTexture     = Texture2D("assets/textures/background_color_trees.png", GL_REPEAT);
    auto blockMiddleTexture = Texture2D("assets/textures/terrain_grass_horizontal_middle.png");
    auto blockLeftTexture   = Texture2D("assets/textures/terrain_grass_horizontal_left.png");
    auto blockRightTexture  = Texture2D("assets/textures/terrain_grass_horizontal_right.png");
    auto coinTexture        = Texture2D("assets/textures/coin_gold.png");

    auto characterTexture   = Texture2D("assets/textures/spritesheet-characters-default.png");

    SpriteFrame frameIdle  = { 0.0f,   258.0f, 128.0f, 128.0f };
    SpriteFrame frameJump  = { 129.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame frameWalkA = { 258.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame frameWalkB = { 387.0f, 258.0f, 128.0f, 128.0f };
    SpriteFrame currentFrame = frameIdle;

    float animTimer = 0.0f;
    const float animSpeed = 0.15f;
    int walkStep = 0;

    while(!w.shouldClose())
    {
        float currentFrameTime = (float)w.getTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        w.pollEvents();

        if(w.keyIsPressed(GLFW_KEY_ESCAPE))
            w.setShouldClose(true);

        float moveX = 0.0f;
        if(w.keyIsPressed(GLFW_KEY_A)) moveX -= 1.0f;
        if(w.keyIsPressed(GLFW_KEY_D)) moveX += 1.0f;

        if (w.keyIsPressed(GLFW_KEY_SPACE) && isGrounded)
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

        renderer.begin(proj);

        const float bgTileSize = 300.0f;
        renderer.draw(bgCloudsTexture,
                      {400.0f, 150.0f},
                      {800.0f, bgTileSize},
                      {(camX * 0.1f) / bgTileSize, 0.0f},
                      {800.0f / bgTileSize, 1.0f});

        renderer.draw(bgTreesTexture,
                      {400.0f, 450.0f},
                      {800.0f, bgTileSize},
                      {(camX * 0.25f) / bgTileSize, 0.0f},
                      {800.0f / bgTileSize, 1.0f});

        renderer.begin(vp);

        for (int i = -5; i < 15; i++)
        {
            float blockX = i * 64.0f;
            float blockY = 568.0f;

            if(i == -5)
                 renderer.draw(blockLeftTexture, {blockX, blockY}, {64.0f, 64.0f});
            else if(i == 14)
                renderer.draw(blockRightTexture, {blockX, blockY}, {64.0f, 64.0f});
            else
                renderer.draw(blockMiddleTexture, {blockX, blockY}, {64.0f, 64.0f});
        }

        if(!coinCollected)
        {
            renderer.draw(coinTexture, {coinX, coinY}, {48.0f, 48.0f});
        }

        renderer.drawFrame(characterTexture, currentFrame, {posX, posY}, {100.0f  * facingDirection, 100.0f});

        w.swapBuffers();
    }

    basicShader.Unbind();

    return 0;
}

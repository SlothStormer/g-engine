#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <vector>

#include "Camara2D.hpp"
#include "Physics.hpp"
#include "Shader.hpp"
#include "SpriteRenderer.hpp"
#include "Texture2D.hpp"
#include "Window.hpp"

struct Block
{
    glm::vec2 pos;
    glm::vec2 size;
    Texture2D* texture;
};

int main()
{
    Window w;

    auto basicShader = Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    SpriteRenderer renderer(basicShader);

    Camara2D cam(800.0f, 600.0f);

    float posX = 400.0f;
    float posY = 300.0f;
    float velY = 0.0f;
    float speed = 300.0f;
    const float gravity = 1200.0f;
    const float jumpForce = -650.0f;
    bool isGrounded = false;
    float facingDirection = 1.0f;

    float coinX = 600.0f;
    float coinY = 450.0f;
    bool coinCollected = false;

    double lastFrameTime = 0.0f;

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

    std::vector<Block> blocks;

    for(int i = -5; i < 15; i++)
    {
        Texture2D* tex = &blockMiddleTexture;
        if (i == -5) tex = &blockLeftTexture;
        else if (i == 14) tex = &blockRightTexture;

        blocks.push_back({ {i * 64.0f, 568.0f}, {64.0f, 64.0f}, tex });
    }

    blocks.push_back({ {256.0f, 504.0f}, {64.0f, 64.0f}, &blockMiddleTexture });

    blocks.push_back({ {448.0f, 350.0f}, {64.0f, 64.0f}, &blockLeftTexture });
    blocks.push_back({ {512.0f, 350.0f}, {64.0f, 64.0f}, &blockMiddleTexture });
    blocks.push_back({ {576.0f, 350.0f}, {64.0f, 64.0f}, &blockRightTexture });

    while(!w.shouldClose())
    {
        double currentFrameTime = w.getTime();
        float deltaTime = (float)(currentFrameTime - lastFrameTime);
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

        glm::vec2 playerHitbox = { 64.0f, 76.0f };
        glm::vec2 hitboxOffset = { 0.0f, 12.0f };

        posX += moveX * speed * deltaTime;

        for (const auto& block : blocks)
        {
            glm::vec2 colliderPos = { posX + hitboxOffset.x, posY + hitboxOffset.y };
            glm::vec2 overlap = Physics::getOverlap({ colliderPos, playerHitbox }, { block.pos, block.size });

            if (overlap.x > 0.0f && overlap.y > 0.0f)
            {
                if (moveX > 0.0f)
                    posX -= overlap.x;
                else if(moveX < 0.0f)
                    posX += overlap.x;
            }
        }

        velY += gravity * deltaTime;
        posY += velY * deltaTime;

        for (const auto& block : blocks)
        {
            glm::vec2 colliderPos = { posX + hitboxOffset.x, posY + hitboxOffset.y };
            glm::vec2 overlap = Physics::getOverlap({ colliderPos, playerHitbox }, { block.pos, block.size });

            if (overlap.x > 0.0f && overlap.y > 0.0f)
            {
                if (velY > 0.0f)
                {
                    posY -= overlap.y;
                    velY = 0.0f;
                }
                else if (velY < 0.0f)
                {
                    posY += overlap.y;
                    velY = 0.0f;
                }
            }
        }

        isGrounded = false;
        if (velY >= 0.0f)
        {
            glm::vec2 feetPos = { posX + hitboxOffset.x, posY + hitboxOffset.y + 1.0f };
            for (const auto& block : blocks)
            {
                if (Physics::checkOverlap({ feetPos, playerHitbox }, { block.pos, block.size }))
                {
                    isGrounded = true;
                    break;
                }
            }
        }

        if (!coinCollected)
        {
            glm::vec2 colliderPos = { posX + hitboxOffset.x, posY + hitboxOffset.y };
            if (Physics::checkOverlap({ colliderPos, playerHitbox }, { {coinX, coinY}, {40.0f, 40.0f} }))
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

        // Drawing
        cam.setCenter({posX, 486.0f});
        renderer.begin(cam.getProjection());

        // Background
        const float bgTileSize = 300.0f;
        renderer.draw(bgCloudsTexture,
                      {400.0f, 150.0f},
                      {800.0f, bgTileSize},
                      {(cam.getPosition().x * 0.1f) / bgTileSize, 0.0f},
                      {800.0f / bgTileSize, 1.0f});

        renderer.draw(bgTreesTexture,
                      {400.0f, 450.0f},
                      {800.0f, bgTileSize},
                      {(cam.getPosition().x * 0.25f) / bgTileSize, 0.0f},
                      {800.0f / bgTileSize, 1.0f});

        renderer.begin(cam.getViewProjection());

        // Platforms
        for (const auto& block : blocks)
        {
            renderer.draw(*block.texture, block.pos, block.size);
        }

        // Coin
        if(!coinCollected)
        {
            renderer.draw(coinTexture, {coinX, coinY}, {48.0f, 48.0f});
        }

        // Player
        renderer.drawFrame(characterTexture, currentFrame, {posX, posY}, {100.0f  * facingDirection, 100.0f});

        w.swapBuffers();
    }

    basicShader.Unbind();

    return 0;
}

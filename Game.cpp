#include <SFML/Window.hpp>
#include <random>
#include <iostream>
#include <algorithm>

#include "Game.hpp"

Game::Game(GLuint _width, GLuint _height)
    : state(GAME_ACTIVE), keys(), width(_width), height(_height), cameraAtual(CAM_THIRD_PERSON)
{

}

void Game::init()
{
    // Load shaders
    ResourceManager::loadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::loadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
    ResourceManager::loadShader("shaders/postProcessing.vs", "shaders/postProcessing.frag", nullptr, "postProcessing");
    // Load textures
    //ResourceManager::loadTexture("textures/block.png", GL_FALSE, "block");
    ResourceManager::loadTexture("textures/rock.png", GL_FALSE, "block");
    ResourceManager::loadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::loadTexture("textures/paddle.png", GL_FALSE, "paddle");
    ResourceManager::loadTexture("textures/crosshair.png", GL_TRUE, "crosshair");
    ResourceManager::loadTexture("textures/particle.png", GL_TRUE, "particle");

    this->camera = std::make_unique<Camera>();
    // Set render-specific controls
    this->renderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("sprite"));
    // Configure game objects
    this->player = std::make_unique<GameObject>(glm::vec3(0.0f, -1.0f, 0.0f), PLAYER_SIZE, glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(1.5f, 1.5f, 1.5f), 0, ResourceManager::getTexture("paddle"));

    this->crosshair = std::make_unique<GameObject>(glm::vec3(0.0f, -1.0f, -8.0f), glm::vec3(0.3f, 0.3f, 0.0f), glm::vec3(0.5f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0, ResourceManager::getTexture("crosshair"));

    this->particles = std::make_unique<ParticleGenerator>(ResourceManager::getShader("particle"), ResourceManager::getTexture("particle"), 100);

    this->effects = std::make_unique<PostProcessor>(ResourceManager::getShader("postProcessing"), this->width, this->height);

    this->resetCameraPosition();

    glm::mat4 projection = glm::perspective(this->camera->Zoom, (float)this->width/(float)this->height, 0.1f, 1000.0f);
    ResourceManager::getShader("sprite").Use().SetMatrix4("projection", projection);
    ResourceManager::getShader("particle").Use().SetMatrix4("projection", projection);

    // ResourceManager::getShader("sprite").Use().SetMatrix4("projection", glm::perspective(this->camera->Zoom, (float)this->width/(float)this->height, 0.1f, 1000.0f));
}

void Game::update(GLfloat dt)
{
    this->doCollisions();

    this->particles->update(dt, *this->player, 2);

    // excluir tiros do vetor se estiverem < z -30
    this->tiros.erase(std::remove_if(this->tiros.begin(), this->tiros.end(), [](auto & tiro){
        return tiro.position.z <= -30 || tiro.destroyed;
    }), this->tiros.end());

    // excluir asteroides ?
    // ???

    // atualizar posicoes dos tiros
    // diminui o z , vao para o fundo
    for(auto & tiro : this->tiros)
    {
        tiro.position.z -= tiro.velocity.z;
        tiro.position.x += tiro.velocity.x;
        tiro.position.y += tiro.velocity.y;
    }

    // atualizar posicoes dos asteroides
    // aumenta o z, vem em direcao ao player
    for(auto & asteroide : this->asteroides)
    {
        if (asteroide.position.z >= 1)
        {
            asteroide.destroyed = true;
        }
        asteroide.position.z += asteroide.velocity.z;
        asteroide.rotation += dt;
    }

    // criar asteroides
    if (this->clockCriacaoAsteroide.getElapsedTime().asSeconds() > 1)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        static std::uniform_real_distribution<> disM2P2(-2, 2);
        static std::uniform_real_distribution<> disM10M20(-10, -20);
        static std::uniform_real_distribution<> disP0P1(0, 1);

        this->clockCriacaoAsteroide.restart();
        glm::vec3 posAsteroide = glm::vec3(disM2P2(gen), disM2P2(gen), disM10M20(gen));
        float tam = disP0P1(gen);
        glm::vec3 tamAsteroide = glm::vec3(tam, tam, tam);
        glm::vec3 corAsteroide = glm::vec3(0.7f, 0.6f, 0.6f);
        glm::vec3 velocidadeAsteroide = glm::vec3(0.0f, 0.0f, dt);
        GLfloat rotation = 0;
        this->asteroides.push_back(GameObject(posAsteroide, tamAsteroide, corAsteroide, velocidadeAsteroide, rotation, ResourceManager::getTexture("block")));
    }
}

void Game::processInput(GLfloat dt)
{
    if (this->state == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        bool resetCameraPosition = false;
        // Move playerboard
        // Camera controls
        if (this->keys[sf::Keyboard::W])
        {
            this->player->position.y += velocity;
            resetCameraPosition = true;
        }
        else if (this->keys[sf::Keyboard::S])
        {
            this->player->position.y -= velocity;
            resetCameraPosition = true;
        }
        else if (this->keys[sf::Keyboard::A])
        {
            this->player->position.x -= velocity;
            resetCameraPosition = true;
        }
        else if (this->keys[sf::Keyboard::D])
        {
            this->player->position.x += velocity;
            resetCameraPosition = true;
        }
        else if (this->keys[sf::Keyboard::Num1])
        {
            this->effects->enable(Effect::CONFUSE);
        }
        else if (this->keys[sf::Keyboard::Num2])
        {
            this->effects->enable(Effect::SHAKE);
        }
        else if (this->keys[sf::Keyboard::Num3])
        {
            this->effects->enable(Effect::CHAOS);
        }
        else if (this->keys[sf::Keyboard::Num0])
        {
            this->effects->enable(Effect::NORMAL);
        }

        // nao colocar else if, assim o player consegue se mexer e atirar ao mesmo tempo
        if (this->keys[sf::Keyboard::Space])
        {
            if (this->clockCriacaoTiro.getElapsedTime().asMilliseconds() > 100)
            {
                this->clockCriacaoTiro.restart();
                glm::vec3 posTiro = this->player->position;
                glm::vec3 tamTiro = glm::vec3(0.05f, 0.05f, 0.05f);
                glm::vec3 corTiro = glm::vec3(0.3f, 0.5f, 0.5f);
                glm::vec3 velocidadeTiro = glm::vec3(0.0f, 0.0f, 0.1f);
                this->tiros.push_back(GameObject(posTiro, tamTiro, corTiro, velocidadeTiro, 0, ResourceManager::getTexture("block")));
                resetCameraPosition = true;
            }
        }
        else if (this->keys[sf::Keyboard::C]) // mudar camera
        {
            if (this->clockMudancaCamera.getElapsedTime().asMilliseconds() > 500)
            {
                this->clockMudancaCamera.restart();
                this->cameraAtual = this->cameraAtual == CAM_THIRD_PERSON ? CAM_FIRST_PERSON : CAM_THIRD_PERSON;
                resetCameraPosition = true;
            }
        }

        if (resetCameraPosition)
        {
            this->resetCameraPosition();
            this->resetCrosshairPosition();
        }
    }
}

void Game::render(GLfloat dt)
{
    if (this->state == GAME_ACTIVE)
    {
        effects->beginRender();

        if (this->cameraAtual == CAM_THIRD_PERSON)
            this->particles->draw();

        // Draw player
        this->player->draw(*this->renderer);

        // Desenhar tiros
        for(auto & tiro : this->tiros)
        {
            if (!tiro.destroyed) tiro.draw(*this->renderer);
        }
        // Desenhar asteroides
        for(auto & asteroide : this->asteroides)
        {
            if (!asteroide.destroyed) asteroide.draw(*this->renderer);
        }

        // para mostrar o crosshair sempre em primeiro plano
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw crosshair
        this->crosshair->draw(*this->renderer);

        effects->endRender();

        effects->render(dt);
    }
}

GLboolean checkCollision(const GameObject &one, const GameObject &two) // AABB - AABB collision
{
    // Collision x-axis?
    bool collisionX = one.position.x + (one.size.x/2) > two.position.x - (two.size.x/2) &&
        one.position.x - (one.size.x/2) < two.position.x + (two.size.x/2);
    // Collision y-axis?
    bool collisionY = one.position.y + (one.size.y/2) > two.position.y - (two.size.y/2) &&
        one.position.y - (one.size.y/2) < two.position.y + (two.size.y/2);
    // Collision z-axis?
    bool collisionZ = one.position.z + (one.size.z/2) > two.position.z - (two.size.z/2) &&
        one.position.z - (one.size.z/2) < two.position.z + (two.size.z/2);
    // Collision only if on both axes

    return collisionX && collisionY && collisionZ;
}

void Game::doCollisions()
{
    for (GameObject & asteroide : this->asteroides)
    {
        if (!asteroide.destroyed)
        {
            // verifica se asteroide bateu no player
            if (checkCollision(*this->player, asteroide))
            {
                // player morre
                std::cout << "morreu\n";
            }
            else
            {
                // verifica se tiro acertou algum asteroide
                for (GameObject & tiro : this->tiros)
                {
                    if (!tiro.destroyed)
                    {
                        if (checkCollision(asteroide, tiro))
                        {
                            asteroide.destroyed = true;
                            tiro.destroyed = true;
                        }
                    }
                }
            }
        }
    }
}

void Game::resetCameraPosition()
{
    double z = this->cameraAtual == CAM_THIRD_PERSON
        ? 2.0f
        : this->player->size.z * 2; //  dobro do size z pra ficar atras

    // seta na posicao do player mais 20 % no y
    this->camera->resetPosition(glm::vec3(this->player->position.x, this->player->position.y + (this->player->size.y*1.2), z));

    ResourceManager::getShader("sprite").Use().SetVector3f("viewPos", this->camera->Position.x, this->camera->Position.y, this->camera->Position.z);
    ResourceManager::getShader("sprite").SetMatrix4("view", this->camera->GetViewMatrix());

    ResourceManager::getShader("particle").Use().SetMatrix4("view", this->camera->GetViewMatrix());
}

void Game::resetCrosshairPosition()
{
    this->crosshair->position.x = this->player->position.x;
    this->crosshair->position.y = this->player->position.y;
    this->crosshair->position.z = this->cameraAtual == CAM_THIRD_PERSON ? -8.0f : -4.5f;
}

#ifndef GAME_H
#define GAME_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Window.hpp>

#include "GameObject.hpp"
#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "ParticleGenerator.hpp"
#include "PostProcessor.hpp"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum TipoCamera {
    CAM_THIRD_PERSON,
    CAM_FIRST_PERSON
};

// Initial size of the player paddle
const glm::vec3 PLAYER_SIZE {0.6f, 0.2f, 0.2f};
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY {1.5f};

class Game
{
    public:
        // Game state
        GameState  state;
        GLboolean  keys[1024];
        GLuint	   width, height;
        TipoCamera cameraAtual;
        // Game-related State data
        std::unique_ptr<SpriteRenderer> renderer;
        std::unique_ptr<GameObject> player;
        std::unique_ptr<GameObject> crosshair;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<ParticleGenerator> particles;
        std::unique_ptr<PostProcessor> effects;
        // Constructor/Destructor
        Game(GLuint width, GLuint height);
        // Initialize game state (load all shaders/textures/levels)
        std::vector<GameObject> tiros;
        std::vector<GameObject> asteroides;
        sf::Clock clockCriacaoTiro;
        sf::Clock clockMudancaCamera;
        sf::Clock clockCriacaoAsteroide;
        void init();
        // GameLoop
        void processInput(GLfloat dt);
        void update(GLfloat dt);
        void render(GLfloat dt);
        void doCollisions();
        void resetLevel();
        void resetPlayer();
        void resetCameraPosition();
        void resetCrosshairPosition();
};

GLboolean checkCollision(const GameObject &, const GameObject &);

#endif
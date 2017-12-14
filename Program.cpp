#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>

#include "Game.hpp"
#include "ResourceManager.hpp"


// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

void processarKeyInput(sf::Window & window, Game & asteroids, bool & running)
{
    sf::Event windowEvent;
    while (window.pollEvent(windowEvent))
    {
        switch (windowEvent.type)
        {
        case sf::Event::Closed:
            running = false;
            break;
        case sf::Event::KeyPressed:
            asteroids.keys[windowEvent.key.code] = GL_TRUE;
            break;
        case sf::Event::KeyReleased:
            asteroids.keys[windowEvent.key.code] = GL_FALSE;
            break;
        default:
            ;
        }
    }
}

int main(int argc, char *argv[])
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3; /* 1 */
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::Window window(sf::VideoMode(800, 600, 32), "Asteroids", sf::Style::Titlebar | sf::Style::Close, settings);

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // imagens transparentes (crosshair)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize game
    Game asteroids(SCREEN_WIDTH, SCREEN_HEIGHT);
    asteroids.init();

    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    // Start Game within Menu State
    asteroids.state = GAME_ACTIVE;

    sf::Clock clock;
    bool running = true;
    while (running)
    {
        processarKeyInput(window, asteroids, running);

        // Calculate delta time
        GLfloat currentFrame = clock.getElapsedTime().asSeconds();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Manage user input
        asteroids.processInput(deltaTime);

        // Update Game state
        asteroids.update(deltaTime);

        // Render

        asteroids.render(deltaTime);

        window.display();
    }

    // Delete all resources as loaded using the resource manager
    ResourceManager::clear();

    window.close();

    return 0;
}

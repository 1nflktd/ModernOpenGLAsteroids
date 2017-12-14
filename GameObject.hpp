#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // Object state
    glm::vec3   position, size, velocity, color;
    GLfloat     rotation;
    // Render state
    GLboolean   destroyed;
    Texture2D   sprite;
    // Constructor(s)
    GameObject();
    GameObject(const glm::vec3 & pos, const glm::vec3 & size, const glm::vec3 & color, const glm::vec3 & velocity, GLfloat rotation, const Texture2D & sprite);
    // Draw sprite
    void draw(SpriteRenderer &renderer);
};

#endif
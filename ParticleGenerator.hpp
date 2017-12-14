#ifndef PARTICLE_GENERATOR_HPP
#define PARTICLE_GENERATOR_HPP
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"
#include "GameObject.hpp"

// Represents a single particle and its state
struct Particle {
    glm::vec3 position, velocity;
    glm::vec4 color;
    GLfloat life;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
    // Constructor
    ParticleGenerator(const Shader & shader, const Texture2D & texture, GLuint amount);
    // update all particles
    void update(GLfloat dt, const GameObject & object, GLuint newParticles);
    // Render all particles
    void draw();
private:
    // State
    std::vector<Particle> particles;
    // Render state
    Shader shader;
    Texture2D texture;
    // State
    GLuint amount;
    // Render state
    GLuint VAO;
    // Initializes buffer and vertex attributes
    void init();
    // Returns the first Particle index that's currently unused e.g. life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle();
    // Respawns particle
    void respawnParticle(Particle &particle, const GameObject &object);
    // Stores the index of the last particle used (for quick access to next dead particle)
    GLuint lastUsedParticle;
};

#endif
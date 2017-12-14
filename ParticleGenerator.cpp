#include "ParticleGenerator.hpp"

#include <random>
#include <iostream>

ParticleGenerator::ParticleGenerator(const Shader & shader, const Texture2D & texture, GLuint amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

void ParticleGenerator::update(GLfloat dt, const GameObject & object, GLuint newParticles)
{
    // Add new particles
    for (GLuint i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object);
    }
    // Update all particles

    for (GLuint i = 0; i < this->amount; ++i)
    {
        this->particles[i].life -= dt; // reduce life
        if (this->particles[i].life > 0.0f)
        {	// particle is alive, thus update
            this->particles[i].position.x -= this->particles[i].velocity.x * dt;
            this->particles[i].position.y -= this->particles[i].velocity.y * dt;
            this->particles[i].color.a -= dt * 0.1;
        }
    }

}

// Render all particles
void ParticleGenerator::draw()
{
    this->shader.Use();
    for (const auto & particle : this->particles)
    {
        if (particle.life > 0.0f)
        {
            this->shader.SetVector3f("offset", particle.position);
            this->shader.SetVector4f("color", particle.color);
            this->texture.bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
}

void ParticleGenerator::init()
{
    // Set up mesh and attribute properties
    GLuint VBO;
    GLfloat particle_quad[] = {
        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,        0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,        1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,        1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,        0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,        0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,        0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,        0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,        0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,        0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,        0.0f, 0.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // Set mesh attributes

    GLint vPosition_attrib = glGetAttribLocation(this->shader.ID, "vPosition");
    glVertexAttribPointer(vPosition_attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(vPosition_attrib);

    GLint vTexCoords_attrib = glGetAttribLocation(this->shader.ID, "vTexCoords");
    glVertexAttribPointer(vTexCoords_attrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(vTexCoords_attrib);

    glBindVertexArray(0);

    // Create this->amount default particle instances
    for (GLuint i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

GLuint ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = this->lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].life <= 0.0f){
            this->lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < this->lastUsedParticle; ++i){
        if (this->particles[i].life <= 0.0f){
            this->lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    this->lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle & particle, const GameObject & object)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    static std::uniform_real_distribution<float> rColorDistribution(0, 1);
    static std::uniform_real_distribution<float> rPositionDistribution(-0.05, 0.05);

    GLfloat rColor = rColorDistribution(gen);

    particle.position = object.position + rPositionDistribution(gen);
    particle.position.z = 0;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.0f;
    particle.velocity = object.velocity * 0.1f;
}

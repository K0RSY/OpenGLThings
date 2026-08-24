#ifndef SHDR
#define SHDR 1

#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rdfl.hpp"

using namespace std;
using namespace glm;

typedef struct {
    unsigned int VBO, VAO, EBO, program;
    float* vertecies;
    unsigned int* indices;
    unsigned int vertecies_count, indices_count;
} shdr;

// Define
shdr shdr_init(
    float* vertecies, unsigned int* indices,
    unsigned int vertecies_count, unsigned int indices_count,
    const char* vert_path, const char* frag_path,
    unsigned int attribute_count, unsigned int* attribute_sizes
);
unsigned int shdr_init_texture(const char* path);

void shdr_set_unformf(shdr* shader, const char* name, float value);
void shdr_set_unformi(shdr* shader, const char* name, int value);
void shdr_set_unformm4(shdr* shader, const char* name, float* value_pointer);
void shdr_set_texture(shdr* shader, const char* name, unsigned int texture, unsigned int id);

void shdr_clean(shdr* shader);
void shdr_use(shdr* shader);
void shdr_draw(shdr* shader);

// Implement
shdr shdr_init(
    float* vertecies, unsigned int* indices,
    unsigned int vertecies_count, unsigned int indices_count,
    const char* vert_path, const char* frag_path,
    unsigned int attribute_count, unsigned int* attribute_sizes
) {
    shdr shader;

    shader.vertecies = vertecies;
    shader.indices = indices;
    shader.indices_count = indices_count;
    shader.vertecies_count = vertecies_count;

    // Read shaders code
    string vertex_shader_source_str = rdfl(vert_path);
    const char* vertex_shader_source = vertex_shader_source_str.c_str();
    string fragment_shader_source_str = rdfl(frag_path);
    const char* fragment_shader_source = fragment_shader_source_str.c_str();

    // Create vertex shader
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // Create fragment shader
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Create program
    shader.program = glCreateProgram();

    glAttachShader(shader.program, vertex_shader);
    glAttachShader(shader.program, fragment_shader);
    glLinkProgram(shader.program);
    
    // Clean up fucking shit
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Generate buffers
    glGenBuffers(1, &shader.VBO);
    glGenBuffers(1, &shader.EBO);
    glGenVertexArrays(1, &shader.VAO);

    glBindVertexArray(shader.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shader.VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader.EBO);
    
    glBufferData(GL_ARRAY_BUFFER, vertecies_count * sizeof(float), vertecies, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    int attribute_lenght = 0;
    for (int i = 0; i < attribute_count; i++) {
        attribute_lenght += attribute_sizes[i];
    }

    int offset = 0;
    for (int i = 0; i < attribute_count; i++) {
        glVertexAttribPointer(i, attribute_sizes[i], GL_FLOAT, GL_FALSE, attribute_lenght * sizeof(float), (void*) (offset * sizeof(float)));
        glEnableVertexAttribArray(i);

        offset += attribute_sizes[i];
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return shader;
};

unsigned int shdr_init_texture(const char* path) {

    // Texture preparing
    int texture_width, texture_height, color_channels_number;
    unsigned char* texture_data;

    // Load texture
    texture_data = stbi_load(
        path,
        &texture_width, &texture_height,
        &color_channels_number, 0
    );

    unsigned int texture;
    glGenTextures(1, &texture);
    
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int color_mode;
    if (color_channels_number <= 3) {
        color_mode = GL_RGB;
    } else if (color_channels_number >= 4) {
        color_mode = GL_RGBA;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, color_mode, texture_width, texture_height, 0, color_mode, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    // Texture exparing
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
};

void shdr_set_unformf(shdr* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->program, name), value);
};

void shdr_set_unformi(shdr* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->program, name), value);
};

void shdr_set_unformm4(shdr* shader, const char* name, float* value_pointer) {
    glUniformMatrix4fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, value_pointer);
};

void shdr_set_texture(shdr* shader, const char* name, unsigned int texture, unsigned int id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(shader->program, name), id);
};

void shdr_clean(shdr* shader) {
    glDeleteVertexArrays(1, &shader->VAO);
    glDeleteBuffers(1, &shader->VBO);
    glDeleteBuffers(1, &shader->EBO);
};

void shdr_use(shdr* shader) {
    glUseProgram(shader->program);
    glBindVertexArray(shader->VAO);
};

void shdr_draw(shdr* shader) {
    glDrawElements(GL_TRIANGLES, shader->indices_count, GL_UNSIGNED_INT, 0);
};

#endif
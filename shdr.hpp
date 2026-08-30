#ifndef SHDR
#define SHDR 1

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rdfl.hpp"
#include "objc.hpp"
#include "vxbf.hpp"

using namespace std;
using namespace glm;

typedef struct {
    uint program;
} shdr;

// Define
// TODO: Unite objcs and VAO
shdr shdr_init(const char* vert_path, const char* frag_path);
uint shdr_init_texture(const char* path);

void shdr_set_unformf(shdr* shader, const char* name, float value);
void shdr_set_unformi(shdr* shader, const char* name, int value);
void shdr_set_unformm4(shdr* shader, const char* name, float* value_pointer);
void shdr_set_unformv3(shdr* shader, const char* name, vec3 value);
void shdr_set_unformv4(shdr* shader, const char* name, vec4 value);
void shdr_set_texture(shdr* shader, const char* name, uint texture, uint id);

void shdr_use(shdr* shader);
void shdr_draw(vxbf* vertex_buffer);

// Implement
shdr shdr_init(const char* vert_path, const char* frag_path) {
    shdr shader;

    // Read shaders code
    string vertex_shader_source_str = rdfl(vert_path);
    const char* vertex_shader_source = vertex_shader_source_str.c_str();
    string fragment_shader_source_str = rdfl(frag_path);
    const char* fragment_shader_source = fragment_shader_source_str.c_str();

    // Create vertex shader
    uint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // Create fragment shader
    uint fragment_shader;
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

    return shader;
};

uint shdr_init_texture(const char* path) {
    // Texture preparing
    int texture_width, texture_height, color_channels_number;
    unsigned char* texture_data;

    // Load texture
    texture_data = stbi_load(
        path,
        &texture_width, &texture_height,
        &color_channels_number, 0
    );

    uint texture;
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

void shdr_set_unformv3(shdr* shader, const char* name, vec3 value) {
    glUniform3f(glGetUniformLocation(shader->program, name), value.x, value.y, value.z);
};

void shdr_set_unformv4(shdr* shader, const char* name, vec4 value) {
    glUniform4f(glGetUniformLocation(shader->program, name), value.x, value.y, value.z, value.w);
};

void shdr_set_texture(shdr* shader, const char* name, uint texture, uint id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(shader->program, name), id);
};

void shdr_use(shdr* shader) {
    glUseProgram(shader->program);
};

void shdr_draw(vxbf* vertex_buffer) {
    glDrawElements(GL_TRIANGLES, vertex_buffer->indices_size, GL_UNSIGNED_INT, 0);
};

#endif
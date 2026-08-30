#ifndef VXBF
#define VXBF 1

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rdfl.hpp"
#include "objc.hpp"

using namespace std;
using namespace glm;

typedef struct {
    uint VBO, VAO, EBO;
    float* vertecies;
    uint* indices;
    uint vertecies_size, indices_size;
} vxbf;

// Define
// TODO: Unite objcs and VAO
vxbf vxbf_init(
    float* vertecies, uint* indices,
    uint vertecies_size, uint indices_size,
    uint attribute_size, uint* attribute_sizes
);
vxbf vxbf_init(objc* object);

void vxbf_use(vxbf* vertex_buffer);
void vxbf_clean(vxbf* vertex_buffer);

// Implement
vxbf vxbf_init(
    float* vertecies, uint* indices,
    uint vertecies_size, uint indices_size,
    uint attribute_size, uint* attribute_sizes
) {
    vxbf vertex_buffer;

    vertex_buffer.vertecies = vertecies;
    vertex_buffer.indices = indices;
    vertex_buffer.indices_size = indices_size;
    vertex_buffer.vertecies_size = vertecies_size;

    // Generate buffers
    glGenBuffers(1, &vertex_buffer.VBO);
    glGenBuffers(1, &vertex_buffer.EBO);
    glGenVertexArrays(1, &vertex_buffer.VAO);

    glBindVertexArray(vertex_buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer.EBO);
    
    glBufferData(GL_ARRAY_BUFFER, vertecies_size * sizeof(float), vertecies, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(uint), indices, GL_STATIC_DRAW);

    int attribute_lenght = 0;
    for (int i = 0; i < attribute_size; i++) {
        attribute_lenght += attribute_sizes[i];
    }

    int offset = 0;
    for (int i = 0; i < attribute_size; i++) {
        glVertexAttribPointer(i, attribute_sizes[i], GL_FLOAT, GL_FALSE, attribute_lenght * sizeof(float), (void*) (offset * sizeof(float)));
        glEnableVertexAttribArray(i);

        offset += attribute_sizes[i];
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return vertex_buffer;
};

vxbf vxbf_init(objc* object) {
    uint pos_nor_tex_attribute_sizes[] = {3, 3, 2};

    return vxbf_init(
        objc_pointer((*object).vertecies), objc_pointer((*object).indices),
        (*object).vertecies.size(), (*object).indices.size(),
        3, pos_nor_tex_attribute_sizes
    );
};

void vxbf_use(vxbf* vertex_buffer) {
    glBindVertexArray(vertex_buffer->VAO);
};

void vxbf_clean(vxbf* vertex_buffer) {
    glDeleteVertexArrays(1, &vertex_buffer->VAO);
    glDeleteBuffers(1, &vertex_buffer->VBO);
    glDeleteBuffers(1, &vertex_buffer->EBO);
};

#endif
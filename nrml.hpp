#ifndef NRML
#define NRML 1

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Define
void nrml_generate_normals(
    float* vertecies, uint* indices,
    uint vertecies_size, uint indices_size,
    uint nor_data_start
);

// Implement
void nrml_generate_normals(
    float* vertecies, uint* indices,
    uint vertecies_size, uint indices_size,
    uint vertecies_pos_offset, uint vertecies_nor_offset, uint vertecies_stride
) {
    for (uint i = 0; i < indices_size / 3; i++) {
        uint index[3];
        vec3 pos[3];

        for (uint j = 0; j < 3; j++) {
            index[j] = indices[i * 3 + j] * vertecies_stride + vertecies_pos_offset;
            pos[j] = vec3(vertecies[index[j]], vertecies[index[j] + 1], vertecies[index[j] + 2]);
        }

        vec3 normal = normalize(cross(pos[1] - pos[0], pos[2] - pos[0]));

        for (uint j = 0; j < 3; j++) {
            index[j] = indices[i * 3 + j] * vertecies_stride + vertecies_nor_offset;
            vertecies[index[j]] = normal.x;
            vertecies[index[j] + 1] = normal.y;
            vertecies[index[j] + 2] = normal.z;
        }
    }
};

#endif
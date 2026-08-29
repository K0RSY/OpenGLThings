#include <iostream>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "rdfl.hpp"
#include "shdr.hpp"
#include "cmra.hpp"
#include "scrn.hpp"
#include "nrml.hpp"

#define unfduration std::chrono::nanoseconds
#define second_ratio 1000000000L
#define unftimepoint std::enable_if<true, std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::_V2::system_clock::duration>>::type
#define unftime() std::chrono::time_point_cast<unfduration>(std::chrono::system_clock::now())
#define unftofloat(d) std::chrono::duration<float>(d).count()

using namespace std;
using namespace glm;

int main() {
    int width = 640;
    int height = 480;
    bool run = true;
    auto now_time = unftime();
    auto last_time = unftime();
    float time = 0;
    float swing;
    float delta_time;
    mat4 mouse_dog_model;
    mat4 mouse_dog_normal_model;
    mat4 light_model;
    mat4 view;
    mat4 projection;
    mat4 billboard;
    mat4 skybox;
    vector<SDL_Keycode> pressed_keys {};

    bool weapon_visible = 1;
    bool screen_warp = 1;
    bool lights_visible = 1;

    mat4 light_rotate;
    const uint light_count = 2;
    vec4 lights[] = {
        vec4(3.f, 3.f, 3.f, .2f),
        vec4(-3.f, -3.f, -3.f, .2f),
    };
    vec4 global_light = vec4(normalize(vec3(-1.f, 0.5f, -.9f)), 1.f);
    float ambient_light = .1f;
    
    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Sexy pixels",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        0 | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    gladLoadGL(); 
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Init camera
    cmra camera = cmra_init();

    // Init shaders
    float plane_vertecies[] = {
        1.f,  1.f,  0.f,    1.f,  0.f,
        1.f, -1.f,  0.f,    1.f,  1.f,
       -1.f, -1.f,  0.f,    0.f,  1.f,
       -1.f,  1.f,  0.f,    0.f,  0.f,
    };
    float screen_vertecies[] = {
        1.f,  1.f,  0.f,    1.f,   1.f,
        1.f, -1.f,  0.f,    1.f,   0.f,
       -1.f, -1.f,  0.f,    0.f,   0.f,
       -1.f,  1.f,  0.f,    0.f,   1.f,
    };
    float skybox_vertecies[] = {
        1.f,  1.f,  1.f,    .0f,   .25f,
        1.f, -1.f,  1.f,    .0f,   .5f,
       -1.f, -1.f,  1.f,    .25f,  .5f,
       -1.f,  1.f,  1.f,    .25f,  .25f,

        1.f,  1.f, -1.f,    .75f, .25f,
        1.f, -1.f, -1.f,    .75f, .5f,
       -1.f, -1.f, -1.f,    .5f,  .5f,
       -1.f,  1.f, -1.f,    .5f,  .25f,

        1.f,  1.f,  1.f,    1.f,   .25f,
        1.f, -1.f,  1.f,    1.f,   .5f,
        1.f, -1.f, -1.f,    .75f,  .5f,
        1.f,  1.f, -1.f,    .75f,  .25f,

       -1.f,  1.f,  1.f,    .25f,   .25f,
       -1.f, -1.f,  1.f,    .25f,   .5f,
       -1.f, -1.f, -1.f,    .5f,    .5f,
       -1.f,  1.f, -1.f,    .5f,    .25f,

        1.f,  1.f,  1.f,    .25f, .0f,
       -1.f,  1.f,  1.f,    .25f, .25f,
       -1.f,  1.f, -1.f,    .5f,  .25f,
        1.f,  1.f, -1.f,    .5f,  .0f,

        1.f, -1.f,  1.f,    .25f,  .75f,
       -1.f, -1.f,  1.f,    .25f,  .5f,
       -1.f, -1.f, -1.f,    .5f, .5f,
        1.f, -1.f, -1.f,    .5f, .75f,
    };
    float cube_vertecies[] = {
        1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 0.f,
        1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
       -1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
       -1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

        1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 0.f,
        1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,

        1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 0.f,
        1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
        1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
        1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

       -1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 0.f,
       -1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,

        1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 0.f,
       -1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
        1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

        1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 0.f,
       -1.f, -1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 1.f,
        1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,
    };
    float mouse_dog_vertecies[] = {
        0.0625f, 0.29375f, -0.19375f, 0.0f, 0.9778024140774096f, 0.20952908873087359f, 0.059579375f, 0.4736175f, -0.0625f, 0.29375f, -0.19375f, 0.0f, 0.9778024140774096f, 0.20952908873087359f, 0.002920625f, 0.5f, 0.0f, 0.265625f, -0.0625f, 0.0f, 0.9778024140774096f, 0.20952908873087359f, 0.002920625f, 0.4259684375f, -0.084375f, 0.159375f, -0.19375f, 0.0f, -0.7772448707074855f, 0.6291982286679645f, 0.2825734375f, 0.5924f, 0.084375f, 0.159375f, -0.19375f, 0.0f, -0.7772448707074855f, 0.6291982286679645f, 0.3580515625f, 0.5546875f, 0.0f, 0.265625f, -0.0625f, 0.0f, -0.7772448707074855f, 0.6291982286679645f, 0.3580515625f, 0.6490746875f, 0.0625f, 0.29375f, -0.19375f, 0.0f, 0.0f, -1.0f, 0.41796875f, 0.71875f, 0.084375f, 0.159375f, -0.19375f, 0.0f, 0.0f, -1.0f, 0.40703125f, 0.65625f, -0.084375f, 0.159375f, -0.19375f, 0.0f, 0.0f, -1.0f, 0.49140625f, 0.65625f, -0.0625f, 0.29375f, -0.19375f, 0.0f, 0.0f, -1.0f, 0.48046875f, 0.71875f, 0.0625f, 0.29375f, -0.19375f, 0.8812376609632864f, 0.14345729364518617f, 0.45037783004934295f, 0.4609375f, 0.47957031250000004f, 0.0f, 0.265625f, -0.0625f, 0.8812376609632864f, 0.14345729364518617f, 0.45037783004934295f, 0.4079275f, 0.53125f, 0.084375f, 0.159375f, -0.19375f, 0.8812376609632864f, 0.14345729364518617f, 0.45037783004934295f, 0.4079275f, 0.43686375f, 0.0f, 0.265625f, -0.0625f, -0.8812376609632864f, 0.14345729364518617f, 0.4503778300493429f, 0.50063625f, 0.57199f, -0.0625f, 0.29375f, -0.19375f, -0.8812376609632864f, 0.14345729364518617f, 0.4503778300493429f, 0.4489565625f, 0.625f, -0.084375f, 0.159375f, -0.19375f, -0.8812376609632864f, 0.14345729364518617f, 0.4503778300493429f, 0.40625f, 0.57199f, 0.10625f, 0.09687500000000002f, -0.1625f, -0.5070925528371099f, -0.16903085094570328f, 0.8451542547285166f, 0.40625f, 0.8984375f, 0.16875f, 0.22187500000000002f, -0.1f, -0.5070925528371099f, -0.16903085094570328f, 0.8451542547285166f, 0.4647103125f, 0.9478453125f, 0.04375f, 0.28437500000000004f, -0.1625f, -0.5070925528371099f, -0.16903085094570328f, 0.8451542547285166f, 0.40625f, 0.9972559375f, -0.16875f, 0.22187500000000002f, -0.1f, 0.50709255283711f, -0.16903085094570328f, 0.8451542547285166f, 0.145901875f, 0.5897103125000001f, -0.10625f, 0.09687500000000002f, -0.1625f, 0.50709255283711f, -0.16903085094570328f, 0.8451542547285166f, 0.0964940625f, 0.53125f, -0.04375f, 0.28437500000000004f, -0.1625f, 0.50709255283711f, -0.16903085094570328f, 0.8451542547285166f, 0.1953125f, 0.53125f, 0.09375f, 0.0031250000000000028f, 0.275f, 0.919145030018058f, 0.39391929857916774f, 0.0f, 0.0f, 0.875f, 0.09375f, 0.0031250000000000028f, -0.225f, 0.919145030018058f, 0.39391929857916774f, 0.0f, 0.25f, 0.875f, 0.0f, 0.221875f, -0.15f, 0.919145030018058f, 0.39391929857916774f, 0.0f, 0.2125f, 1.0f, 0.0f, 0.140625f, 0.275f, 0.919145030018058f, 0.39391929857916774f, 0.0f, 0.0f, 0.962659375f, -0.09375f, 0.0031250000000000028f, 0.275f, 0.0f, 0.0f, 1.0f, 0.375f, 0.775f, 0.09375f, 0.0031250000000000028f, 0.275f, 0.0f, 0.0f, 1.0f, 0.46875f, 0.775f, 0.0f, 0.140625f, 0.275f, 0.0f, 0.0f, 1.0f, 0.421875f, 0.84375f, 0.0f, 0.221875f, -0.15f, 0.0f, 0.3243243243243244f, -0.9459459459459459f, 0.327238125f, 0.984375f, 0.09375f, 0.0031250000000000028f, -0.225f, 0.0f, 0.3243243243243244f, -0.9459459459459459f, 0.27997625f, 0.8743925f, -0.09375f, 0.0031250000000000028f, -0.225f, 0.0f, 0.3243243243243244f, -0.9459459459459459f, 0.37372625f, 0.8743925f, 0.0f, 0.221875f, -0.15f, -0.919145030018058f, 0.39391929857916774f, 0.0f, 0.0375f, 0.84375f, -0.09375f, 0.0031250000000000028f, -0.225f, -0.919145030018058f, 0.39391929857916774f, 0.0f, 0.0f, 0.71875f, -0.09375f, 0.0031250000000000028f, 0.275f, -0.919145030018058f, 0.39391929857916774f, 0.0f, 0.25f, 0.71875f, 0.0f, 0.140625f, 0.275f, -0.919145030018058f, 0.39391929857916774f, 0.0f, 0.25f, 0.806409375f, 0.0f, 0.078125f, 0.2125f, 1.0f, 0.0f, 0.0f, 0.0579234375f, 0.28147999999999995f, 0.0f, 0.265625f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0579234375f, 0.4140625f, 0.0f, 0.078125f, 0.275f, 1.0f, 0.0f, 0.0f, 0.0358265625f, 0.30357718749999996f, 0.03125f, 0.046875f, 0.290625f, 0.0f, -0.9284766908852594f, -0.3713906763541037f, 0.1416025f, 0.42611406249999995f, -0.03125f, 0.046875f, 0.290625f, 0.0f, -0.9284766908852594f, -0.3713906763541037f, 0.1708975f, 0.41523437500000004f, 0.0f, 0.078125f, 0.2125f, 0.0f, -0.9284766908852594f, -0.3713906763541037f, 0.1708975f, 0.4601140625f, 0.03125f, 0.046875f, 0.290625f, 0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.203125f, 0.44117281249999996f, 0.0f, 0.078125f, 0.2125f, 0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.248005625f, 0.44117281249999996f, 0.0f, 0.09375f, 0.275f, 0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.22352875f, 0.46210937500000004f, -0.03125f, 0.046875f, 0.290625f, 0.0f, 0.316227766016838f, 0.948683298050514f, 0.140625f, 0.343653125f, 0.03125f, 0.046875f, 0.290625f, 0.0f, 0.316227766016838f, 0.948683298050514f, 0.171875f, 0.343653125f, 0.0f, 0.09375f, 0.275f, 0.0f, 0.316227766016838f, 0.948683298050514f, 0.15625f, 0.36835937500000004f, 0.0f, 0.078125f, 0.2125f, -0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.2056259375f, 0.39960937500000004f, -0.03125f, 0.046875f, 0.290625f, -0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.2056259375f, 0.35472875000000004f, 0.0f, 0.09375f, 0.275f, -0.8444407432001202f, 0.5196558419693047f, -0.12991396049232617f, 0.2265625f, 0.3751325f, 0.0f, 0.265625f, 0.4f, -1.0f, 0.0f, 0.0f, 0.0579234375f, 0.4140625f, 0.0f, 0.078125f, 0.2125f, -1.0f, 0.0f, 0.0f, 0.0579234375f, 0.28147999999999995f, 0.0f, 0.078125f, 0.275f, -1.0f, 0.0f, 0.0f, 0.0358265625f, 0.30357718749999996f, -0.0625f, 0.140625f, -0.33125000000000004f, 0.0f, -0.9461639849533561f, 0.3236876790629904f, 0.2821759375f, 0.7295128125f, 0.0625f, 0.140625f, -0.33125000000000004f, 0.0f, -0.9461639849533561f, 0.3236876790629904f, 0.3428240625f, 0.7144118749999999f, 0.0f, 0.22187500000000004f, -0.09375f, 0.0f, -0.9461639849533561f, 0.3236876790629904f, 0.3428240625f, 0.84375f, 0.0625f, 0.140625f, -0.33125000000000004f, 0.0f, 0.0f, -1.0f, 0.5f, 0.946875f, -0.0625f, 0.140625f, -0.33125000000000004f, 0.0f, 0.0f, -1.0f, 0.5625f, 0.946875f, 0.0f, 0.24687499999999996f, -0.33125000000000004f, 0.0f, 0.0f, -1.0f, 0.53125f, 1.0f, -0.0625f, 0.140625f, -0.33125000000000004f, -0.8607092584669869f, 0.5062995638041101f, 0.05329469092674823f, 0.097221875f, 0.625f, 0.0f, 0.22187500000000004f, -0.09375f, -0.8607092584669869f, 0.5062995638041101f, 0.05329469092674823f, 0.2265625f, 0.625f, 0.0f, 0.24687499999999996f, -0.33125000000000004f, -0.8607092584669869f, 0.5062995638041101f, 0.05329469092674823f, 0.121458125f, 0.6816681250000001f, 0.0f, 0.22187500000000004f, -0.09375f, 0.8607092584669868f, 0.5062995638041101f, 0.05329469092674824f, 0.0f, 0.684028125f, 0.0625f, 0.140625f, -0.33125000000000004f, 0.8607092584669868f, 0.5062995638041101f, 0.05329469092674824f, 0.0f, 0.5546875f, 0.0f, 0.24687499999999996f, -0.33125000000000004f, 0.8607092584669868f, 0.5062995638041101f, 0.05329469092674824f, 0.056668125f, 0.57892375f, -0.10624999999999996f, 0.09687500000000002f, -0.16249999999999998f, -0.5070925528371101f, 0.1690308509457033f, -0.8451542547285167f, 0.40625f, 0.8984375f, -0.16874999999999996f, 0.22187500000000002f, -0.09999999999999998f, -0.5070925528371101f, 0.1690308509457033f, -0.8451542547285167f, 0.4647103125f, 0.9478453125f, -0.043749999999999956f, 0.28437500000000004f, -0.16249999999999998f, -0.5070925528371101f, 0.1690308509457033f, -0.8451542547285167f, 0.40625f, 0.9972559375f, 0.16875000000000007f, 0.22187500000000002f, -0.09999999999999998f, 0.5070925528371099f, 0.16903085094570328f, -0.8451542547285167f, 0.145901875f, 0.5897103125000001f, 0.10625000000000004f, 0.09687500000000002f, -0.16249999999999998f, 0.5070925528371099f, 0.16903085094570328f, -0.8451542547285167f, 0.0964940625f, 0.53125f, 0.04375000000000004f, 0.28437500000000004f, -0.16249999999999998f, 0.5070925528371099f, 0.16903085094570328f, -0.8451542547285167f, 0.1953125f, 0.53125f
    };
    uint plane_indices[] = {
        1, 3, 2,
        1, 0, 3,
    };
    uint mouse_dog_indices[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 7, 9, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 23, 25, 22, 26, 27, 28, 29, 30, 31, 33, 34, 35, 33, 35, 32, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71
    };
    uint cube_indices[] = {
        1, 3, 2,
        1, 0, 3,

        7, 5, 6,
        5, 7, 4,

        11, 9, 10,
        9, 11, 8,

        13, 15, 14,
        15, 13, 12,

        17, 19, 18,
        19, 17, 16,

        23, 21, 22,
        21, 23, 20,
    };

    nrml_generate_normals(
        cube_vertecies, cube_indices,
        sizeof(cube_vertecies) / sizeof(float), sizeof(cube_indices) / sizeof(uint),
        0, 3, 8
    );

    uint pos_nor_tex_attribute_sizes[] = {3, 3, 2};
    uint pos_tex_attribute_sizes[] = {3, 2};

    scrn screen = scrn_init(width, height);

    shdr mouse_dog_shader = shdr_init(
        mouse_dog_vertecies, mouse_dog_indices,
        sizeof(mouse_dog_vertecies) / sizeof(float), sizeof(mouse_dog_indices) / sizeof(uint),
        "shaders/mouse_dog.vert", "shaders/mouse_dog.frag",
        3, pos_nor_tex_attribute_sizes
    );

    shdr light_shader = shdr_init(
        plane_vertecies, plane_indices,
        sizeof(plane_vertecies) / sizeof(float), sizeof(plane_indices) / sizeof(uint),
        "shaders/light.vert", "shaders/light.frag",
        2, pos_tex_attribute_sizes
    );

    shdr weapon_shader = shdr_init(
        plane_vertecies, plane_indices,
        sizeof(plane_vertecies) / sizeof(float), sizeof(plane_indices) / sizeof(uint),
        "shaders/weapon.vert", "shaders/weapon.frag",
        2, pos_tex_attribute_sizes
    );

    shdr screen_shader = shdr_init(
        screen_vertecies, plane_indices,
        sizeof(screen_vertecies) / sizeof(float), sizeof(plane_indices) / sizeof(uint),
        "shaders/screen.vert", "shaders/screen.frag",
        2, pos_tex_attribute_sizes
    );

    shdr skybox_shader = shdr_init(
        skybox_vertecies, cube_indices,
        sizeof(skybox_vertecies) / sizeof(float), sizeof(cube_indices) / sizeof(uint),
        "shaders/skybox.vert", "shaders/skybox.frag",
        2, pos_tex_attribute_sizes
    );

    uint mouse_dog_texture = shdr_init_texture("assets/mouse_dog.png");
    uint noise_texture = shdr_init_texture("assets/noise.png");
    uint skybox_texture = shdr_init_texture("assets/skyboxc.png");
    uint weapon_texture = shdr_init_texture("assets/weapon.png");
    uint light_texture = shdr_init_texture("assets/light.png");

    // Static calculations
    mouse_dog_model = mat4(1.f);
    mouse_dog_model = scale(mouse_dog_model, vec3(8.f, 8.f, 8.f));
    mouse_dog_model = translate(mouse_dog_model, vec3(0.f, -.1f, 0.f));

    SDL_Event event;
    while (run) {
        // Other things
        now_time = unftime();
        delta_time = unftofloat(now_time - last_time);
        time += delta_time;

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) run = false;

            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;
                // scrn_set_dimentions(&screen, width, height);
            }

            else if (event.type == SDL_KEYDOWN) {
                cmra_process_keypresses(&camera, event.key.keysym.sym, delta_time);

                if (count(pressed_keys.begin(), pressed_keys.end(), event.key.keysym.sym) == 0)
                    pressed_keys.push_back(event.key.keysym.sym);
            }

            else if (event.type == SDL_KEYUP) {
                auto key_location = find(pressed_keys.begin(), pressed_keys.end(), event.key.keysym.sym);

                switch (event.key.keysym.sym) {
                    case SDLK_F1:
                        weapon_visible = !weapon_visible;
                        break;

                    case SDLK_F2:
                        screen_warp = !screen_warp;
                        break;

                    case SDLK_F3:
                        lights_visible = !lights_visible;
                        break;
                    
                    default:
                        break;
                }

                if (key_location != pressed_keys.end())
                    pressed_keys.erase(key_location);
            }

            if (event.type == SDL_MOUSEWHEEL) {
                cmra_process_scroll(&camera, event.wheel.y);
            }
        }

        for (auto key : pressed_keys) {
            cmra_process_keydowns(&camera, key, delta_time);
        }

        cmra_process_tick(&camera, delta_time, window, width, height);
        
        view = cmra_get_view(&camera);
        skybox = cmra_get_skybox(&camera);
        billboard = cmra_get_billboard(&camera, skybox);

        mouse_dog_model = rotate(mouse_dog_model, radians(80 * delta_time), vec3(0.f, 1.f, 0.f));
        mouse_dog_normal_model = transpose(inverse(mouse_dog_model));
        
        projection = cmra_get_projection(&camera, ((float) width) / ((float) height));

        // Draw on screen
        scrn_use(&screen);

        glClearColor(.01, .01, .01, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw skybox
        glCullFace(GL_FRONT);

        shdr_use(&skybox_shader);

        shdr_set_texture(&skybox_shader, "texture_data", skybox_texture, 0);
        shdr_set_texture(&skybox_shader, "texture_datb", noise_texture, 1);

        shdr_set_unformf(&skybox_shader, "time", time);
        
        shdr_set_unformm4(&skybox_shader, "view", value_ptr(skybox));
        shdr_set_unformm4(&skybox_shader, "projection", value_ptr(projection));
        
        shdr_draw(&skybox_shader);

        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw world objects
        // Mouse dog
        glCullFace(GL_BACK);

        shdr_use(&mouse_dog_shader);

        shdr_set_texture(&mouse_dog_shader, "texture_data", mouse_dog_texture, 0);
        shdr_set_texture(&mouse_dog_shader, "texture_datb", noise_texture, 1);

        shdr_set_unformf(&mouse_dog_shader, "time", time);
        shdr_set_unformf(&mouse_dog_shader, "ambient_light", ambient_light);
        
        shdr_set_unformv4(&mouse_dog_shader, "global_light", global_light);
        for (uint i = 0; i < light_count; i++) {
            char stri[16];
            sprintf(stri, "lights[%d]", i);
            shdr_set_unformv4(&mouse_dog_shader, stri, lights[i]);
        }

        shdr_set_unformm4(&mouse_dog_shader, "model", value_ptr(mouse_dog_model));
        shdr_set_unformm4(&mouse_dog_shader, "normal_model", value_ptr(mouse_dog_normal_model));
        shdr_set_unformm4(&mouse_dog_shader, "view", value_ptr(view));
        shdr_set_unformm4(&mouse_dog_shader, "projection", value_ptr(projection));
        
        shdr_draw(&mouse_dog_shader);

        // Light
        light_rotate = rotate(mat4(1.f), radians(30 * delta_time), vec3(0.f, 1.f, 0.f));

        for (uint i = 0; i < light_count; i++) {
            lights[i] = light_rotate * lights[i];
        }

        if (lights_visible) {
            shdr_use(&light_shader);

            shdr_set_texture(&light_shader, "texture_data", light_texture, 0);
            
            shdr_set_unformm4(&light_shader, "view", value_ptr(view));
            shdr_set_unformm4(&light_shader, "projection", value_ptr(projection));

            
            for (uint i = 0; i < light_count; i++) {
                light_model = mat4(1.f);
                light_model = translate(light_model, vec3(lights[i]));
                light_model = scale(light_model, vec3(.5f));
                light_model = light_model * billboard;

                shdr_set_unformm4(&light_shader, "model", value_ptr(light_model));

                shdr_draw(&light_shader);
            }
        }
        
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw weapon
        if (weapon_visible) {
            shdr_use(&weapon_shader);

            shdr_set_texture(&weapon_shader, "texture_data", weapon_texture, 0);

            float velocity = length(camera.acceleration);
            swing += velocity * delta_time;
            float sin_time_y = (sin(time - swing) + 1) * velocity / 80.f;
            float sin_time_x = sin((time - swing) / 2) * velocity / 80.f;
            shdr_set_unformf(&weapon_shader, "sin_time_y", sin_time_y);
            shdr_set_unformf(&weapon_shader, "sin_time_x", sin_time_x);

            shdr_draw(&weapon_shader);
        }

        // Draw in window
        scrn_stop(width, height);

        glClearColor(.01, .01, .01, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shdr_use(&screen_shader);

        shdr_set_texture(&screen_shader, "texture_data", screen.texture, 0);

        shdr_set_unformf(&screen_shader, "crt", screen_warp * (.05f + (1 - (camera.fov / 90.f)) / 8.f));
        // shdr_set_unformf(&screen_shader, "crt", 0.f);

        shdr_draw(&screen_shader);

        SDL_GL_SwapWindow(window);

        // Fuck
        last_time = now_time;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    shdr_clean(&mouse_dog_shader);
    shdr_clean(&weapon_shader);
    shdr_clean(&skybox_shader);
    shdr_clean(&light_shader);
    scrn_clean(&screen);

    return 0;
}
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
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 billboard;
    mat4 skybox;
    vector<SDL_Keycode> pressed_keys {};
    
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

    // Init shader
    float weapon_vertecies[] = {
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
    float mouse_dog_vertecies[] = {
        1.f,  1.f,  1.f,    1.f, 0.f, 0.f,    1.f, 0.f,
        1.f, -1.f,  1.f,    0.f, 1.f, 0.f,    1.f, 1.f,
       -1.f, -1.f,  1.f,    0.f, 0.f, 1.f,    0.f, 1.f,
       -1.f,  1.f,  1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

        1.f,  1.f, -1.f,    1.f, 0.f, 0.f,    0.f, 0.f,
        1.f, -1.f, -1.f,    0.f, 1.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 1.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,

        1.f,  1.f,  1.f,    1.f, 0.f, 0.f,    1.f, 0.f,
        1.f, -1.f,  1.f,    0.f, 1.f, 0.f,    1.f, 1.f,
        1.f, -1.f, -1.f,    0.f, 0.f, 1.f,    0.f, 1.f,
        1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

       -1.f,  1.f,  1.f,    1.f, 0.f, 0.f,    0.f, 0.f,
       -1.f, -1.f,  1.f,    0.f, 1.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 1.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,

        1.f,  1.f,  1.f,    1.f, 0.f, 0.f,    1.f, 0.f,
       -1.f,  1.f,  1.f,    0.f, 1.f, 0.f,    1.f, 1.f,
       -1.f,  1.f, -1.f,    0.f, 0.f, 1.f,    0.f, 1.f,
        1.f,  1.f, -1.f,    0.f, 0.f, 0.f,    0.f, 0.f,

        1.f, -1.f,  1.f,    1.f, 0.f, 0.f,    0.f, 0.f,
       -1.f, -1.f,  1.f,    0.f, 1.f, 0.f,    0.f, 1.f,
       -1.f, -1.f, -1.f,    0.f, 0.f, 1.f,    1.f, 1.f,
        1.f, -1.f, -1.f,    0.f, 0.f, 0.f,    1.f, 0.f,
    };
    unsigned int weapon_indices[] = {
        1, 3, 2,
        1, 0, 3,
    };
    unsigned int screen_indices[] = {
        1, 3, 2,
        1, 0, 3,
    };
    unsigned int mouse_dog_indices[] = {
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

    unsigned int mouse_dog_attribute_sizes[] = {3, 3, 2};
    unsigned int skybox_attribute_sizes[] = {3, 2};
    unsigned int weapon_attribute_sizes[] = {3, 2};
    unsigned int screen_attribute_sizes[] = {3, 2};

    scrn screen = scrn_init(width, height);

    shdr mouse_dog_shader = shdr_init(
        mouse_dog_vertecies, mouse_dog_indices,
        sizeof(mouse_dog_vertecies) / sizeof(float), sizeof(mouse_dog_indices) / sizeof(unsigned int),
        "shaders/mouse_dog.vert", "shaders/mouse_dog.frag",
        3, mouse_dog_attribute_sizes
    );

    shdr weapon_shader = shdr_init(
        weapon_vertecies, weapon_indices,
        sizeof(weapon_vertecies) / sizeof(float), sizeof(weapon_indices) / sizeof(unsigned int),
        "shaders/weapon.vert", "shaders/weapon.frag",
        2, weapon_attribute_sizes
    );

    shdr screen_shader = shdr_init(
        screen_vertecies, screen_indices,
        sizeof(screen_vertecies) / sizeof(float), sizeof(screen_indices) / sizeof(unsigned int),
        "shaders/screen.vert", "shaders/screen.frag",
        2, screen_attribute_sizes
    );

    shdr skybox_shader = shdr_init(
        skybox_vertecies, mouse_dog_indices,
        sizeof(skybox_vertecies) / sizeof(float), sizeof(mouse_dog_indices) / sizeof(unsigned int),
        "shaders/skybox.vert", "shaders/skybox.frag",
        2, skybox_attribute_sizes
    );

    unsigned int mouse_dog_texture = shdr_init_texture("assets/mouse_dog.png");
    unsigned int noise_texture = shdr_init_texture("assets/noise.png");
    unsigned int skybox_texture = shdr_init_texture("assets/skyboxc.png");
    unsigned int weapon_texture = shdr_init_texture("assets/weapon.png");

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

        model = mat4(1.f);
        model = model;
        
        view = cmra_get_view(&camera);
        skybox = cmra_get_skybox(&camera);
        billboard = cmra_get_billboard(&camera, view, skybox);

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

        // Draw mouse_dog
        glCullFace(GL_BACK);

        shdr_use(&mouse_dog_shader);

        shdr_set_texture(&mouse_dog_shader, "texture_data", mouse_dog_texture, 0);
        shdr_set_texture(&mouse_dog_shader, "texture_datb", noise_texture, 1);

        shdr_set_unformf(&mouse_dog_shader, "time", time);
        
        shdr_set_unformm4(&mouse_dog_shader, "model", value_ptr(model));
        shdr_set_unformm4(&mouse_dog_shader, "view", value_ptr(view));
        shdr_set_unformm4(&mouse_dog_shader, "projection", value_ptr(projection));
        
        shdr_draw(&mouse_dog_shader);
        
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw weapon
        shdr_use(&weapon_shader);

        shdr_set_texture(&weapon_shader, "texture_data", weapon_texture, 0);

        float velocity = length(camera.acceleration);
        swing += velocity * delta_time;
        float sin_time_y = (sin(time - swing) + 1) * velocity / 80.f;
        float sin_time_x = sin((time - swing) / 2) * velocity / 80.f;
        shdr_set_unformf(&weapon_shader, "sin_time_y", sin_time_y);
        shdr_set_unformf(&weapon_shader, "sin_time_x", sin_time_x);

        shdr_draw(&weapon_shader);

        // Draw in window
        scrn_stop(width, height);

        glClearColor(.01, .01, .01, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shdr_use(&screen_shader);

        shdr_set_texture(&screen_shader, "texture_data", screen.texture, 0);

        shdr_set_unformf(&screen_shader, "crt", .05f + (1 - (camera.fov / 90.f)) / 8.f);
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
    scrn_clean(&screen);

    return 0;
}
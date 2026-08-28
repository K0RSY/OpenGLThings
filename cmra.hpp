#ifndef CMRA
#define CMRA 1

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

typedef struct {
    short int lock_cursor;
    vec3 position;
    vec3 acceleration;
    vec3 up;
    vec3 front;
    vec3 right;

    float fov;
    const float acceleration_speed;
    const float fov_speed;
    const float slowdown_coefficient;
    const float mouse_sensitivity;
    const float buttons_sensitivity;
} cmra;

// Define
cmra cmra_init(
    vec3 position,
    vec3 up,
    vec3 front,

    float fov,
    const float acceleration_speed,
    const float fov_speed,
    const float slowdown_coefficient,
    const float mouse_sensitivity,
    const float buttons_sensitivity
);

void cmra_process_keydowns(cmra* camera, SDL_Keycode key, float delta_time);
void cmra_process_keypresses(cmra* camera, SDL_Keycode key, float delta_time);
void cmra_process_scroll(cmra* camera, int scroll_amount);
void cmra_process_tick(cmra* camera, float delta_time, SDL_Window* window, int width, int height);

void cmra_accelerate(cmra* camera, vec3 direction, float delta_time, float coefficient);
void cmra_rotate(vec3* rotating_a, vec3* rotating_b, vec3 origin, float angle, float delta_time, float angle_coefficient, float cross_coefficient);

mat4 cmra_get_view(cmra* camera);
mat4 cmra_get_billboard(cmra* camera, mat4 view, mat4 skybox);
mat4 cmra_get_skybox(cmra* camera);    
mat4 cmra_get_projection(cmra* camera, float ratio);

// Implement
cmra cmra_init(
    vec3 position = vec3(0.f),
    vec3 up = vec3(0.f, 1.f, 0.f),
    vec3 front = vec3(0.f, 0.f, -1.f),

    float fov = 90.f,
    const float acceleration_speed = 8.f,
    const float fov_speed = 2.f,
    const float slowdown_coefficient = .5f,
    const float mouse_sensitivity = 0.1f,
    const float buttons_sensitivity = 120.f
) {
    cmra camera {
        true, position, vec3(0.f), up, front, cross(front, up),
        fov, acceleration_speed, fov_speed, slowdown_coefficient, mouse_sensitivity, buttons_sensitivity
    };

    SDL_ShowCursor(false);

    return camera;
};

void cmra_process_keydowns(cmra* camera, SDL_Keycode key, float delta_time) {
    switch (key) {
        case SDLK_w:
            cmra_accelerate(camera, camera->front, delta_time, 1.f);
            break;

        case SDLK_s:
            cmra_accelerate(camera, camera->front, delta_time, -1.f);
            break;

        case SDLK_d:
            cmra_accelerate(camera, camera->right, delta_time, 1.f);
            break;

        case SDLK_a:
            cmra_accelerate(camera, camera->right, delta_time, -1.f);
            break;

        case SDLK_SPACE:
            cmra_accelerate(camera, camera->up, delta_time, 1.f);
            break;

        case SDLK_LSHIFT:
            cmra_accelerate(camera, camera->up, delta_time, -1.f);
            break;

        case SDLK_UP:
            cmra_rotate(&(camera->up), &(camera->front), camera->right, camera->buttons_sensitivity, delta_time, -1.f, 1.f);
            break;

        case SDLK_DOWN:
            cmra_rotate(&(camera->up), &(camera->front), camera->right, camera->buttons_sensitivity, delta_time, 1.f, 1.f);
            break;

        case SDLK_LEFT:
            cmra_rotate(&(camera->right), &(camera->front), camera->up, camera->buttons_sensitivity, delta_time, -1.f, -1.f);
            break;

        case SDLK_RIGHT:
            cmra_rotate(&(camera->right), &(camera->front), camera->up, camera->buttons_sensitivity, delta_time, 1.f, -1.f);
            break;

        case SDLK_e:
            cmra_rotate(&(camera->right), &(camera->up), camera->front, camera->buttons_sensitivity, delta_time, -1.f, 1.f);
            break;

        case SDLK_q:
            cmra_rotate(&(camera->right), &(camera->up), camera->front, camera->buttons_sensitivity, delta_time, 1.f, 1.f);
            break;
        
        default:
            break;
    }
};

void cmra_process_keypresses(cmra* camera, SDL_Keycode key, float delta_time) {
    if (key == SDLK_ESCAPE) {
        SDL_ShowCursor(camera->lock_cursor);
        camera->lock_cursor = !camera->lock_cursor;
    }
};

void cmra_process_scroll(cmra* camera, int scroll_amount) {
    camera->fov += -scroll_amount * camera->fov_speed;
};

void cmra_process_tick(cmra* camera, float delta_time, SDL_Window* window, int width, int height) {
    if (camera->lock_cursor == 1) {
        SDL_WarpMouseInWindow(window, width / 2, height / 2);
        camera->lock_cursor = 2;
    } else if (camera->lock_cursor) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        mouse_x = (mouse_x - width / 2);
        mouse_y = (mouse_y - height / 2);

        camera->up = vec4(camera->up, 1.f) * rotate(mat4(1.f), radians((float) mouse_y) * camera->mouse_sensitivity, camera->right);
        camera->right = vec4(camera->right, 1.f) * rotate(mat4(1.f), radians((float) mouse_x) * camera->mouse_sensitivity, camera->up);
        camera->front = cross(camera->up, camera->right);

        SDL_WarpMouseInWindow(window, width / 2, height / 2);
    }

    camera->position += camera->acceleration * delta_time;

    camera->acceleration = camera->acceleration * pow(camera->slowdown_coefficient, delta_time);
};

void cmra_accelerate(cmra* camera, vec3 direction, float delta_time, float coefficient) {
    camera->acceleration += direction * delta_time * camera->acceleration_speed * coefficient;
};

void cmra_rotate(vec3* rotating_a, vec3* rotating_b, vec3 origin, float angle, float delta_time, float angle_coefficient, float cross_coefficient) {
    *rotating_a = vec4(*rotating_a, 1.f) * rotate(mat4(1.f), angle_coefficient * radians(angle) * delta_time, origin);
    *rotating_b = cross(*rotating_a, origin) * cross_coefficient;
};

mat4 cmra_get_view(cmra* camera) {
    return lookAt(camera->position, camera->position + camera->front, camera->up);
};

mat4 cmra_get_billboard(cmra* camera, mat4 skybox) {
    return inverse(skybox);
};

mat4 cmra_get_skybox(cmra* camera) {
    return lookAt(vec3(0.f), camera->front, camera->up);
};

mat4 cmra_get_projection(cmra* camera, float ratio) {
    return perspective(radians(camera->fov), ratio, .1f, 100.f);
};

#endif
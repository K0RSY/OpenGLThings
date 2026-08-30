#ifndef SCRN
#define SCRN 1

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

typedef struct {
    uint FBO, depth_texture, color_texture;
    int width, height;
} scrn;

// Define
scrn scrn_init(int width, int height);

void scrn_clean(scrn* screen);
void scrn_use(scrn* screen);
void scrn_stop(int width, int height);
void scrn_set_dimentions(scrn* screen, int width, int height);

// Implement
scrn scrn_init(int width, int height) {
    scrn screen {.width=width, .height=height};
    const float border_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

    // Generate FBO
    glGenFramebuffers(1, &screen.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screen.FBO);

    // Generate color texture
    glGenTextures(1, &screen.color_texture);
    glBindTexture(GL_TEXTURE_2D, screen.color_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen.color_texture, 0);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate depth texture
    glGenTextures(1, &screen.depth_texture);
    glBindTexture(GL_TEXTURE_2D, screen.depth_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screen.width, screen.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, screen.depth_texture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return screen;
};

void scrn_clean(scrn* screen) {
    glDeleteFramebuffers(1, &screen->FBO);
};

void scrn_use(scrn* screen) {
    glBindFramebuffer(GL_FRAMEBUFFER, screen->FBO);
    glViewport(0, 0, screen->width, screen->height);
};

void scrn_stop(int width, int height) {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void scrn_set_dimentions(scrn* screen, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, screen->color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, screen->depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    screen->width = width;
    screen->height = height;
};

#endif
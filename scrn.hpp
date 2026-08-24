#ifndef SCRN
#define SCRN 1

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

typedef struct {
    unsigned int FBO, RBO, texture;
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

    // Generate FBO
    glGenFramebuffers(1, &screen.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screen.FBO);

    // Generate texture
    glGenTextures(1, &screen.texture);
    glBindTexture(GL_TEXTURE_2D, screen.texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    float border_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen.texture, 0);

    // Generate RBO
    glGenRenderbuffers(1, &screen.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screen.RBO);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen.width, screen.height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screen.RBO);

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
    glBindTexture(GL_TEXTURE_2D, screen->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, screen->RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    screen->width = width;
    screen->height = height;
};

#endif
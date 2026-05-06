#include "../include/billiard.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

GLuint loadTexture(const char* f) {
    GLuint t; glGenTextures(1, &t); glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int w, h, c; unsigned char *d = stbi_load(f, &w, &h, &c, 0);
    if (d) {
        GLenum fmt = (c == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, d);
        stbi_image_free(d);
    }
    return t;
}

int main() {
    glfwInit();
    GameState state = {0}; initGame(&state);
    state.camY=2; state.camZ=5; state.yaw=-90; state.pitch=-20; state.lightBrightness=1;

    GLFWwindow* w = glfwCreateWindow(1024, 768, "Billiard Pro", NULL, NULL);
    glfwMakeContextCurrent(w); glfwSetWindowUserPointer(w, &state);
    glfwSetCursorPosCallback(w, mouse_callback); glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glMatrixMode(GL_PROJECTION); gluPerspective(45, 1024.0/768.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW); glEnable(GL_DEPTH_TEST);

    state.wallTex = loadTexture("wall.jpg"); state.carpetTex = loadTexture("carpet.jpg"); state.clothTex = loadTexture("cloth.jpg");

    while (!glfwWindowShouldClose(w)) {
        static float lt = 0; float now = glfwGetTime(); state.deltaTime = now - lt; lt = now;
        processInput(w, &state); updatePhysics(&state); drawScene(&state);
        glfwSwapBuffers(w); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}

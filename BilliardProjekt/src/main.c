#include "billiard.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 


GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        printf("Sikeres textúra betöltés: %s\n", filename);
    } else {
        printf("HIBA: Nem sikerült betölteni a textúrát: %s\n", filename);
    }
    
    return textureID;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "HIBA: GLFW inicializálása sikertelen!\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Billiard Pro - Made by Skafyy", NULL, NULL);
    if (!window) {
        fprintf(stderr, "HIBA: Nem sikerült létrehozni az ablakot!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    GameState state = {0};
    game_init(&state); 


    state.camera.position = (Vec3){0.0f, 2.5f, 5.0f};
    state.camera.yaw = -90.0f;
    state.camera.pitch = -30.0f;
    state.deltaTime = 0.0f;

    glfwSetWindowUserPointer(window, &state);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1024.0 / 768.0, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    state.textures[0] = loadTexture("assets/cloth.jpg"); 
    state.textures[1] = loadTexture("assets/carpet.jpg"); 
    state.textures[2] = loadTexture("assets/wall.jpg"); 


    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        state.deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        input_handle(window, &state);
        physics_update(&state, state.deltaTime);
        renderer_draw(&state);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Takarítás
    glfwTerminate();
    return 0;
}
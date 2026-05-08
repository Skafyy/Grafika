#ifndef BILLIARD_H
#define BILLIARD_H

#ifdef _WIN32
    #include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include "config.h"
#include <stdbool.h>

typedef struct { float x, y, z; } Vec3;

typedef struct {
    Vec3 position;
    Vec3 velocity;
    float rotation_angle;
    Vec3 rotation_axis;
    bool is_in_pocket;
} Ball;

typedef struct {
    Ball balls[16];
    struct {
        Vec3 position;
        float yaw, pitch;
    } camera;
    float strike_power;
    float deltaTime;
    float light_intensity;
    bool is_charging;
    bool show_menu;
    unsigned int textures[3]; // 0: cloth, 1: carpet, 2: wall
} GameState;

void input_handle(GLFWwindow* window, GameState* state);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void game_init(GameState* state);
void physics_update(GameState* state, float dt);
void renderer_draw(GameState* state);

#endif
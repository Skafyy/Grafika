#include "../include/billiard.h"
#include <math.h>

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 512, lastY = 384;
    static bool first = true;
    GameState* state = (GameState*)glfwGetWindowUserPointer(window);

    if (first) { lastX = xpos; lastY = ypos; first = false; }
    state->yaw += (xpos - lastX) * 0.1f;
    state->pitch += (lastY - ypos) * 0.1f;
    lastX = xpos; lastY = ypos;

    if (state->pitch > 89.0f) state->pitch = 89.0f;
    if (state->pitch < -89.0f) state->pitch = -89.0f;
}

void processInput(GLFWwindow* window, GameState* state) {
    float speed = 2.5f * state->deltaTime;
    float rY = state->yaw * M_PI / 180.0f, rP = state->pitch * M_PI / 180.0f;
    float fx = cos(rY)*cos(rP), fy = sin(rP), fz = sin(rY)*cos(rP);
    state->targetX = state->camX + fx; state->targetY = state->camY + fy; state->targetZ = state->camZ + fz;
    float wx = cos(rY), wz = sin(rY), rx = -wz, rz = wx;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { state->camX += wx*speed; state->camZ += wz*speed; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { state->camX -= wx*speed; state->camZ -= wz*speed; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { state->camX -= rx*speed; state->camZ -= rz*speed; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { state->camX += rx*speed; state->camZ += rz*speed; }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && state->isCharging) {
    state->strikePos = state->balls[0].pos;
    state->ballInMotion = true;

    state->balls[0].vel.x = wx * state->strikePower * 0.8f;
    state->balls[0].vel.z = wz * state->strikePower * 0.8f;

    state->strikePower = 0.0f;
    state->cueOffset = 0.1f;
    state->isCharging = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        state->isCharging = true;
        if (state->strikePower < 1.0f) state->strikePower += 0.7f * state->deltaTime;
        state->cueOffset = 0.1f + state->strikePower * 0.5f;
    } else if (state->isCharging) {
        state->balls[0].vel.x = wx * state->strikePower * 0.8f;
        state->balls[0].vel.z = wz * state->strikePower * 0.8f;
        state->strikePower = 0; state->cueOffset = 0.1f; state->isCharging = false;
    }
}
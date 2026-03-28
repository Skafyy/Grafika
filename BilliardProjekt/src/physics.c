#include "../include/billiard.h"
#include <math.h>

// Itt CSAK jelezzük, hogy a deltaTime létezik (a main.c-ben van létrehozva)
// Így nem lesz "multiple definition" hiba a linkelésnél.
extern float deltaTime;

void updatePhysics() {
    float friction = 0.992f; 

    for (int i = 0; i < BALL_COUNT; i++) {
        // Mozgás frissítése
        balls[i].pos.x += balls[i].vel.x * deltaTime * 60.0f; 
        balls[i].pos.z += balls[i].vel.z * deltaTime * 60.0f;

        float speed = sqrt(balls[i].vel.x * balls[i].vel.x + balls[i].vel.z * balls[i].vel.z);

        if (speed > 0.0001f) {
            // Forgás
            balls[i].angle += (speed * 60.0f * deltaTime / BALL_RADIUS) * (180.0f / M_PI);
            balls[i].axis = (Vec3){-balls[i].vel.z, 0, balls[i].vel.x};
            
            // Súrlódás
            balls[i].vel.x *= friction;
            balls[i].vel.z *= friction;
        } else {
            balls[i].vel.x = 0;
            balls[i].vel.z = 0;
        }

        // Fal ütközés
        if (fabs(balls[i].pos.x) > (TABLE_W / 2 - BALL_RADIUS)) {
            balls[i].vel.x *= -0.8f;
            balls[i].pos.x = (balls[i].pos.x > 0 ? 1 : -1) * (TABLE_W / 2 - BALL_RADIUS);
        }
        if (fabs(balls[i].pos.z) > (TABLE_D / 2 - BALL_RADIUS)) {
            balls[i].vel.z *= -0.8f;
            balls[i].pos.z = (balls[i].pos.z > 0 ? 1 : -1) * (TABLE_D / 2 - BALL_RADIUS);
        }

        // Golyó-golyó ütközés
        for (int j = i + 1; j < BALL_COUNT; j++) {
            float dx = balls[j].pos.x - balls[i].pos.x;
            float dz = balls[j].pos.z - balls[i].pos.z;
            float dist = sqrt(dx * dx + dz * dz);

            if (dist < 2 * BALL_RADIUS && dist > 0.0f) {
                Vec3 temp = balls[i].vel;
                balls[i].vel = balls[j].vel;
                balls[j].vel = temp;
                
                float overlap = 2 * BALL_RADIUS - dist;
                float nx = dx / dist;
                float nz = dz / dist;
                
                balls[i].pos.x -= nx * overlap * 0.5f;
                balls[i].pos.z -= nz * overlap * 0.5f;
                balls[j].pos.x += nx * overlap * 0.5f;
                balls[j].pos.z += nz * overlap * 0.5f;
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 512, lastY = 384;
    static int firstMouse = 1;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    yaw   += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    float rad = yaw * M_PI / 180.0f;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camX += cos(rad) * cameraSpeed;
        camZ += sin(rad) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camX -= cos(rad) * cameraSpeed;
        camZ -= sin(rad) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }

    static int spacePressed = 0;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        float force = 0.2f;
        balls[0].vel.x = cos(yaw * M_PI / 180.0f) * force;
        balls[0].vel.z = sin(yaw * M_PI / 180.0f) * force;
        spacePressed = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = 0;
    }
    
}

void initGame() {

    balls[0].pos = (Vec3){0, 0, 1.5f};
    balls[0].vel = (Vec3){0, 0, 0};

    // A többi 4 golyó egy kis kupacban
    for(int i = 1; i < BALL_COUNT; i++) {
        balls[i].pos = (Vec3){(i-2.5f)*0.3f, 0, -1.0f};
        balls[i].vel = (Vec3){0, 0, 0};
        balls[i].angle = 0;
    }
}
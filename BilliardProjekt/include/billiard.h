#ifndef BILLIARD_H
#define BILLIARD_H

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdbool.h>

#define BALL_COUNT 5
#define BALL_RADIUS 0.12f
#define TABLE_W 2.4f
#define TABLE_D 4.8f

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 pos, vel, axis; float angle; } Ball;

typedef struct {
    float camX, camY, camZ;
    float targetX, targetY, targetZ;
    float yaw, pitch;
    float deltaTime, lightBrightness;
    float strikePower, cueOffset;
    Vec3 strikePos;
    bool ballInMotion;
    bool isCharging;
    Ball balls[BALL_COUNT];
    GLuint wallTex, carpetTex, clothTex;
} GameState;

// Függvények
void initGame(GameState* state);
void updatePhysics(GameState* state);
void processInput(GLFWwindow* window, GameState* state);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void drawScene(GameState* state);
GLuint loadTexture(const char* filename);

#endif
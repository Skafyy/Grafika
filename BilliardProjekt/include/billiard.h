#ifndef BILLIARD_H
#define BILLIARD_H

#include <GLFW/glfw3.h>
#include <math.h>

#define BALL_COUNT 5
#define BALL_RADIUS 0.12f
#define TABLE_W 2.4f
#define TABLE_D 4.8f

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// --- Struktúrák ---
typedef struct { float x, y, z; } Vec3;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Vec3 axis;
    float angle;
    GLuint texID;
} Ball;

// --- Globális változók (extern) ---
extern Ball balls[BALL_COUNT];
extern float camX, camY, camZ;
extern float yaw, pitch;
extern float lightBrightness;
extern float deltaTime; // Az egyenletes mozgáshoz

// --- Függvények ---
void initGame();
void updatePhysics();
void drawScene(GLFWwindow* window);

// GLFW Specifikus bemenetkezelés
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // Opcionális: fényerőhöz
GLuint loadTexture(const char* filename);

#endif
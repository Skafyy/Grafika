#include "../include/billiard.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

extern float deltaTime;
extern float camX, camY, camZ;
extern float yaw, pitch;
extern float targetX, targetY, targetZ;
extern Ball balls[BALL_COUNT];

float strikePower = 0.0f;    
float cueOffset = 0.1f;
bool isCharging = false;

static float lastX = 512, lastY = 384;
static bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;

    // 1. Irányvektorok kiszámítása a nézés alapján
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    float frontX = cos(radYaw) * cos(radPitch);
    float frontY = sin(radPitch);
    float frontZ = sin(radYaw) * cos(radPitch);

    // Célpont frissítése a gluLookAt számára
    targetX = camX + frontX;
    targetY = camY + frontY;
    targetZ = camZ + frontZ;

    // 2. Vízszintes mozgási irányok (WASD-hez)
    float walkX = cos(radYaw);
    float walkZ = sin(radYaw);
    float rightX = -walkZ;
    float rightZ = walkX;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camX += walkX * cameraSpeed;
        camZ += walkZ * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camX -= walkX * cameraSpeed;
        camZ -= walkZ * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camX -= rightX * cameraSpeed;
        camZ -= rightZ * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camX += rightX * cameraSpeed;
        camZ += rightZ * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        lightBrightness += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        lightBrightness -= 0.01f;
    if (lightBrightness < 0) lightBrightness = 0;
    }

    //3. Lövés leadása
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isCharging = true;
        // Növeljük az erőt, amíg nyomva tartjuk (kb. 1.5 másodperc a max erőig)
        if (strikePower < 1.0f) {
            strikePower += 0.7f * deltaTime; 
            if (strikePower > 1.0f) strikePower = 1.0f;
        }
    // A dákót hátrahúzzuk az erő függvényében (0.1 alap + max 0.5 eltolás)
        cueOffset = 0.1f + strikePower * 0.5f;
    } 
    else {
        if (isCharging) {
        float maxForce = 0.8f; // A maximális lökőerő
        float finalForce = strikePower * maxForce;
       
        balls[0].vel.x = walkX * finalForce;
        balls[0].vel.z = walkZ * finalForce;

        strikePower = 0.0f;
        cueOffset = 0.1f;
        isCharging = false;
        
        printf("Loves leadva! Ero: %.2f%%\n", finalForce * 100);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    static bool f1WasPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !f1WasPressed) {
        printf("\n--- HASZNALATI UTMUTATO ---\n");
        printf("WASD: Mozgas\n");
        printf("Eger: Nezegetes\n");
        printf("SPACE: Lokes (Feher golyo)\n");
        printf("+ / - : Fenyero allitasa\n");
        printf("ESC: Kilepes\n");
        f1WasPressed = true;
    } if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) f1WasPressed = false;

    // --- SZOBA FALAI ---
    float limit = 19.0f;

    if (camX >  limit) camX =  limit;
    if (camX < -limit) camX = -limit;
    if (camZ >  limit) camZ =  limit;
    if (camZ < -limit) camZ = -limit;

    
    //Asztal ütközés
    float tableW = (TABLE_W / 2.0f) + 0.3f;
    float tableD = (TABLE_D / 2.0f) + 0.3f;

    //Kameraállás vizsgálata
    if (camX > -tableW && camX < tableW && camZ > -tableD && camZ < tableD) {
    if (fabs(camX) / tableW > fabs(camZ) / tableD) {
        camX = (camX > 0) ? tableW : -tableW;
     } else {
        camZ = (camZ > 0) ? tableD : -tableD;
        }
    }
}

void updatePhysics() {
    float friction = 0.994f; 

    for (int i = 0; i < BALL_COUNT; i++) {
        // Csak akkor frissítjük, ha a golyó még az asztalon van (y >= 0)
        if (balls[i].pos.y < 0) continue;

        //Pozíció frissítése
        balls[i].pos.x += balls[i].vel.x * deltaTime * 60.0f;
        balls[i].pos.z += balls[i].vel.z * deltaTime * 60.0f;

        float speed = sqrt(balls[i].vel.x * balls[i].vel.x + balls[i].vel.z * balls[i].vel.z);

        if (speed > 0.001f) {
            balls[i].angle += (speed * 60.0f * deltaTime / BALL_RADIUS) * (180.0f / M_PI);
            balls[i].axis = (Vec3){-balls[i].vel.z, 0, balls[i].vel.x};
            
            balls[i].vel.x *= friction;
            balls[i].vel.z *= friction;
        } else {
            balls[i].vel.x = 0;
            balls[i].vel.z = 0;
        }

        //Lyukakba esés
        float w = TABLE_W / 2.0f;
        float d = TABLE_D / 2.0f;
        float pockets[6][2] = {
            {-w, -d}, {w, -d}, {w, d}, {-w, d},
            {-w, 0},  {w, 0}
        };

        for (int p = 0; p < 6; p++) {
            float pdx = balls[i].pos.x - pockets[p][0];
            float pdz = balls[i].pos.z - pockets[p][1];
            float pDist = sqrt(pdx * pdx + pdz * pdz);

            if (pDist < 0.22f) { // Ha elég közel van a lyuk közepéhez (sugár)
                balls[i].vel = (Vec3){0, 0, 0};
                balls[i].pos.y = -1.0f; // Lehúzzuk az asztal alá

                if (i == 0) {
                    balls[0].pos = (Vec3){0, 0, 1.0f};
                    balls[0].pos.y = 0.0f;
                }
            }
        }

        //Fal ütközés detektálás
        if (balls[i].pos.y >= 0) {
            if (fabs(balls[i].pos.x) > (TABLE_W / 2 - BALL_RADIUS)) {
                balls[i].vel.x *= -0.8f;
                balls[i].pos.x = (balls[i].pos.x > 0 ? 1 : -1) * (TABLE_W / 2 - BALL_RADIUS);
            }
            if (fabs(balls[i].pos.z) > (TABLE_D / 2 - BALL_RADIUS)) {
                balls[i].vel.z *= -0.8f;
                balls[i].pos.z = (balls[i].pos.z > 0 ? 1 : -1) * (TABLE_D / 2 - BALL_RADIUS);
            }
        }

        //Golyó-golyó ütközés
        for (int j = i + 1; j < BALL_COUNT; j++) {
            if (balls[j].pos.y < 0) continue; // Beesett golyóval nem ütközünk

            float dx = balls[j].pos.x - balls[i].pos.x;
            float dz = balls[j].pos.z - balls[i].pos.z;
            float dist = sqrt(dx * dx + dz * dz);

            if (dist < 2 * BALL_RADIUS && dist > 0.0f) {
                Vec3 tempVel = balls[i].vel;
                balls[i].vel.x = balls[j].vel.x * 0.95f;
                balls[i].vel.z = balls[j].vel.z * 0.95f;
                balls[j].vel.x = tempVel.x * 0.95f;
                balls[j].vel.z = tempVel.z * 0.95f;

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

    // ÚJRAINDÍTÁS
        int ballsOnTable = 0;

    for (int i = 1; i < BALL_COUNT; i++) {
        if (balls[i].pos.y >= 0.0f) {
            ballsOnTable++;
        }
    }

        if (ballsOnTable == 0) {
            initGame();
            printf("Gratulalok! Minden golyot elraktal. Jatek ujraindul...\n");
        }
}

void initGame() {
    // 0. indexű golyó: A FEHÉR (Lövő golyó)
    balls[0].pos = (Vec3){0, 0, 1.0f};
    balls[0].vel = (Vec3){0, 0, 0};
    balls[0].angle = 0;

    // A többi golyó elrendezése egy kis háromszögben (vagy sorban)
    for(int i = 1; i < BALL_COUNT; i++) {
        balls[i].pos = (Vec3){(i - 2.5f) * 0.35f, 0, -1.0f};
        balls[i].vel = (Vec3){0, 0, 0};
        balls[i].angle = 0;
    }
}
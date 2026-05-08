#include "billiard.h"
#include <GLFW/glfw3.h>
#include <math.h>
 #include <math.h>
#include <stdio.h>


#ifndef M_PI
    #define M_PI 3.14159265358979323846f
#endif


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 512, lastY = 384;
    static bool first = true;
    GameState* state = (GameState*)glfwGetWindowUserPointer(window);

    if (first) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        first = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    state->camera.yaw += xoffset * 0.1f;
    state->camera.pitch += yoffset * 0.1f;

    if (state->camera.pitch > 89.0f) state->camera.pitch = 89.0f;
    if (state->camera.pitch < -89.0f) state->camera.pitch = -89.0f;
}

void input_handle(GLFWwindow* window, GameState* state) {
    float speed = 2.5f * state->deltaTime;
    float radYaw = state->camera.yaw * M_PI / 180.0f;

    float wx = cosf(radYaw);
    float wz = sinf(radYaw);
    float rx = -wz; 
    float rz = wx;

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        state->light_intensity += 0.5f * state->deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        state->light_intensity -= 0.5f * state->deltaTime;
        if (state->light_intensity < 0.0f) state->light_intensity = 0.0f;
    }

    // --- Súgó megjelenítése (F1) ---
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        static float lastF1 = 0;
        if (glfwGetTime() - lastF1 > 0.5) { // Megakadályozzuk a spamelést
            printf("\n--- HASZNALATI UTMUTATO ---\n");
            printf("W, A, S, D: Mozgas a terben\n");
            printf("Eger: Nezegetes / Celzas\n");
            printf("SPACE (nyomva tartva): Utes erejenek gyujtese\n");
            printf("+ / -: Fenyero allitasa\n");
            printf("F1: Ez a leiras\n");
            printf("ESC: Kilepes\n");
            printf("---------------------------\n");
            lastF1 = glfwGetTime();
        }
    }

    // 1. Mozgás végrehajtása
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { state->camera.position.x += wx * speed; state->camera.position.z += wz * speed; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { state->camera.position.x -= wx * speed; state->camera.position.z -= wz * speed; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { state->camera.position.x -= rx * speed; state->camera.position.z -= rz * speed; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { state->camera.position.x += rx * speed; state->camera.position.z += rz * speed; }


    // ASZTAL ÜTKÖZÉS - Fix értékekkel (ha a TABLE_WIDTH pl. 4 és a DEPTH 8)
    float tw = 2.5f; 
    float td = 4.5f;

    if (state->camera.position.x > -tw && state->camera.position.x < tw &&
        state->camera.position.z > -td && state->camera.position.z < td) {
        
        // Kiszámoljuk melyik széltől milyen messze vagyunk
        float dRight = fabsf(state->camera.position.x - tw);
        float dLeft  = fabsf(state->camera.position.x + tw);
        float dFront = fabsf(state->camera.position.z - td);
        float dBack  = fabsf(state->camera.position.z + td);

        // Kiválasztjuk a legkisebb távolságot és azonnal KILÖKJÜK a kamerát
        if (dRight <= dLeft && dRight <= dFront && dRight <= dBack) state->camera.position.x = tw;
        else if (dLeft <= dRight && dLeft <= dFront && dLeft <= dBack) state->camera.position.x = -tw;
        else if (dFront <= dRight && dFront <= dLeft && dFront <= dBack) state->camera.position.z = td;
        else state->camera.position.z = -td;
    }

    // SZOBÁBÓL VALÓ KILÉPÉS MEGAKADÁLYOZÁSA
    float room = 19.0f;
    if (state->camera.position.x > room)  state->camera.position.x = room;
    if (state->camera.position.x < -room) state->camera.position.x = -room;
    if (state->camera.position.z > room)  state->camera.position.z = room;
    if (state->camera.position.z < -room) state->camera.position.z = -room;
    
    // Padló korlát
    if (state->camera.position.y < -1.5f) state->camera.position.y = -1.5f;

    // --- Ütés mechanika ---
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        state->is_charging = true;
        if (state->strike_power < 1.0f) state->strike_power += 0.8f * state->deltaTime;
    } 
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && state->is_charging) {
        state->balls[0].velocity.x = wx * state->strike_power * 12.0f; 
        state->balls[0].velocity.z = wz * state->strike_power * 12.0f;
        state->strike_power = 0.0f;
        state->is_charging = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        
        printf("\n--- BILLIARD STATUS ---\n");
        printf("Kamera pozicio: X:%.2f, Y:%.2f, Z:%.2f\n", 
                state->camera.position.x, 
                state->camera.position.y, 
                state->camera.position.z);
        
        printf("Kamera szogek: Yaw:%.2f, Pitch:%.2f\n", 
                state->camera.yaw, 
                state->camera.pitch);

        if (state->is_charging) {
            printf("UTES EREJE: [%.0f%%]\n", state->strike_power * 100.0f);
        } else {
            printf("Allapot: Kesz az utesre.\n");
        }
        printf("-----------------------\n");
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}
#include "../include/billiard.h"
#include <stdio.h>
#include <math.h>

Ball balls[BALL_COUNT];
float camX = 0, camY = 2.0f, camZ = 5.0f;
float yaw = -90.0f, pitch = -20.0f;
float lightBrightness = 1.0f;
float lastFrame = 0.0f;
float deltaTime = 0.0f;

void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; i++) {
        float lat0 = M_PI * (-0.5f + (float)(i) / stacks);
        float z0  = sinf(lat0);
        float zr0 = cosf(lat0);

        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = sinf(lat1);
        float zr1 = cosf(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2 * M_PI * (float)j / slices;
            float x = cosf(lng);
            float y = sinf(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glTexCoord2f((float)j / slices, (float)i / stacks);
            glVertex3f(x * zr0 * radius, y * zr0 * radius, z0 * radius);

            glNormal3f(x * zr1, y * zr1, z1);
            glTexCoord2f((float)j / slices, (float)(i + 1) / stacks);
            glVertex3f(x * zr1 * radius, y * zr1 * radius, z1 * radius);
        }
        glEnd();
    }
}

void drawScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Kamera beállítása
    glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-yaw - 90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-camX, -camY, -camZ);

    //Háttér
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float size = 50.0f; // A kocka mérete
    glBegin(GL_QUADS);
        // Padló (barna)
        glColor3f(0.4f, 0.2f, 0.0f); 
        glVertex3f(-size, -2.0f, -size);
        glVertex3f( size, -2.0f, -size);
        glVertex3f( size, -2.0f,  size);
        glVertex3f(-size, -2.0f,  size);

        // Plafon (halvány kék)
        glColor3f(0.6f, 0.8f, 1.0f);
        glVertex3f(-size,  size, -size);
        glVertex3f(-size,  size,  size);
        glVertex3f( size,  size,  size);
        glVertex3f( size,  size, -size);

        // Falak (szürke, különböző árnyalatok a sarokhatás miatt)

        // Hátsó fal
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-size, -2.0f, -size);
        glVertex3f(-size,  size, -size);
        glVertex3f( size,  size, -size);
        glVertex3f( size, -2.0f, -size);

        // Első fal
        glColor3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-size, -2.0f,  size);
        glVertex3f( size, -2.0f,  size);
        glVertex3f( size,  size,  size);
        glVertex3f(-size,  size,  size);

        // Bal fal
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex3f(-size, -2.0f, -size);
        glVertex3f(-size, -2.0f,  size);
        glVertex3f(-size,  size,  size);
        glVertex3f(-size,  size, -size);

        // Jobb fal
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex3f( size, -2.0f, -size);
        glVertex3f( size,  size, -size);
        glVertex3f( size,  size,  size);
        glVertex3f( size, -2.0f,  size);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Fények
    float lp[] = {0, 4, 0, 1};
    float ld[] = {lightBrightness, lightBrightness, lightBrightness, 1.0f};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lp);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);


    //Asztal
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);

    // Posztó (Zöld)
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex3f(-TABLE_W/2, 0, -TABLE_D/2);
    glVertex3f( TABLE_W/2, 0, -TABLE_D/2);
    glVertex3f( TABLE_W/2, 0,  TABLE_D/2);
    glVertex3f(-TABLE_W/2, 0,  TABLE_D/2);
    glEnd();
    glEnable(GL_LIGHTING);

    // Golyók kirajzolása
    glEnable(GL_TEXTURE_2D);
    for(int i = 0; i < BALL_COUNT; i++) {
        glPushMatrix();
        glTranslatef(balls[i].pos.x, BALL_RADIUS, balls[i].pos.z);
        
        if(balls[i].angle > 0.0f) {
            glRotatef(balls[i].angle, balls[i].axis.x, balls[i].axis.y, balls[i].axis.z);
        }
        
        glBindTexture(GL_TEXTURE_2D, balls[i].texID);
        drawSphere(BALL_RADIUS, 20, 20);
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        printf("Hiba a GLFW inicializálásakor!\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Billiard Pro FPS", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Perspektíva beállítása
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = 1024.0f / 768.0f;
    float fov = 45.0f;
    float f = 1.0f / tanf(fov * (M_PI / 360.0f));
    float zNear = 0.1f;
    float zFar = 100.0f;
    float proj[16] = {
        f/aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zFar+zNear)/(zNear-zFar), -1,
        0, 0, (2.0f*zFar*zNear)/(zNear-zFar), 0
    };
    glLoadMatrixf(proj);
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    initGame();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        updatePhysics();
        drawScene(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "../include/billiard.h"
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdbool.h>

// Globális változók
Ball balls[BALL_COUNT];
float camX = 0, camY = 2.0f, camZ = 5.0f;
float targetX = 0, targetY = 0, targetZ = 0; 
float yaw = -90.0f, pitch = -20.0f;
float lightBrightness = 1.0f;
float lastFrame = 0.0f;
float deltaTime = 0.0f;
Model tableModel;
GLuint carpetTex;
GLuint clothTex;
extern float cueOffset;
extern float strikePower;
extern bool isCharging; // Igaz, ha nyomva van a Space

// Külső függvények deklarációja (amik a physics.c-ben vannak)
extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern void processInput(GLFWwindow* window);
extern void updatePhysics();
extern void initGame();

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        printf("Hiba a textura betoltesekor: %s\n", filename);
    }
    return textureID;
}

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
    gluLookAt(camX, camY, camZ, targetX, targetY, targetZ, 0, 1, 0);

    // Szoba alapelemei
    glDisable(GL_LIGHTING);
    float s = 20.0f;
    float h = 10.0f;

    //1. Padló textúrával
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carpetTex);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -2.0f, -s);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( s, -2.0f, -s);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( s, -2.0f,  s);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, -2.0f,  s);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    //2. Plafon textúra nélkül 
    glBegin(GL_QUADS);
        glColor3f(0.7f, 0.7f, 0.7f); // Világosszürke
        glVertex3f(-s, h, -s);
        glVertex3f(-s, h,  s);
        glVertex3f( s, h,  s);
        glVertex3f( s, h, -s);
    glEnd();

    glEnable(GL_LIGHTING);
  
    //3. Falak textúrázása
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableModel.texID);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        //Hátsó fal
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -2.0f, -s); // Bal alsó
        glTexCoord2f(5.0f, 0.0f); glVertex3f( s, -2.0f, -s); // Jobb alsó
        glTexCoord2f(5.0f, 2.0f); glVertex3f( s,  h, -s);    // Jobb felső
        glTexCoord2f(0.0f, 2.0f); glVertex3f(-s,  h, -s);    // Bal felső

        //Elülső fal (Kamera mögötti)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -2.0f,  s);
        glTexCoord2f(5.0f, 0.0f); glVertex3f(-s,  h,  s);
        glTexCoord2f(5.0f, 2.0f); glVertex3f( s,  h,  s);
        glTexCoord2f(0.0f, 2.0f); glVertex3f( s, -2.0f,  s);

        //Bal fal
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -2.0f,  s);
        glTexCoord2f(5.0f, 0.0f); glVertex3f(-s, -2.0f, -s);
        glTexCoord2f(5.0f, 2.0f); glVertex3f(-s,  h, -s);
        glTexCoord2f(0.0f, 2.0f); glVertex3f(-s,  h,  s);

        //Jobb fal
        glTexCoord2f(0.0f, 0.0f); glVertex3f( s, -2.0f, -s);
        glTexCoord2f(5.0f, 0.0f); glVertex3f( s, -2.0f,  s);
        glTexCoord2f(5.0f, 2.0f); glVertex3f( s,  h,  s);
        glTexCoord2f(0.0f, 2.0f); glVertex3f( s,  h, -s);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    //4. Csillár textúra nélkül
    glDisable(GL_LIGHTING);
    glPushMatrix();
    
    glTranslatef(0.0f, h, 0.0f); 
    glRotatef(90, 1, 0, 0);

    glColor3f(0.1f, 0.1f, 0.1f);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.05f, 0.05f, 1.5f, 10, 10); 

    glTranslatef(0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 0.8f);
    drawSphere(0.3f, 15, 15);

    gluDeleteQuadric(quad);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    //5. Fények beállítása
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    float lp[] = {0.0f, 9.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lp);
    float ld[] = {lightBrightness, lightBrightness, lightBrightness, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lp);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
    glEnable(GL_LIGHT0);

    //6. Billiárdasztal 
    float w = TABLE_W / 2.0f; // Posztó szélesség fele
    float d = TABLE_D / 2.0f; // Posztó hosszúság fele
    float boardH = 0.4f;      // Az asztaltest vastagsága
    float topY = 0.02f;       // A posztó és a keret tetejének magassága
    float border = 0.18f;     // A fa keret vastagsága a posztó körül
    float legH = 1.6f;        // Lábak hossza (padlóig -2.0-n)

    //Az asztal teste   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableModel.texID); 
    glColor3f(0.8f, 0.6f, 0.4f);

    glBegin(GL_QUADS);
    //Első oldal (szemből)
    glTexCoord2f(0,0); glVertex3f(-w-border, topY, d+border);
    glTexCoord2f(4,0); glVertex3f( w+border, topY, d+border);
    glTexCoord2f(4,1); glVertex3f( w+border, -boardH, d+border);
    glTexCoord2f(0,1); glVertex3f(-w-border, -boardH, d+border);

    //Hátsó oldal
    glTexCoord2f(0,0); glVertex3f(-w-border, topY, -d-border);
    glTexCoord2f(4,0); glVertex3f( w+border, topY, -d-border);
    glTexCoord2f(4,1); glVertex3f( w+border, -boardH, -d-border);
    glTexCoord2f(0,1); glVertex3f(-w-border, -boardH, -d-border);

    //Bal oldal
    glTexCoord2f(0,0); glVertex3f(-w-border, topY, -d-border);
    glTexCoord2f(2,0); glVertex3f(-w-border, topY,  d+border);
    glTexCoord2f(2,1); glVertex3f(-w-border, -boardH,  d+border);
    glTexCoord2f(0,1); glVertex3f(-w-border, -boardH, -d-border);

    //Jobb oldal
    glTexCoord2f(0,0); glVertex3f( w+border, topY, -d-border);
    glTexCoord2f(2,0); glVertex3f( w+border, topY,  d+border);
    glTexCoord2f(2,1); glVertex3f( w+border, -boardH,  d+border);
    glTexCoord2f(0,1); glVertex3f( w+border, -boardH, -d-border);

    //A keret teteje
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableModel.texID);
    glBegin(GL_QUADS);

    //Hátsó keret-csík
    glTexCoord2f(0,0); glVertex3f(-w - border, topY, -d - border); // Külső bal hátsó
    glTexCoord2f(4,0); glVertex3f( w + border, topY, -d - border); // Külső jobb hátsó
    glTexCoord2f(4,1); glVertex3f( w,          topY, -d);          // Belső jobb hátsó
    glTexCoord2f(0,1); glVertex3f(-w,          topY, -d);          // Belső bal hátsó

    //Első keret-csík (A posztó előtt)
    glTexCoord2f(0,0); glVertex3f(-w - border, topY,  d + border); // Külső bal első
    glTexCoord2f(4,0); glVertex3f( w + border, topY,  d + border); // Külső jobb első
    glTexCoord2f(4,1); glVertex3f( w,          topY,  d);          // Belső jobb első
    glTexCoord2f(0,1); glVertex3f(-w,          topY,  d);          // Belső bal első

    //Bal keret-csík
    glTexCoord2f(0,0); glVertex3f(-w - border, topY, -d - border); // Külső bal hátsó
    glTexCoord2f(2,0); glVertex3f(-w,          topY, -d);          // Belső bal hátsó
    glTexCoord2f(2,1); glVertex3f(-w,          topY,  d);          // Belső bal első
    glTexCoord2f(0,1); glVertex3f(-w - border, topY,  d + border); // Külső bal első

    //Jobb keret-csík
    glTexCoord2f(0,0); glVertex3f( w + border, topY, -d - border); // Külső jobb hátsó
    glTexCoord2f(2,0); glVertex3f( w,          topY, -d);          // Belső jobb hátsó
    glTexCoord2f(2,1); glVertex3f( w,          topY,  d);          // Belső jobb első
    glTexCoord2f(0,1); glVertex3f( w + border, topY,  d + border); // Külső jobb első

    glEnd();
    glDisable(GL_TEXTURE_2D);

    //Az asztal lábai
    glColor3f(0.2f, 0.1f, 0.05f); // Sötétbarna
    for(int i = 0; i < 4; i++) {
        float lx = (i < 2) ? -w - 0.05f : w + 0.05f;
        float lz = (i % 2 == 0) ? -d - 0.05f : d + 0.05f;
        glPushMatrix();
        glTranslatef(lx, -boardH, lz);
        glRotatef(90, 1, 0, 0);
        GLUquadric* q = gluNewQuadric();
        gluCylinder(q, 0.12f, 0.1f, legH, 12, 12);
        gluDeleteQuadric(q);
        glPopMatrix();
    }

    //A zöld posztó textúrával
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, clothTex); // Itt váltunk a posztó képére
    glColor3f(1.0f, 1.0f, 1.0f);            // Fehér, hogy ne színezze el a képet

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, topY + 0.001f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( w, topY + 0.001f, -d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( w, topY + 0.001f,  d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, topY + 0.001f,  d);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tableModel.texID);

    //A lyukak beállítása
    glColor3f(0.05f, 0.05f, 0.05f); // Fekete
    float r = 0.16f; // Lyuk sugara
    for (int i = 0; i < 6; i++) {
        float px, pz;
        if (i == 0) { px = -w; pz = -d; }      // Bal hátsó sarok
        else if (i == 1) { px =  w; pz = -d; } // Jobb hátsó sarok
        else if (i == 2) { px =  w; pz =  d; } // Jobb első sarok
        else if (i == 3) { px = -w; pz =  d; } // Bal első sarok
        else if (i == 4) { px = -w; pz =  0; } // Bal közép
        else if (i == 5) { px =  w; pz =  0; } // Jobb közép

        glPushMatrix();

        glTranslatef(px, topY + 0.002f, pz); // A posztó tetejére
        glRotatef(90, 1, 0, 0);
        GLUquadric* q = gluNewQuadric();
        gluDisk(q, 0, r, 20, 1); // Fekete körlap a lyuknak
        gluDeleteQuadric(q);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);

   //7. Golyók kirajzolása
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    for(int i = 0; i < BALL_COUNT; i++) {
        // Csak akkor rajzoljuk, ha nincs a lyukban (y >= 0)
        if (balls[i].pos.y >= 0.0f) {
            glPushMatrix();
            glTranslatef(balls[i].pos.x, BALL_RADIUS + balls[i].pos.y, balls[i].pos.z);
            
            if(balls[i].angle > 0.0f) {
                glRotatef(balls[i].angle, balls[i].axis.x, balls[i].axis.y, balls[i].axis.z);
            }

            switch(i) {
                case 0: glColor3f(1.0f, 1.0f, 1.0f); break; // Fehér
                case 1: glColor3f(1.0f, 1.0f, 0.0f); break; // Sárga
                case 2: glColor3f(0.0f, 0.0f, 1.0f); break; // Kék
                case 3: glColor3f(1.0f, 0.0f, 0.0f); break; // Piros
                case 4: glColor3f(0.1f, 0.1f, 0.1f); break; // Fekete
                default: glColor3f(0.5f, 0.0f, 0.5f); break;
            }

            drawSphere(BALL_RADIUS, 20, 20);
            glPopMatrix();
        }
    }
    
    //8. Dákó kirajzolása
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tableModel.texID); 
    glDisable(GL_LIGHTING);

    glPushMatrix();
        glTranslatef(balls[0].pos.x, BALL_RADIUS + 0.02f, balls[0].pos.z);
        glRotatef(-yaw - 90, 0, 1, 0); 

        float finalCuePitch = -12.0f + (pitch * 0.3f); 
        if (finalCuePitch > -5.0f) finalCuePitch = -5.0f;
        if (finalCuePitch < -35.0f) finalCuePitch = -35.0f;
        glRotatef(finalCuePitch, 1, 0, 0); 

        glTranslatef(0, 0, cueOffset);

        GLUquadric* cueQuad = gluNewQuadric();
        gluQuadricTexture(cueQuad, GL_TRUE);
        
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();

        glLoadIdentity();
        glScalef(1.0f, 5.0f, 1.0f); 
            
        glMatrixMode(GL_MODELVIEW);
        glColor3f(1.0f, 0.95f, 0.8f);
        gluCylinder(cueQuad, 0.012f, 0.04f, 5.5f, 20, 1); 

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        
        glMatrixMode(GL_MODELVIEW);
        gluDeleteQuadric(cueQuad);
        glPopMatrix();

        glEnable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        // Alaphelyzetbe állítás a következő frame-hez
        glDisable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 1.0f, 1.0f);


    //Erőmérő HUD beállítása
    if (isCharging) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST); // <--- Ez fontos, hogy ne takarják egymást!

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 100, 0, 100, -1, 1);
    
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //A szürke keret beállítása
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(40, 10); glVertex2f(60, 10);
        glVertex2f(60, 15); glVertex2f(40, 15);
        glEnd();

        //A Színes csík (Csak ha van erő!)
        if (strikePower > 0.01f) {
            // Szín: Zöld -> Piros átmenet
            glColor3f(strikePower, 1.0f - strikePower, 0.0f); 
        
            float barWidth = strikePower * 20.0f; 
            glBegin(GL_QUADS);
                glVertex2f(40.1f, 10.1f); // Kicsit beljebb kezdjük
                glVertex2f(40.1f + barWidth - 0.2f, 10.1f);
                glVertex2f(40.1f + barWidth - 0.2f, 14.9f);
                glVertex2f(40.1f, 14.9f);
            glEnd();
        }

    //Visszaállítás
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST); // <--- Visszakapcsoljuk!
    glEnable(GL_LIGHTING);
}
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Hiba (%d): %s\n", error, description);
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        printf("Hiba a GLFW inicializálásakor!\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Billiard Pro FPS", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Projektmátrix beállítása
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = 1024.0f / 768.0f;
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    //Színek és fények inicializálása
    glEnable(GL_LIGHTING);        // Világítás bekapcsolása
    glEnable(GL_LIGHT0);          // Az első számú fényforrás
    glEnable(GL_COLOR_MATERIAL);  // Engedélyezi, hogy a glColor3f hasson a fényelt tárgyakra
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    //Játék adatok betöltése
    initGame();
    
    //Textúra betöltése
    tableModel.texID = loadTexture("wall.jpg");
    if (tableModel.texID == 0) {
        printf("Figyelem: A wood.jpg nem toltheto be!\n");
    }

    carpetTex = loadTexture("carpet.jpg");
    clothTex = loadTexture("cloth.jpg");

    //Fő ciklus
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
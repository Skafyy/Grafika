#include <GL/gl.h>
#include <GL/glu.h>
#include "billiard.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


void draw_sphere(float radius, int slices, int stacks) {
    GLUquadric* quad = gluNewQuadric();
    if (quad) {
        gluQuadricTexture(quad, GL_TRUE);
        gluQuadricNormals(quad, GLU_SMOOTH);
        gluSphere(quad, radius, slices, stacks);
        gluDeleteQuadric(quad);
    }
}

void draw_cue(float length, float width_base, float width_tip) {
    GLUquadric* quad = gluNewQuadric();
    if (quad) {
        gluQuadricTexture(quad, GL_TRUE);
        gluCylinder(quad, width_base, width_tip, length, 16, 1);
        gluDeleteQuadric(quad);
    }
}

void setup_lights() {
glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float light_pos[] = { 0.0f, 3.5f, 0.0f, 1.0f }; // A lámpa a plafonhoz közel, középen helyezkedik el
    float light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Alap sötétebb háttérfény
    float light_diffuse[] = { 1.2f, 1.2f, 0.9f, 1.0f };    // Erős, kicsit meleg fehér fény
    float light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // Csillogás a golyókon

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Fény gyengülése a távolsággal (hogy ne legyen az egész szoba egyformán világos)
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);

    float mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float mat_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void renderer_draw(GameState* state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 1. Kamera beállítása a Yaw/Pitch értékek alapján
    float radYaw = state->camera.yaw * 3.14159f / 180.0f;
    float radPitch = state->camera.pitch * 3.14159f / 180.0f;
    float tx = state->camera.position.x + cosf(radYaw) * cosf(radPitch);
    float ty = state->camera.position.y + sinf(radPitch);
    float tz = state->camera.position.z + sinf(radYaw) * cosf(radPitch);
    
    gluLookAt(state->camera.position.x, state->camera.position.y, state->camera.position.z, 
              tx, ty, tz, 
              0, 1, 0);

    setup_lights();

    // Méretkonstansok
    float tw = TABLE_WIDTH / 2.0f;
    float td = TABLE_DEPTH / 2.0f;
    float bh = 0.25f;  // Palánk magassága
    float bt = 0.15f;  // Palánk vastagsága
    float legH = 2.0f; // Asztal magassága

    // --- PADLÓ (Szőnyeg) ---
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, state->textures[1]); 
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -legH, -20.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 20.0f, -legH, -20.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 20.0f, -legH,  20.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, -legH,  20.0f);
    glEnd();

    // --- FALAK (wall.jpg) ---
    glBindTexture(GL_TEXTURE_2D, state->textures[2]); 
    float roomSize = 20.0f; // Ugyanakkora, mint a padló
    float wallHeight = 10.0f;

    glBegin(GL_QUADS);
        // Hátsó fal
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(-roomSize, -legH, -roomSize);
        glTexCoord2f(1, 0); glVertex3f( roomSize, -legH, -roomSize);
        glTexCoord2f(1, 1); glVertex3f( roomSize, wallHeight, -roomSize);
        glTexCoord2f(0, 1); glVertex3f(-roomSize, wallHeight, -roomSize);

        // Bal oldali fal
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0); glVertex3f(-roomSize, -legH, roomSize);
        glTexCoord2f(1, 0); glVertex3f(-roomSize, -legH, -roomSize);
        glTexCoord2f(1, 1); glVertex3f(-roomSize, wallHeight, -roomSize);
        glTexCoord2f(0, 1); glVertex3f(-roomSize, wallHeight, roomSize);

        // Jobb oldali fal
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0); glVertex3f(roomSize, -legH, -roomSize);
        glTexCoord2f(1, 0); glVertex3f(roomSize, -legH, roomSize);
        glTexCoord2f(1, 1); glVertex3f(roomSize, wallHeight, roomSize);
        glTexCoord2f(0, 1); glVertex3f(roomSize, wallHeight, -roomSize);

        // Első fal (a kamera mögött/körül)
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0); glVertex3f( roomSize, -legH, roomSize);
        glTexCoord2f(1, 0); glVertex3f(-roomSize, -legH, roomSize);
        glTexCoord2f(1, 1); glVertex3f(-roomSize, wallHeight, roomSize);
        glTexCoord2f(0, 1); glVertex3f( roomSize, wallHeight, roomSize);
    glEnd();

    // --- PLAFON ---
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.75f, 0.75f, 0.75f);
    
    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(-roomSize, wallHeight, -roomSize);
        glVertex3f( roomSize, wallHeight, -roomSize);
        glVertex3f( roomSize, wallHeight,  roomSize);
        glVertex3f(-roomSize, wallHeight,  roomSize);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);

    // --- LÁMPATEST AZ ASZTAL FELETT ---
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();

        glTranslatef(0.0f, wallHeight - 0.1f, 0.0f);
        glRotatef(90, 1, 0, 0); // Lefelé fordítjuk
        
        glColor3f(0.2f, 0.2f, 0.2f);
        draw_cue(1.0f, 0.05f, 0.8f);
        
        glDisable(GL_LIGHTING);
        glTranslatef(0.0f, 0.0f, 0.1f);
        glColor3f(1.0f, 1.0f, 0.8f);
        draw_sphere(0.15f, 16, 16);
        glEnable(GL_LIGHTING);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);

    // --- ASZTAL LÁBAI ---
    glEnable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, state->textures[2]); // Fa textúra
    float lw = 0.2f; 
    for (int i = 0; i < 4; i++) {
        float lx = (i < 2 ? 1 : -1) * (tw - lw);
        float lz = (i % 2 == 0 ? 1 : -1) * (td - lw);
        glPushMatrix();
        glTranslatef(lx, -legH, lz);
        glBegin(GL_QUADS);
            glNormal3f(1,0,0);  glVertex3f(lw, 0, 0); glVertex3f(lw, legH, 0); glVertex3f(lw, legH, lw); glVertex3f(lw, 0, lw);
            glNormal3f(-1,0,0); glVertex3f(0, 0, 0); glVertex3f(0, legH, 0); glVertex3f(0, legH, lw); glVertex3f(0, 0, lw);
            glNormal3f(0,0,1);  glVertex3f(0, 0, lw); glVertex3f(0, legH, lw); glVertex3f(lw, legH, lw); glVertex3f(lw, 0, lw);
            glNormal3f(0,0,-1); glVertex3f(0, 0, 0); glVertex3f(0, legH, 0); glVertex3f(lw, legH, 0); glVertex3f(lw, 0, 0);
        glEnd();
        glPopMatrix();
    }

    // --- ASZTAL POSZTÓ (Játékfelület) ---
    glBindTexture(GL_TEXTURE_2D, state->textures[0]);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0); glVertex3f(-tw, 0.01f, -td);
        glTexCoord2f(1, 0); glVertex3f( tw, 0.01f, -td);
        glTexCoord2f(1, 1); glVertex3f( tw, 0.01f,  td);
        glTexCoord2f(0, 1); glVertex3f(-tw, 0.01f,  td);
    glEnd();

    // --- LYUKAK (ZSEBEK) ---
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);

    float holeRadius = 0.22f; 
    float holeY = 0.015f;

    float holePositions[6][2] = {
        {-tw + 0.05f, -td + 0.05f}, {tw - 0.05f, -td + 0.05f},
        {-tw + 0.05f,  td - 0.05f}, {tw - 0.05f,  td - 0.05f},
        {-tw - 0.02f, 0.0f},        {tw + 0.02f, 0.0f}
    };

    GLUquadric* holeQuad = gluNewQuadric();
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glTranslatef(holePositions[i][0], holeY, holePositions[i][1]);
        glRotatef(90, 1, 0, 0);
        
        gluDisk(holeQuad, 0.0f, holeRadius, 32, 1); 
        glPopMatrix();
    }
    gluDeleteQuadric(holeQuad);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, state->textures[2]);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    float woodAmb[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    float woodDiff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, woodDiff);
    glMaterialfv(GL_FRONT, GL_AMBIENT, woodAmb);

    // --- ASZTAL FA KERET ÉS PALÁNK ---
    glBindTexture(GL_TEXTURE_2D, state->textures[2]);

    // Külső függőleges palánkok
    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);  glTexCoord2f(0,0); glVertex3f(tw+bt, -bh, -td-bt);  glTexCoord2f(1,0); glVertex3f(tw+bt, -bh, td+bt);  glTexCoord2f(1,1); glVertex3f(tw+bt, bh, td+bt);  glTexCoord2f(0,1); glVertex3f(tw+bt, bh, -td-bt);
        glNormal3f(-1, 0, 0); glTexCoord2f(0,0); glVertex3f(-tw-bt, -bh, -td-bt); glTexCoord2f(1,0); glVertex3f(-tw-bt, -bh, td+bt); glTexCoord2f(1,1); glVertex3f(-tw-bt, bh, td+bt); glTexCoord2f(0,1); glVertex3f(-tw-bt, bh, -td-bt);
        glNormal3f(0, 0, 1);  glTexCoord2f(0,0); glVertex3f(-tw-bt, -bh, td+bt);  glTexCoord2f(1,0); glVertex3f(tw+bt, -bh, td+bt);  glTexCoord2f(1,1); glVertex3f(tw+bt, bh, td+bt);  glTexCoord2f(0,1); glVertex3f(-tw-bt, bh, td+bt);
        glNormal3f(0, 0, -1); glTexCoord2f(0,0); glVertex3f(-tw-bt, -bh, -td-bt); glTexCoord2f(1,0); glVertex3f(tw+bt, -bh, -td-bt); glTexCoord2f(1,1); glVertex3f(tw+bt, bh, -td-bt); glTexCoord2f(0,1); glVertex3f(-tw-bt, bh, -td-bt);
    glEnd();

    // Felső vízszintes keret
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0,0); glVertex3f(tw, bh, -td-bt);     glTexCoord2f(1,0); glVertex3f(tw, bh, td+bt);      glTexCoord2f(1,1); glVertex3f(tw+bt, bh, td+bt);    glTexCoord2f(0,1); glVertex3f(tw+bt, bh, -td-bt);
        glTexCoord2f(0,0); glVertex3f(-tw-bt, bh, -td-bt); glTexCoord2f(1,0); glVertex3f(-tw-bt, bh, td+bt);  glTexCoord2f(1,1); glVertex3f(-tw, bh, td+bt);      glTexCoord2f(0,1); glVertex3f(-tw, bh, -td-bt);
        glTexCoord2f(0,0); glVertex3f(-tw, bh, td);        glTexCoord2f(1,0); glVertex3f(tw, bh, td);         glTexCoord2f(1,1); glVertex3f(tw, bh, td+bt);       glTexCoord2f(0,1); glVertex3f(-tw, bh, td+bt);
        glTexCoord2f(0,0); glVertex3f(-tw, bh, -td-bt);    glTexCoord2f(1,0); glVertex3f(tw, bh, -td-bt);     glTexCoord2f(1,1); glVertex3f(tw, bh, -td);         glTexCoord2f(0,1); glVertex3f(-tw, bh, -td);
    glEnd();

    // Belső gumiperem
    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0); glVertex3f(tw, 0, -td); glVertex3f(tw, 0, td); glVertex3f(tw, bh, td); glVertex3f(tw, bh, -td);
        glNormal3f(1, 0, 0);  glVertex3f(-tw, 0, -td); glVertex3f(-tw, 0, td); glVertex3f(-tw, bh, td); glVertex3f(-tw, bh, -td);
        glNormal3f(0, 0, -1); glVertex3f(-tw, 0, td); glVertex3f(tw, 0, td); glVertex3f(tw, bh, td); glVertex3f(-tw, bh, td);
        glNormal3f(0, 0, 1);  glVertex3f(-tw, 0, -td); glVertex3f(tw, 0, -td); glVertex3f(tw, bh, -td); glVertex3f(-tw, bh, -td);
    glEnd();

    // --- GOLYÓK ---
    glDisable(GL_TEXTURE_2D); 
    glEnable(GL_COLOR_MATERIAL);
    for (int i = 0; i < 16; i++) {
        if (state->balls[i].is_in_pocket) continue;
        glPushMatrix();
        glTranslatef(state->balls[i].position.x, BALL_RADIUS + 0.01f, state->balls[i].position.z);
        glRotatef(state->balls[i].rotation_angle, state->balls[i].rotation_axis.x, state->balls[i].rotation_axis.y, state->balls[i].rotation_axis.z);
        
        if (i == 0)      glColor3f(1.0f, 1.0f, 1.0f);     // Fehér golyó
        else if (i == 8) glColor3f(0.05f, 0.05f, 0.05f);  // Fekete golyó
        else if (i < 8)  glColor3f(1.0f, 0.2f, 0.2f);     // "Simák" (piros)
        else             glColor3f(0.2f, 0.2f, 1.0f);     // "Csíkosak" (kék)
        
        draw_sphere(BALL_RADIUS, 32, 32);
        glPopMatrix();
    }

// --- DÁKÓ ---
if (!state->balls[0].is_in_pocket) {
    glPushMatrix();

    Vec3 ballPos = state->balls[0].position;
    Vec3 camPos = state->camera.position;

    float dx = ballPos.x - camPos.x;
    float dy = (BALL_RADIUS) - (camPos.y - 0.4f); // A derékmagassághoz képest
    float dz = ballPos.z - camPos.z;
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);

    glTranslatef(camPos.x, camPos.y - 0.4f, camPos.z);

    // Cél irányának kiszámítása (yaw és pitch)
    float targetYaw = atan2f(dx, dz) * 180.0f / M_PI;
    float targetPitch = -asinf(dy / dist) * 180.0f / M_PI;

    // Lassan forduljon a cél irányába (simább animáció)
    glRotatef(targetYaw, 0.0f, 1.0f, 0.0f);
    glRotatef(targetPitch, 1.0f, 0.0f, 0.0f);

    // A dákó helyzete a golyóhoz képest (közelítve a golyóhoz, de nem túl közel)
    float swing = state->strike_power * 0.7f;
    glTranslatef(0.0f, 0.0f, 0.3f - swing);

    glDisable(GL_COLOR_MATERIAL);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, state->textures[2]); 

    float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    glMaterialfv(GL_FRONT, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Rajzolás
    draw_cue(3.5f, 0.04f, 0.015f);

    glPopMatrix();
    glEnable(GL_COLOR_MATERIAL);
}

    // --- HUD (Erőmérő) ---
    if (state->is_charging) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 100, 0, 100, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Háttér keret
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(40, 10); glVertex2f(60, 10); glVertex2f(60, 14); glVertex2f(40, 14);
        glEnd();

        // Aktuális erő csíkja (Piros -> Zöld átmenet)
        glColor3f(state->strike_power, 1.0f - state->strike_power, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(40, 10); 
            glVertex2f(40 + state->strike_power * 20.0f, 10); 
            glVertex2f(40 + state->strike_power * 20.0f, 14); 
            glVertex2f(40, 14);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glEnable(GL_DEPTH_TEST);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}
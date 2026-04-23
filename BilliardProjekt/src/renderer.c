#include <GL/gl.h>
#include <GL/glu.h>
#include "../include/billiard.h"
#include <math.h>


void drawSphere(float r, int sl, int st) {
    GLUquadric* sphereQuad = gluNewQuadric(); // Beszédesebb név
    if (sphereQuad != NULL) {                // Biztonsági ellenőrzés
        gluQuadricTexture(sphereQuad, GL_TRUE);
        gluSphere(sphereQuad, r, sl, st);
        gluDeleteQuadric(sphereQuad);
    }
}

void drawScene(GameState* state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(state->camX, state->camY, state->camZ, state->targetX, state->targetY, state->targetZ, 0, 1, 0);

    // --- SZOBA ÉS FÉNYEK ---
    glDisable(GL_LIGHTING); glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, state->carpetTex);
    glBegin(GL_QUADS); // Padló
        glTexCoord2f(0,0); glVertex3f(-20,-2,-20); glTexCoord2f(1,0); glVertex3f(20,-2,-20);
        glTexCoord2f(1,1); glVertex3f(20,-2,20); glTexCoord2f(0,1); glVertex3f(-20,-2,20);
    glEnd();

    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0);
    float lp[] = {0,9,0,1}, ld[] = {state->lightBrightness, state->lightBrightness, state->lightBrightness, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lp); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);

    // --- ASZTAL ---
    float tw = TABLE_W/2, td = TABLE_D/2;
    glBindTexture(GL_TEXTURE_2D, state->clothTex);
    glBegin(GL_QUADS); // Posztó
        glNormal3f(0,1,0); glTexCoord2f(0,0); glVertex3f(-tw, 0.021, -td);
        glTexCoord2f(1,0); glVertex3f(tw, 0.021, -td);
        glTexCoord2f(1,1); glVertex3f(tw, 0.021, td);
        glTexCoord2f(0,1); glVertex3f(-tw, 0.021, td);
    glEnd();

    // --- GOLYÓK ---
    glEnable(GL_COLOR_MATERIAL);
    for(int i=0; i<BALL_COUNT; i++) {
        if (state->balls[i].pos.y < 0){
            continue;
        } 
        glPushMatrix();
        glTranslatef(state->balls[i].pos.x, BALL_RADIUS + state->balls[i].pos.y, state->balls[i].pos.z);
        glRotatef(state->balls[i].angle, state->balls[i].axis.x, state->balls[i].axis.y, state->balls[i].axis.z);

        if(i==0){
            glColor3f(1,1,1);
        }  else if(i==1){
             glColor3f(1,1,0);
        } else if(i==2){
            glColor3f(0,0,1);
        } else if(i==3){
            glColor3f(1,0,0);
        }  else {
            glColor3f(0.1,0.1,0.1);
        } 

        drawSphere(BALL_RADIUS, 20, 20);
        glPopMatrix();
    }

    // --- HUD (Erőmérő) ---
    if (state->isCharging) {
        glDisable(GL_DEPTH_TEST); 
        glMatrixMode(GL_PROJECTION); 
        glPushMatrix(); 
        glLoadIdentity(); 

        glOrtho(0,100,0,100,-1,1);
        glMatrixMode(GL_MODELVIEW); 
        glPushMatrix(); 
        glLoadIdentity();

        glColor3f(0.2,0.2,0.2); 

        glBegin(GL_QUADS); 
        glVertex2f(40,10); 
        glVertex2f(60,10); 
        glVertex2f(60,15); 
        glVertex2f(40,15); 
        glEnd();

        glColor3f(state->strikePower, 1.0f-state->strikePower, 0); 
        glBegin(GL_QUADS); 
        glVertex2f(40.1,10.1); 
        glVertex2f(40.1+state->strikePower*20-0.2, 10.1); 
        glVertex2f(40.1+state->strikePower*20-0.2, 14.9); 
        glVertex2f(40.1,14.9); 
        glEnd();

        glPopMatrix(); 
        glMatrixMode(GL_PROJECTION); 
        glPopMatrix(); 
        glMatrixMode(GL_MODELVIEW); 
        glEnable(GL_DEPTH_TEST);
    }
}
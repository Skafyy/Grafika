#include "../include/billiard.h"
#include <math.h>

void initGame(GameState* state) {
    state->balls[0].pos = (Vec3){0, 0, 1.0f};
    state->balls[0].vel = (Vec3){0, 0, 0};
    for(int i = 1; i < BALL_COUNT; i++) {
        state->balls[i].pos = (Vec3){(i - 2.5f) * 0.35f, 0, -1.0f};
        state->balls[i].vel = (Vec3){0, 0, 0};
        state->balls[i].angle = 0;
    }
}

void updatePhysics(GameState* state) {
    float friction = 0.994f;
    float w = TABLE_W / 2.0f, d = TABLE_D / 2.0f;
    float pks[6][2] = {{-w,-d}, {w,-d}, {w,d}, {-w,d}, {-w,0}, {w,0}};

    for (int i = 0; i < BALL_COUNT; i++) {
        if (state->balls[i].pos.y < 0) continue;

        // Mozgás
        state->balls[i].pos.x += state->balls[i].vel.x * state->deltaTime * 60.0f;
        state->balls[i].pos.z += state->balls[i].vel.z * state->deltaTime * 60.0f;

        float speed = sqrt(state->balls[i].vel.x * state->balls[i].vel.x + state->balls[i].vel.z * state->balls[i].vel.z);
        if (speed > 0.001f) {
            state->balls[i].angle += (speed * 60.0f * state->deltaTime / BALL_RADIUS) * (180.0f / M_PI);
            state->balls[i].axis = (Vec3){-state->balls[i].vel.z, 0, state->balls[i].vel.x};
            state->balls[i].vel.x *= friction; state->balls[i].vel.z *= friction;
        }

        // Ütközés: Lyukak
        for (int p = 0; p < 6; p++) {
            float dx = state->balls[i].pos.x - pks[p][0], dz = state->balls[i].pos.z - pks[p][1];
            if (sqrt(dx*dx + dz*dz) < 0.22f) {
                state->balls[i].vel = (Vec3){0,0,0}; state->balls[i].pos.y = -1.0f;
                if (i == 0) { state->balls[0].pos = (Vec3){0,0,1.0f}; state->balls[0].pos.y = 0.0f; }
            }
        }

        // Ütközés: Falak
        if (fabs(state->balls[i].pos.x) > (w - BALL_RADIUS)) {
            state->balls[i].vel.x *= -0.8f; 
            state->balls[i].pos.x = (state->balls[i].pos.x > 0 ? 1 : -1) * (w - BALL_RADIUS);
        }
        if (fabs(state->balls[i].pos.z) > (d - BALL_RADIUS)) {
            state->balls[i].vel.z *= -0.8f; 
            state->balls[i].pos.z = (state->balls[i].pos.z > 0 ? 1 : -1) * (d - BALL_RADIUS);
        }

        // Ütközés: Golyó-Golyó
        for (int j = i + 1; j < BALL_COUNT; j++) {
            if (state->balls[j].pos.y < 0) continue;
            float dx = state->balls[j].pos.x - state->balls[i].pos.x;
            float dz = state->balls[j].pos.z - state->balls[i].pos.z;
            float dist = sqrt(dx*dx + dz*dz);
            if (dist < 2 * BALL_RADIUS && dist > 0.0f) {
                Vec3 v = state->balls[i].vel;
                state->balls[i].vel.x = state->balls[j].vel.x * 0.95f; 
                state->balls[i].vel.z = state->balls[j].vel.z * 0.95f;
                state->balls[j].vel.x = v.x * 0.95f; 
                state->balls[j].vel.z = v.z * 0.95f;
                float overlap = 2 * BALL_RADIUS - dist;
                state->balls[i].pos.x -= (dx/dist) * overlap * 0.5f;
                state->balls[j].pos.x += (dx/dist) * overlap * 0.5f;
            }
        }
    }
}
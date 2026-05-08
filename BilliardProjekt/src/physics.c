#include "../include/billiard.h"
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846f
#endif

void game_init(GameState* state) {
    // Fehér golyó pozíciója (D-vonal környéke)
    state->balls[0].position = (Vec3){0.0f, 0.0f, TABLE_DEPTH / 4.0f};
    state->balls[0].velocity = (Vec3){0, 0, 0};
    state->balls[0].is_in_pocket = false;

    // Többi golyó elrendezése háromszög alakban
    int idx = 1;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col <= row; col++) {
            if (idx < 16) {
                state->balls[idx].position = (Vec3){
                    (col - row * 0.5f) * (BALL_RADIUS * 2.1f), 
                    0.0f, 
                    -TABLE_DEPTH / 4.0f - row * (BALL_RADIUS * 1.9f)
                };
                state->balls[idx].velocity = (Vec3){0, 0, 0};
                state->balls[idx].rotation_angle = 0;
                state->balls[idx].is_in_pocket = false;
                idx++;
            }
        }
    }
}


 //Golyók közötti ütközés lekezelése impulzusmegmaradással.
 
void resolve_collision(Ball* a, Ball* b) {
    float dx = b->position.x - a->position.x;
    float dz = b->position.z - a->position.z;
    float dist = sqrtf(dx * dx + dz * dz);

    if (dist < 2.0f * BALL_RADIUS && dist > 0.0f) {
        // 1. Ütközési normálvektor
        float nx = dx / dist;
        float nz = dz / dist;

        // 2. Relatív sebesség a normális mentén
        float rvx = a->velocity.x - b->velocity.x;
        float rvz = a->velocity.z - b->velocity.z;
        float velAlongNormal = rvx * nx + rvz * nz;

        // Csak akkor ütköznek, ha egymás felé mozognak
        if (velAlongNormal > 0) {
            // 3. Impulzus nagysága (rugalmassági tényezővel: BOUNCE_BALL)
            float j = -(1.0f + BOUNCE_BALL) * velAlongNormal;
            j /= 2.0f; // Azonos tömeg miatt m1=m2

            // 4. Sebességvektorok frissítése
            a->velocity.x += j * nx;
            a->velocity.z += j * nz;
            b->velocity.x -= j * nx;
            b->velocity.z -= j * nz;

            // 5. Statikus ütközéselhárítás (behatolás korrekciója, hogy ne ragadjanak össze)
            float overlap = 2.0f * BALL_RADIUS - dist;
            a->position.x -= nx * overlap * 0.5f;
            a->position.z -= nz * overlap * 0.5f;
            b->position.x += nx * overlap * 0.5f;
            b->position.z += nz * overlap * 0.5f;
        }
    }
}

void physics_update(GameState* state, float dt) {
    float hw = TABLE_WIDTH / 2.0f;
    float hd = TABLE_DEPTH / 2.0f;
    
    // Lyukak pozíciói
    float pockets[6][2] = {{-hw,-hd}, {hw,-hd}, {hw,hd}, {-hw,hd}, {-hw,0}, {hw,0}};

    for (int i = 0; i < 16; i++) {
        Ball* b = &state->balls[i];
        if (b->is_in_pocket) continue;

        // --- Mozgás integrálás ---
        b->position.x += b->velocity.x * dt;
        b->position.z += b->velocity.z * dt;

        float speed = sqrtf(b->velocity.x * b->velocity.x + b->velocity.z * b->velocity.z);
        
        if (speed > 0.001f) {
            // Vizuális forgás számítása
            b->rotation_angle += (speed * dt / BALL_RADIUS) * (180.0f / M_PI);
            b->rotation_axis = (Vec3){-b->velocity.z, 0, b->velocity.x};
            
            // Súrlódás alkalmazása (időalapon, nem fixen)
            b->velocity.x *= powf(FRICTION_FACTOR, dt * 60.0f);
            b->velocity.z *= powf(FRICTION_FACTOR, dt * 60.0f);
        } else {
            b->velocity = (Vec3){0, 0, 0};
        }

        // --- Ütközés: Lyukak ---
        for (int p = 0; p < 6; p++) {
            float dx = b->position.x - pockets[p][0];
            float dz = b->position.z - pockets[p][1];
            if (sqrtf(dx*dx + dz*dz) < BALL_RADIUS * 2.0f) {
                b->is_in_pocket = true;
                b->velocity = (Vec3){0,0,0};
                
                // Fehér golyó büntetés (scratch)
                if (i == 0) {
                    b->position = (Vec3){0.0f, 0.0f, TABLE_DEPTH / 4.0f};
                    b->is_in_pocket = false;
                }
            }
        }

        // --- Ütközés: Falak ---
        if (fabsf(b->position.x) > (hw - BALL_RADIUS)) {
            b->velocity.x *= -BOUNCE_WALL;
            b->position.x = (b->position.x > 0 ? 1 : -1) * (hw - BALL_RADIUS);
        }
        if (fabsf(b->position.z) > (hd - BALL_RADIUS)) {
            b->velocity.z *= -BOUNCE_WALL;
            b->position.z = (b->position.z > 0 ? 1 : -1) * (hd - BALL_RADIUS);
        }

        // --- Ütközés: Golyó-Golyó ---
        for (int j = i + 1; j < 16; j++) {
            if (!state->balls[j].is_in_pocket) {
                resolve_collision(b, &state->balls[j]);
            }
        }
    }
}
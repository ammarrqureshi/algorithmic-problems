
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 80
#define HEIGHT 25
#define NUM_PARTICLES_X 20
#define NUM_PARTICLES_Y 10
#define SPACING 3
#define GRAVITY 0.1f
#define FRICTION 0.9f
#define STIFFNESS 0.1f
#define DAMPING 0.1f

typedef struct {
    float x, y;
    float old_x, old_y;
} Particle;

typedef struct {
    int p1, p2;
    float rest_length;
} Constraint;

Particle particles[NUM_PARTICLES_X * NUM_PARTICLES_Y];
Constraint constraints[(NUM_PARTICLES_X - 1) * NUM_PARTICLES_Y + NUM_PARTICLES_X * (NUM_PARTICLES_Y - 1)];

void init_cloth() {
    int c = 0;
    for (int y = 0; y < NUM_PARTICLES_Y; y++) {
        for (int x = 0; x < NUM_PARTICLES_X; x++) {
            int i = y * NUM_PARTICLES_X + x;
            particles[i].x = x * SPACING;
            particles[i].y = y * SPACING;
            particles[i].old_x = particles[i].x;
            particles[i].old_y = particles[i].y;

            if (x > 0) {
                constraints[c].p1 = i;
                constraints[c].p2 = i - 1;
                constraints[c].rest_length = SPACING;
                c++;
            }
            if (y > 0) {
                constraints[c].p1 = i;
                constraints[c].p2 = i - NUM_PARTICLES_X;
                constraints[c].rest_length = SPACING;
                c++;
            }
        }
    }
}

void update_particles(float dt) {
    for (int i = 0; i < NUM_PARTICLES_X * NUM_PARTICLES_Y; i++) {
        if (i < NUM_PARTICLES_X) continue; // Pin the top row

        float vx = (particles[i].x - particles[i].old_x) * (1 - DAMPING);
        float vy = (particles[i].y - particles[i].old_y) * (1 - DAMPING);

        particles[i].old_x = particles[i].x;
        particles[i].old_y = particles[i].y;

        particles[i].x += vx;
        particles[i].y += vy + GRAVITY * dt;

        if (particles[i].x < 0) particles[i].x = 0;
        if (particles[i].x >= WIDTH) particles[i].x = WIDTH - 1;
        if (particles[i].y < 0) particles[i].y = 0;
        if (particles[i].y >= HEIGHT) particles[i].y = HEIGHT - 1;
    }
}

void satisfy_constraints() {
    for (int c = 0; c < (NUM_PARTICLES_X - 1) * NUM_PARTICLES_Y + NUM_PARTICLES_X * (NUM_PARTICLES_Y - 1); c++) {
        Constraint constraint = constraints[c];
        Particle *p1 = &particles[constraint.p1];
        Particle *p2 = &particles[constraint.p2];

        float dx = p2->x - p1->x;
        float dy = p2->y - p1->y;
        float dist = sqrt(dx * dx + dy * dy);
        float diff = (dist - constraint.rest_length) / dist * 0.5f;

        if (dist > 0) {
            p1->x += dx * diff * STIFFNESS;
            p1->y += dy * diff * STIFFNESS;
            p2->x -= dx * diff * STIFFNESS;
            p2->y -= dy * diff * STIFFNESS;
        }
    }
}

void render_cloth() {
    char screen[HEIGHT][WIDTH];
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screen[y][x] = ' ';
        }
    }

    for (int i = 0; i < NUM_PARTICLES_X * NUM_PARTICLES_Y; i++) {
        int px = (int)particles[i].x;
        int py = (int)particles[i].y;
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            screen[py][px] = '*';
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            putchar(screen[y][x]);
        }
        putchar('\n');
    }
}

int main() {
    srand((unsigned int)time(NULL));

    init_cloth();

    const float dt = 0.016f; // Simulation timestep
    while (1) {
        update_particles(dt);
        for (int i = 0; i < 3; i++) { // Iteratively satisfy constraints
            satisfy_constraints();
        }

        printf("\033[H\033[J"); // Clear screen
        render_cloth();

        struct timespec ts = {0, 16000000L}; // 16 ms
        nanosleep(&ts, NULL);
    }

    return 0;
}


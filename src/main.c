// Forward And Backward Reaching Inverse Kinematics
// Implementation: http://www.andreasaristidou.com/FABRIK.html
#include "raylib.h"
#include "raymath.h"


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define MAX_FPS 360
#define NUM_POINTS 64
#define MAX_STEPS 10


int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "FABRIK");
    SetTargetFPS(MAX_FPS);

    const Vector2 WINDOW_CENTRE = (Vector2) {
        WINDOW_WIDTH / 2.0f,
        WINDOW_HEIGHT / 2.0f
    };

    Vector2 joints[NUM_POINTS] = {};
    float lengths[NUM_POINTS] = {};
    Vector2 position = WINDOW_CENTRE;
    float total_length = 0;

    float d = 12;
    int i = 0;
    for (i = 0; i < NUM_POINTS; i++) {
        joints[i] = position;
        position.x += d;
        d -= 0.15f;
    }
    float length = 0;
    for (i = 0; i < NUM_POINTS - 1; i++) {
        length = Vector2Length(Vector2Subtract(joints[i], joints[i + 1]));
        total_length += length;
        lengths[i] = length; 
    }
    lengths[i] = length;

    Vector2 start = WINDOW_CENTRE;
    Vector2 goal = Vector2Zero();  // Will be mouse pointer
    float tolerance = 0.1f;

    Vector2 r = Vector2Zero();
    float l = 0;
    Vector2 pos = Vector2Zero();
    float distance = 0;
    int b_count = 0;

    while (!WindowShouldClose())
    {
        goal = GetMousePosition();

        distance = Vector2Length(Vector2Subtract(joints[0], goal));
        if (distance > total_length) {
            for (int i = 0; i < NUM_POINTS - 1; i++) {
                r = Vector2Subtract(goal, joints[i]);
                l = lengths[i] / Vector2Length(r);
                pos = Vector2Add(Vector2Scale(joints[i], (1 - l)), Vector2Scale(goal, l));
                joints[i + 1] = pos;
            }
        } else {
            b_count = 0;
            while (distance > tolerance && b_count < MAX_STEPS) {
                // Backward
                joints[NUM_POINTS - 1] = goal;
                for (int i = NUM_POINTS - 2; i > -1; i--) {
                    r = Vector2Subtract(joints[i + 1], joints[i]);
                    l = lengths[i] / Vector2Length(r);
                    pos = Vector2Add(Vector2Scale(joints[i + 1], (1 - l)), Vector2Scale(joints[i], l));
                    joints[i] = pos;
                }

                // Forward
                joints[0] = start;
                for (int i = 0; i < NUM_POINTS - 1; i++) {
                    r = Vector2Subtract(joints[i + 1], joints[i]);
                    l = lengths[i] / Vector2Length(r);
                    pos = Vector2Add(Vector2Scale(joints[i], (1 - l)), Vector2Scale(joints[i + 1], l));
                    joints[i + 1] = pos;
                }

                // Recalculate distance to goal
                distance = Vector2Length(Vector2Subtract(joints[0], goal));
                b_count++;
            }
        }


        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < NUM_POINTS; i++) {
            DrawCircleV(joints[i], lengths[i], MAGENTA);
        }
        DrawLineStrip(joints, NUM_POINTS, GREEN);
        EndDrawing();

        DrawFPS(0, 0);
    }

    CloseWindow();

    return 0;
}

#include "raylib.h"
#include "raymath.h"
#define DARRAY_IMPLEMENTATION
#include "darray.h"

int main(void) {
    int screenSize = 90;
    const int screenWidth = screenSize * 16;
    const int screenHeight = screenSize * 9;

    InitWindow(screenWidth, screenHeight, "Curves");

    // Create a dynamic array to store the points
    Vector2* points = darrayCreate(Vector2);

    // Index that is currently being dragged by the mouse
    int dragIndex = -1;
    // The amount of squares inbetween handles
    int sampleSize = 50;
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Bezier Curves", 30, 50, 50, LIGHTGRAY);
        DrawText("LMB - Make handles", 30, 110, 25, LIGHTGRAY);
        DrawText("RMB - Move handles", 30, 160, 25, LIGHTGRAY);
        DrawText("Mouse Wheel - Change sample size", 30, 210, 25, LIGHTGRAY);

        float mouseWheel = GetMouseWheelMove();
        if (mouseWheel != 0)
        {
            sampleSize += (int)mouseWheel;
            if (sampleSize < 10) sampleSize = 10;
        }

        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            darrayPush(points, mouse);
        }

        int len = darrayLength(points);
        Vector2 size = {.x = 20, .y = 20};
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            // Go through the handles and see if the RMB happened on one of them
            for (int i = 0; i < len; i++) {
                if (CheckCollisionPointRec(
                        mouse, (Rectangle){.x = points[i].x - size.x / 2,
                                           .y = points[i].y - size.y / 2,
                                           .width = size.x,
                                           .height = size.y})) {
                    dragIndex = i;
                    break;
                }
            }
        }

        // If currently dragging something, move it's pos
        if (dragIndex != -1) {
            points[dragIndex] = mouse;
            if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                dragIndex = -1;
            }
        }

        for (int i = 0; i < len; i++) {
            Vector2 p1 = points[i];

            if (i + 3 < len) {
                Vector2 p2 = points[i + 1];
                Vector2 p3 = points[i + 2];
                Vector2 p4 = points[i + 3];
                for (int y = 0; y < sampleSize; y++) {
                    float t = (float)y / sampleSize;
                    Vector2 d1 = Vector2Lerp(p1, p2, t);
                    Vector2 d2 = Vector2Lerp(p2, p3, t);
                    Vector2 d3 = Vector2Lerp(p3, p4, t);

                    Vector2 pos = Vector2Lerp(Vector2Lerp(d1, d2, t),
                                              Vector2Lerp(d2, d3, t), t);
                    DrawRectangleV((Vector2){.x = pos.x - size.x / 2,
                                             .y = pos.y - size.y / 2},
                                   size, BLUE);
                }

                // Draw the handles last for the correct z-index
                DrawRectangleV(
                    (Vector2){.x = p2.x - size.x / 2, .y = p2.y - size.y / 2},
                    size, RED);
                DrawRectangleV(
                    (Vector2){.x = p3.x - size.x / 2, .y = p3.y - size.y / 2},
                    size, RED);
                DrawRectangleV(
                    (Vector2){.x = p4.x - size.x / 2, .y = p4.y - size.y / 2},
                    size, RED);

                i += 2;
            }
            // Draw the first handle last for the correct z-index
            DrawRectangleV(
                (Vector2){.x = p1.x - size.x / 2, .y = p1.y - size.y / 2}, size,
                RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

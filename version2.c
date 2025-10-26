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
    // so we can orientate the handles correctly
    Vector2 lastClickedPoint = {0, 0};
    char autoCloseShape = true;

    // Index that is currently being dragged by the mouse
    int dragIndex = -1;
    // The amount of squares inbetween handles
    int sampleSize = 50;
    while (!WindowShouldClose()) {
        Vector2 size = {.x = 20, .y = 20};
        Vector2 handleSize = {.x = 15, .y = 15};

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Bezier Curves", 30, 50, 50, LIGHTGRAY);
        DrawText("LMB - Make handles", 30, 110, 25, LIGHTGRAY);
        DrawText("RMB - Move handles", 30, 160, 25, LIGHTGRAY);
        DrawText("Mouse Wheel - Change sample size", 30, 210, 25, LIGHTGRAY);
        DrawText("R - Clear the screen", 30, 260, 25, LIGHTGRAY);
        DrawText("Space - Close the shape", 30, 310, 25, LIGHTGRAY);

        if (IsKeyPressed(KEY_R)) {
            darrayClear(points);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            autoCloseShape = !autoCloseShape;
        }

        float mouseWheel = GetMouseWheelMove();
        if (mouseWheel != 0) {
            sampleSize += (int)mouseWheel;
            if (sampleSize < 10)
                sampleSize = 10;
        }

        int len = darrayLength(points);
        Vector2 mouse = GetMousePosition();
        // Spacing between the handles and the point
        int offset = 50;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // What to multiple the offset with either 1 or -1
            int mult = 1;
            if (len > 1) {
                // If last clicked is right of mouse flip the handles so they are correct
                if (mouse.x < lastClickedPoint.x) {
                    mult = -1;
                }
            }
            // Handle 1
            darrayPush(points,
                       ((Vector2){.x = mouse.x - size.x / 2 - offset * mult,
                                  .y = mouse.y - size.y / 2}));
            // Anchor
            darrayPush(points, ((Vector2){.x = mouse.x - size.x / 2,
                                          .y = mouse.y - size.y / 2}));
            // Handle 2
            darrayPush(points,
                       ((Vector2){.x = mouse.x - size.x / 2 + offset * mult,
                                  .y = mouse.y - size.y / 2}));

            lastClickedPoint = mouse;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            // Go through the handles and see if the RMB happened on one of them
            for (int i = 0; i < len; i++) {
                if (CheckCollisionPointRec(mouse,
                                           (Rectangle){.x = points[i].x,
                                                       .y = points[i].y,
                                                       .width = size.x,
                                                       .height = size.y})) {
                    dragIndex = i;
                    break;
                }
            }
        }

        // If currently dragging something, move it's pos
        if (dragIndex != -1) {
            points[dragIndex] = ((Vector2){.x = mouse.x - size.x / 2,
                                           .y = mouse.y - size.y / 2});
            if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
                dragIndex = -1;
            }
        }

        for (int i = 0; i < len; i++) {
            Vector2 p1 = points[i];

            if (i == 0 || i == len - 1){
                if (autoCloseShape){
                    DrawRectangleV(p1, handleSize, RED);
                }
                continue;
            }

            if (autoCloseShape ? i + 3 <= len + 1 : i + 3 < len) {
                Vector2 p2 = points[i + 1];
                Vector2 p3 = points[(i + 2) % len];
                Vector2 p4 = points[(i + 3) % len];
                for (int y = 1; y < sampleSize; y++) {
                    float t = (float)y / sampleSize;
                    Vector2 d1 = Vector2Lerp(p1, p2, t);
                    Vector2 d2 = Vector2Lerp(p2, p3, t);
                    Vector2 d3 = Vector2Lerp(p3, p4, t);

                    Vector2 pos = Vector2Lerp(Vector2Lerp(d1, d2, t),
                                              Vector2Lerp(d2, d3, t), t);
                    DrawRectangleV(pos, size, BLUE);
                }

                // Draw the handles last for the correct z-index
                DrawRectangleV(p2, handleSize, RED);
                DrawRectangleV(p3, handleSize, RED);

                i += 2;
            }
            // Draw the first handle last for the correct z-index
            if (i <= len){
                DrawRectangleV(p1, size, RED);
            }
        }
        // Draw the first anchor last so that it is on top of the blue sample squares
        if (len > 0){
            DrawRectangleV(points[1], size, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

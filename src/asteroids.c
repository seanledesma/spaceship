#include <raylib.h>
#include <math.h>
#include <stdlib.h>

const int BORDER = 500;
#define screenWidth 1920
#define worldWidth screenWidth + BORDER
#define screenHeight 1080
#define worldHeight screenHeight + BORDER
#define ballCount sizeof(balls) / sizeof(balls[0])

const float SPEED = 10.0f;
// this uses both typedef and struct tag :)
typedef struct Ball{
    Vector2 position;
    Vector2 speed;
    float radius;
} Ball;

typedef struct Player{
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 velocity;
    Vector2 direction;
    int lives;
} Player;


void RotateTriangle(Vector2 *point, Vector2 *pivot, float angle) {
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);

    float tempX = pivot->x + (point->x - pivot->x) * cosTheta - (point->y - pivot->y) * sinTheta;
    float tempY = pivot->y + (point->x - pivot->x) * sinTheta + (point->y - pivot->y) * cosTheta;

    point->x = tempX;
    point->y = tempY;
}


int main(void) {
    InitWindow(screenWidth, screenHeight, "spaceship");

    Ball balls[20] = {};


    Player player = {
        .v1 = { screenWidth / 2, screenHeight / 2 },
        .v2 = { (screenWidth / 2) - 15, (screenHeight / 2) + 30 },
        .v3 = { (screenWidth / 2) + 15, (screenHeight / 2) + 30 },
        .velocity = { 0.0f, 0.0f },
        .direction = { 0, -1 },
        .lives = 100,
    };

    for(int i = 0; i < ballCount; i++) {
        balls[i].position = (Vector2) { GetRandomValue(20, worldWidth), GetRandomValue(20, worldHeight) };
        float temp_speed = (float) GetRandomValue(3, 8);
        balls[i].speed = (Vector2) { temp_speed, temp_speed };
        balls[i].radius = (float) GetRandomValue(15, 40);
    }
    
    float totalRotationAngle = 0.0f;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        float frameRotationAngle = 0.0f;
        
        if (IsKeyDown(KEY_LEFT)) {
            frameRotationAngle = -0.05f;  
            totalRotationAngle += frameRotationAngle;
        }else if (IsKeyDown(KEY_RIGHT)) {
            frameRotationAngle = 0.05f;
            totalRotationAngle += frameRotationAngle;
        }
        // i need to make the direction towards the nose of the spacecraft, which is have a quarter of a turn left from right
        // full turn equals 2PI
        player.direction = (Vector2){
            cos(totalRotationAngle - PI/2),
            sin(totalRotationAngle - PI/2)
        };

        if (IsKeyDown(KEY_UP)) {
            player.velocity.x = player.direction.x * SPEED;
            player.velocity.y = player.direction.y * SPEED;
        } else {
            player.velocity.x *= 0.95;
            player.velocity.y *= 0.95;
        }

        player.v1.x += player.velocity.x;
        player.v1.y += player.velocity.y;
        player.v2.x += player.velocity.x;
        player.v2.y += player.velocity.y;
        player.v3.x += player.velocity.x;
        player.v3.y += player.velocity.y;

        // Rotate the triangle around v1 (the nose of the spaceship)
        RotateTriangle(&player.v2, &player.v1, frameRotationAngle);
        RotateTriangle(&player.v3, &player.v1, frameRotationAngle);

        // get player to reappear on opposite side
        if (player.v1.x < 0 && player.v2.x < 0 && player.v3.x < 0) {
            player.v1.x += screenWidth;
            player.v2.x += screenWidth;
            player.v3.x += screenWidth;
        }
        if (player.v1.x > screenWidth && player.v2.x > screenWidth && player.v3.x > screenWidth) {
            player.v1.x -= screenWidth;
            player.v2.x -= screenWidth;
            player.v3.x -= screenWidth;
        }
        if (player.v1.y < 0 && player.v2.y < 0 && player.v3.y < 0) {
            player.v1.y += screenHeight;
            player.v2.y += screenHeight;
            player.v3.y += screenHeight;
        }
        if (player.v1.y > screenHeight && player.v2.y > screenHeight && player.v3.y > screenHeight) {
            player.v1.y -= screenHeight;
            player.v2.y -= screenHeight;
            player.v3.y -= screenHeight;
        }


        for (int i = 0; i < ballCount; i++) {
            // Move ball
            balls[i].position.x += balls[i].speed.x;
            balls[i].position.y += balls[i].speed.y;

            // Wall collisions with position correction
            if (balls[i].position.x >= (worldWidth - balls[i].radius)) {
                balls[i].position.x = worldWidth - balls[i].radius;  // safety check to make sure balls don't clip into walls
                balls[i].speed.x *= -1.0f;
            // subtracting BORDER from radius to add padding so balls go out of frame
            } else if (balls[i].position.x <= balls[i].radius - BORDER) {
                balls[i].position.x = balls[i].radius - BORDER;  
                balls[i].speed.x *= -1.0f;
            }

            if (balls[i].position.y >= (worldHeight - balls[i].radius)) {
                balls[i].position.y = worldHeight - balls[i].radius; 
                balls[i].speed.y *= -1.0f;
            } else if (balls[i].position.y <= balls[i].radius - BORDER) {
                balls[i].position.y = balls[i].radius - BORDER;  
                balls[i].speed.y *= -1.0f;
            }


            // Ball-ball collisions
            for (int j = i + 1; j < ballCount; j++) {
                float dx = balls[j].position.x - balls[i].position.x;
                float dy = balls[j].position.y - balls[i].position.y;
                float distanceSquared = dx * dx + dy * dy;
                float radiusSum = balls[i].radius + balls[j].radius;

                if (distanceSquared <= radiusSum * radiusSum) {
                    // Calculate normal
                    Vector2 normal = { dx, dy };
                    float distance = sqrtf(distanceSquared);
                    normal.x /= distance;
                    normal.y /= distance;

                    // Calculate relative velocity
                    Vector2 relativeVelocity = { balls[j].speed.x - balls[i].speed.x, balls[j].speed.y - balls[i].speed.y };

                    // Calculate the velocity along the normal
                    float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

                    // If the balls are moving away from each other, don't do anything
                    if (velocityAlongNormal > 0)
                        continue;

                    // Swap velocities along the normal direction
                    float restitution = 1.0f; // 1.0 for elastic collision (you can tweak it for less elastic collisions)
                    float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;

                    // Apply impulse to the balls
                    balls[i].speed.x -= impulseMagnitude * normal.x / 2;
                    balls[i].speed.y -= impulseMagnitude * normal.y / 2;
                    balls[j].speed.x += impulseMagnitude * normal.x / 2;
                    balls[j].speed.y += impulseMagnitude * normal.y / 2;

                    // Push balls apart slightly to avoid sticking or repeated collisions
                    float overlap = radiusSum - distance;
                    balls[i].position.x -= normal.x * (overlap / 2);
                    balls[i].position.y -= normal.y * (overlap / 2);
                    balls[j].position.x += normal.x * (overlap / 2);
                    balls[j].position.y += normal.y * (overlap / 2);
                }
            }

            // player - ball collisions
            float distX1 = player.v1.x - balls[i].position.x;
            float distY1 = player.v1.y - balls[i].position.y;
            float distSquare1 = distX1 * distX1 + distY1 * distY1;

            float distX2 = player.v2.x - balls[i].position.x;
            float distY2 = player.v2.y - balls[i].position.y;
            float distSquare2 = distX2 * distX2 + distY2 * distY2;

            float distX3 = player.v3.x - balls[i].position.x;
            float distY3 = player.v3.y - balls[i].position.y;
            float distSquare3 = distX3 * distX3 + distY3 * distY3;

            if(sqrt(distSquare1) < balls[i].radius) {
                balls[i].speed.x *= -1.0f;
                balls[i].speed.y *= -1.0f;
            }

        }

        BeginDrawing();
            ClearBackground(BLACK);  

            for(int i = 0; i < (sizeof(balls) / sizeof(balls[0])); i++) {
                DrawCircleV(balls[i].position, (float) balls[i].radius, WHITE);
            }

            DrawTriangle(player.v1, player.v2, player.v3, GOLD);

            DrawFPS(10, 10);    

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}
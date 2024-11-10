#include <raylib.h>
#include <math.h>
#include <stdlib.h>

const int BORDER = 500;
#define screenWidth 1280
#define worldWidth screenWidth + BORDER
#define screenHeight 720
#define worldHeight screenHeight + BORDER
#define ballCount sizeof(balls) / sizeof(balls[0])

const float SPEED = 10.0f;
int counter = 0;



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
    float health;
    bool hit;
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
        .velocity = { -1.1f, 1.1f },
        .direction = { 0, -1 },
        .health = 100.0f,
        .hit = false,
    };

    for(int i = 0; i < ballCount; i++) {
        balls[i].position = (Vector2) { GetRandomValue(20, worldWidth), GetRandomValue(20, worldHeight) };
        float temp_speed = (float) GetRandomValue(3, 8);
        balls[i].speed = (Vector2) { temp_speed, temp_speed };
        balls[i].radius = (float) GetRandomValue(15, 40);
    }
    
    float totalRotationAngle = 0.0f;
    // used for health bar positioning
    int top_bar = screenHeight / 4;
    int bottom_bar = screenHeight / 2;
    int new_top = top_bar;
    int new_bottom = bottom_bar;

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

        if (IsKeyDown(KEY_UP) && !player.hit) {
            player.velocity.x = player.direction.x * SPEED;
            player.velocity.y = player.direction.y * SPEED;
        } else {
            // if(player.velocity.x > 0.01 || player.velocity.x < -0.01) {
            //     player.velocity.x -= 0.01;
            //     player.velocity.y -= 0.01;
            //} 

            //this keeps the player's velocity from going too low and screwing up the math
            if (player.velocity.x < 1.1 && player.velocity.x > -1.1 && player.velocity.y < 1.1 && player.velocity.y > -1.1) {
                //do nothing
            } else {
                player.velocity.x *= 0.97;
                player.velocity.y *= 0.97;
            }
            // player.velocity.x *= 0.97;
            // player.velocity.y *= 0.97;
            // player.velocity.x -= 0.01;
            // player.velocity.y -= 0.01;
            
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
            // update balls position depending on its speed
            balls[i].position.x += balls[i].speed.x;
            balls[i].position.y += balls[i].speed.y;

            // check for wall collisions, react accordingly + safety check to avoid balls clipping into wall
            if (balls[i].position.x >= (worldWidth - balls[i].radius)) {
                balls[i].position.x = worldWidth - balls[i].radius; // set ball just outside wall (no clip)
                balls[i].speed.x *= -1.0f; // send ball in opposite x direction
            } else if (balls[i].position.x <= balls[i].radius - BORDER) {
                balls[i].position.x = balls[i].radius - BORDER;  
                balls[i].speed.x *= -1.0f; // send ball in opposity y direction
            }
            // same as before but for floor and ceiling
            if (balls[i].position.y >= (worldHeight - balls[i].radius)) {
                balls[i].position.y = worldHeight - balls[i].radius; 
                balls[i].speed.y *= -1.0f;
            } else if (balls[i].position.y <= balls[i].radius - BORDER) {
                balls[i].position.y = balls[i].radius - BORDER; 
                balls[i].speed.y *= -1.0f;
            }


            // loop through all balls, check for collisions, update speed accordingly
            for (int j = i + 1; j < ballCount; j++) {
                // getting the relative distance between two balls
                float dx = balls[j].position.x - balls[i].position.x;
                float dy = balls[j].position.y - balls[i].position.y;
                float distanceSquared = dx * dx + dy * dy;
                float radiusSum = balls[i].radius + balls[j].radius;
                // A^2(dx) + B^2(dy) = C^2(distanceSquared)
                // if C^2 (hypotenuse) is less than or equal to radius^2, collision!
                // we could finish by calling sqrt(), but that is costly and not needed here
                if (distanceSquared <= radiusSum * radiusSum) {
                    // once we know there is a collision, we need to figure out the new direction of the balls
                    // the relationship between two points can be described as a vector, which has direction and magnitude (length)
                    // the length in this case is the hypotenuse, which we don't care about, so we "normalize" the vector
                    // by setting it's length to something standard, like 1, (which is known as a unit vector)
                    // this makes it easier to deal with the part we do care about, the direction
                    Vector2 normal = { dx, dy };
                    float distance = sqrtf(distanceSquared); // hypotenuse/magnitude
                    normal.x /= distance;
                    normal.y /= distance;
                    // now we have a normalized vector, but you may ask yourself why we divided distanceX and distanceY
                    // by the sqrt of distanceSquared, and how does that make the magnitude 1? good question,
                    // if you 'unwravel' what we've done so far it actually adds up to one, in this case
                    // normal.x^2 + normal.y^2 = 1. 

                    // due to the way positions are updated in this simulation, it is possible to detect a collision,
                    // apply the new directions and speeds, loop through, and detect the collision a second time, either
                    // due to the balls slightly overlapping or any other error in calculation. To avoid applying the 'bounce'
                    // more than once to two balls, we can check if they are heading towards each other still, or if they are moving
                    // away from each other. if they are moving away, we do not need to do anything more, but if they are heading
                    // towards each other, we have not yet applied the bounce. To get this
                    Vector2 relativeVelocity = { balls[j].speed.x - balls[i].speed.x, balls[j].speed.y - balls[i].speed.y };
                    float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;


                    // if the balls are moving away from each other, don't do anything
                    if (velocityAlongNormal > 0)
                        continue;

                    // swap velocities along the normal direction
                    float restitution = 1.0f; // 1.0 for elastic collision (you can tweak it for less elastic collisions)
                    float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;

                    // apply impulse to the balls
                    balls[i].speed.x -= impulseMagnitude * normal.x / 2;
                    balls[i].speed.y -= impulseMagnitude * normal.y / 2;
                    balls[j].speed.x += impulseMagnitude * normal.x / 2;
                    balls[j].speed.y += impulseMagnitude * normal.y / 2;

                    // push balls apart slightly to avoid sticking
                    float overlap = radiusSum - distance;
                    balls[i].position.x -= normal.x * (overlap / 2);
                    balls[i].position.y -= normal.y * (overlap / 2);
                    balls[j].position.x += normal.x * (overlap / 2);
                    balls[j].position.y += normal.y * (overlap / 2);
                }
            }

            // player - ball collisions
            //this sucks in its current state
            float distX1 = player.v1.x - balls[i].position.x;
            float distY1 = player.v1.y - balls[i].position.y;
            float distSquare1 = distX1 * distX1 + distY1 * distY1;

            float distX2 = player.v2.x - balls[i].position.x;
            float distY2 = player.v2.y - balls[i].position.y;
            float distSquare2 = distX2 * distX2 + distY2 * distY2;

            float distX3 = player.v3.x - balls[i].position.x;
            float distY3 = player.v3.y - balls[i].position.y;
            float distSquare3 = distX3 * distX3 + distY3 * distY3;

            float distance1 = sqrt(distSquare1);
            float distance2 = sqrt(distSquare2);
            float distance3 = sqrt(distSquare3);
            // this works, but looks unnatural, I should try to push both player and ball away from eachother
            if(distance1 <= balls[i].radius) {
                player.hit = true;
                player.health -= 5;
                counter = 100;
                balls[i].speed.x *= -1.0f;
                balls[i].speed.y *= -1.0f;
                // get balls seperated from player a bit (may not be needed)
                Vector2 normal = { distX1, distY1 };
                normal.x /= distance1;
                normal.y /= distance1;

                if(player.velocity.x < 1.0f && player.velocity.x > -1.0f) {
                    if (player.velocity.x > 0) 
                        player.velocity.x = 1.1f;
                    if (player.velocity.x < 0)
                        player.velocity.x = -1.1f;
                }

                if(player.velocity.y < 1.0f && player.velocity.y > -1.0f) {
                    if (player.velocity.y > 0) 
                        player.velocity.y = 1.1f;
                    if (player.velocity.y < 0)
                        player.velocity.y = -1.1f;
                }

                Vector2 relativeVelocity = { player.velocity.x - balls[i].speed.x, player.velocity.y - balls[i].speed.y };
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;


                // if the balls are moving away from each other, don't do anything
                if (velocityAlongNormal > 0)
                    continue;

                // swap velocities along the normal direction
                float restitution = 1.0f; // 1.0 for elastic collision (you can tweak it for less elastic collisions)
                float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;

                // apply impulse to the balls
                balls[i].speed.x -= impulseMagnitude * normal.x / 2;
                balls[i].speed.y -= impulseMagnitude * normal.y / 2;
                player.velocity.x += impulseMagnitude * normal.x / 2;
                player.velocity.y += impulseMagnitude * normal.y / 2;

                float overlap = balls[i].radius - distance1;
                balls[i].position.x -= normal.x * (overlap / 2);
                balls[i].position.y -= normal.y * (overlap / 2);

                // frameRotationAngle = 1.0f;
                // totalRotationAngle += frameRotationAngle;
                // RotateTriangle(&player.v2, &player.v1, frameRotationAngle);
                // RotateTriangle(&player.v3, &player.v1, frameRotationAngle);
            } 
            if(distance2 <= balls[i].radius) {
                player.hit = true;
                player.health -= 5;
                counter = 100;
                balls[i].speed.x *= -1.0f;
                balls[i].speed.y *= -1.0f;
                Vector2 normal = { distX2, distY2 };
                normal.x /= distance2;
                normal.y /= distance2;

                if(player.velocity.x < 1.0f && player.velocity.x > -1.0f) {
                    if (player.velocity.x > 0) 
                        player.velocity.x = 1.1f;
                    if (player.velocity.x < 0)
                        player.velocity.x = -1.1f;
                }

                if(player.velocity.y < 1.0f && player.velocity.y > -1.0f) {
                    if (player.velocity.y > 0) 
                        player.velocity.y = 1.1f;
                    if (player.velocity.y < 0)
                        player.velocity.y = -1.1f;
                }

                Vector2 relativeVelocity = { player.velocity.x - balls[i].speed.x, player.velocity.y - balls[i].speed.y };
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;


                // if the balls are moving away from each other, don't do anything
                if (velocityAlongNormal > 0)
                    continue;

                // swap velocities along the normal direction
                float restitution = 1.0f; // 1.0 for elastic collision (you can tweak it for less elastic collisions)
                float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;

                // apply impulse to the balls
                balls[i].speed.x -= impulseMagnitude * normal.x / 2;
                balls[i].speed.y -= impulseMagnitude * normal.y / 2;
                player.velocity.x += impulseMagnitude * normal.x / 2;
                player.velocity.y += impulseMagnitude * normal.y / 2;

                float overlap = balls[i].radius - distance2;
                balls[i].position.x -= normal.x * (overlap / 2);
                balls[i].position.y -= normal.y * (overlap / 2);

            } 
            if(distance3 <= balls[i].radius) {
                player.hit = true;
                player.health -= 5;
                counter = 100;
                balls[i].speed.x *= -1.0f;
                balls[i].speed.y *= -1.0f;
                Vector2 normal = { distX3, distY3 };
                normal.x /= distance3;
                normal.y /= distance3;

                if(player.velocity.x < 1.0f && player.velocity.x > -1.0f) {
                    if (player.velocity.x > 0) 
                        player.velocity.x = 1.1f;
                    if (player.velocity.x < 0)
                        player.velocity.x = -1.1f;
                }

                if(player.velocity.y < 1.0f && player.velocity.y > -1.0f) {
                    if (player.velocity.y > 0) 
                        player.velocity.y = 1.1f;
                    if (player.velocity.y < 0)
                        player.velocity.y = -1.1f;
                }

                Vector2 relativeVelocity = { player.velocity.x - balls[i].speed.x, player.velocity.y - balls[i].speed.y };
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;


                // if the balls are moving away from each other, don't do anything
                if (velocityAlongNormal > 0)
                    continue;

                // swap velocities along the normal direction
                float restitution = 1.0f; // 1.0 for elastic collision (you can tweak it for less elastic collisions)
                float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;

                // apply impulse to the balls
                balls[i].speed.x -= impulseMagnitude * normal.x / 2;
                balls[i].speed.y -= impulseMagnitude * normal.y / 2;
                player.velocity.x += impulseMagnitude * normal.x / 2;
                player.velocity.y += impulseMagnitude * normal.y / 2;

                float overlap = balls[i].radius - distance3;
                balls[i].position.x -= normal.x * (overlap / 2);
                balls[i].position.y -= normal.y * (overlap / 2);
            }

            // if (player.hit) {
            //     for (int i = 0; i < 1000; i++) {
            //         frameRotationAngle = 1.0f;
            //         totalRotationAngle += frameRotationAngle;
            //         RotateTriangle(&player.v2, &player.v1, frameRotationAngle);
            //         RotateTriangle(&player.v3, &player.v1, frameRotationAngle);
            //     }
            //     player.hit = false;
            // }
        }

        BeginDrawing();
            ClearBackground(BLACK);  

            for(int i = 0; i < (sizeof(balls) / sizeof(balls[0])); i++) {
                DrawCircleV(balls[i].position, (float) balls[i].radius, WHITE);
            }

            if (player.hit) {
                // make player flash gold and red when hit to indicate damage
                if (counter > 0) {
                    if (counter % 10 == 0 || counter % 5 == 0) {
                        DrawTriangle(player.v1, player.v2, player.v3, GOLD);
                    } else {
                        DrawTriangle(player.v1, player.v2, player.v3, RED);
                    }
                    counter--;
                } 
                if (counter <= 0) {
                    player.hit = false;
                }
                
            }else {
                DrawTriangle(player.v1, player.v2, player.v3, GOLD);
            }

            // DrawText(TextFormat("player X velocity: %f", player.velocity.x), screenWidth / 2, 50, 25, WHITE);
            // DrawText(TextFormat("player Y velocity: %f", player.velocity.y), 150, 50, 25, WHITE);
            DrawText(TextFormat("HEALTH: %.0f", player.health), screenWidth / 7, 15, 20, WHITE);


            //new_top += new_top / (player.health / 100);
            // DrawRectangleLines(15, top_bar, screenWidth / 50, bottom_bar, GREEN);
            // DrawRectangle(15, new_top, screenWidth / 50, new_bottom, GREEN);
            //DrawRectangle()

            DrawRectangleLines(screenWidth / 4, 15, screenWidth / 2, screenHeight / 30, GREEN);
            DrawRectangle(screenWidth / 4, 15, (player.health / 100) * (screenWidth / 2), screenHeight / 30, GREEN);
            // game over
            if (player.health <= 0) {
                DrawRectangle(0, 0, screenWidth, screenHeight, RED);
                DrawText("GAME OVER", screenWidth / 3, screenHeight / 2, 50, WHITE);
            }

            //DrawFPS(10, 10);    

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}
// Made by Arktikus

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>

constexpr double GAME_UPDATE_INTERVAL = 0.2;
constexpr int TARGET_FPS = 60;

const int cellSize = 30;
const int cellCount = 25;
const int offset = 75;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 targetElement, const std::deque<Vector2>& snakeBody) {
    for(const Vector2& bodyPart : snakeBody) {
        if(Vector2Equals(bodyPart, targetElement)) {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake {
public:
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw() {
        for(const Vector2& bodyPart : body) {
            float x = bodyPart.x;
            float y = bodyPart.y;
            Rectangle segment = Rectangle{offset+x * cellSize, offset+y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if(addSegment == true) {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void Reset() {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food {
public:
    Vector2 position;
    Texture2D texture;

    Food(std::deque<Vector2> snakeBody) {
        Image image = LoadImage("../assets/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, offset+position.x * cellSize, offset+position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandomPos(const std::deque<Vector2>& snakeBody) {
        Vector2 position = GenerateRandomCell();
        while(ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);

    bool running = true;
    int score = 0;

    Sound eatSound;
    Sound wallSound;

    Game() {
        InitAudioDevice();
        SetMasterVolume(0.1);
        eatSound = LoadSound("../sounds/eat.mp3");
        wallSound = LoadSound("../sounds/wall.mp3");
    }

    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        if(running) {
            snake.Update();
            CheckFoodCollision();
            CheckEdgeCollision();
            CheckTailCollision();
        }
    }

    void CheckFoodCollision() {
        if(Vector2Equals(snake.body[0], food.position))  {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckEdgeCollision() {
    if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || 
        snake.body[0].y < 0 || snake.body[0].y >= cellCount) {
            GameOver();
    }
    }

    void CheckTailCollision() {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }

    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        PlaySound(wallSound);
        running = false;
        score = 0;
    }
};

int main() {
    std::cout << "Game is starting...\n";
    InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Snake Game - by Arktikus");
    SetTargetFPS(TARGET_FPS);

    Game game = Game();

    while(WindowShouldClose() == false) {
        
        BeginDrawing();

        // if(eventTriggered(gameUpdateInterval)) - The old speed.
        if(eventTriggered(std::max(0.1, GAME_UPDATE_INTERVAL - game.score * 0.01))) {
            game.Update();
        }

        if(IsKeyPressed(KEY_W) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_S) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_A) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_D) && game.snake.direction.x != -1) {
            game.snake.direction = {1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_SPACE) && game.running == false) {
            game.running = true;
        }

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize * (float)cellCount + 10, (float)cellSize * (float)cellCount + 10}, 5, darkGreen);
        DrawText("Snake Game", offset -5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset+cellSize*cellCount+10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
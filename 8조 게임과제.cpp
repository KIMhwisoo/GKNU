#include "raylib.h"
#include <cstdio>  // C++에서 sprintf_s를 사용하기 위해 추가

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_BRICK_SOUNDS 10 // 최대 사운드 수
Sound brickSounds[MAX_BRICK_SOUNDS]; // 사운드 배열
int soundIndex = 0;  // 현재 재생할 사운드 인덱스

// 게임 객체 구조체 정의
typedef struct Paddle {
    Rectangle rect;
    float speed;
} Paddle;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
} Ball;

typedef struct Block {
    Rectangle rect;
    bool active;
} Block;

void InitBlocks(Block blocks[], int rows, int cols, int blockWidth, int blockHeight);

Sound defeatSound;

int main(void) {
    // 게임 초기화
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");
    InitAudioDevice();  // 오디오 장치 초기화
    SetTargetFPS(60);


    // 배경 음악 로드
    Music backgroundMusic = LoadMusicStream("C:/Users/dambi/Desktop/mariomusic.mp3"); //경로 변경 C:/Users/유저네임/Desktop/mariomusic.mp3
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.2f);

    // 효과음 로드
    for (int i = 0; i < MAX_BRICK_SOUNDS; i++) {
        brickSounds[i] = LoadSound("C:/Users/dambi/Desktop/brick.mp3");  //경로 변경
        SetSoundVolume(brickSounds[i], 0.1f);
    }

    // 패배 효과음 로드
    defeatSound = LoadSound("C:/Users/dambi/Desktop/defeat.mp3");//경로 변경
    SetSoundVolume(defeatSound, 0.5f);

    // 패들 초기화
    Paddle paddle = { { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20 }, 5.0f };

    // 공 초기화
    Ball ball = { { SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50 }, { 5.0f, -5.0f }, 10.0f };

    // 블록 초기화
    const int rows = 5;
    const int cols = 10;
    const int blockWidth = 60;
    const int blockHeight = 20;
    Block blocks[rows * cols];
    InitBlocks(blocks, rows, cols, blockWidth, blockHeight);

    int score = 0;
    bool gameOver = false;
    bool startScreen = true;  // 게임 시작 화면을 표시할 변수

    while (!WindowShouldClose()) {
        // 음악 스트림 업데이트
        UpdateMusicStream(backgroundMusic);

        // 게임 로직
        if (startScreen) {
            // 시작 화면 표시: 텍스트 없이 그냥 하얀 배경만
            BeginDrawing();
            ClearBackground(RAYWHITE);  // 하얀 배경만

            // 텍스트를 그려서 시작 화면 표시
            DrawText("Breakout Game", 300, 200, 40, BLACK);
            DrawText("Press Enter to Start", 270, 250, 20, DARKGRAY);

            EndDrawing();

            // "Enter" 키를 눌러서 게임 시작
            if (IsKeyPressed(KEY_ENTER)) {
                startScreen = false;  // 게임 시작
            }
        }
        else {
            if (!gameOver) {
                // 패들 이동
                if (IsKeyDown(KEY_LEFT) && paddle.rect.x > 0) {
                    paddle.rect.x -= paddle.speed;
                }
                if (IsKeyDown(KEY_RIGHT) && paddle.rect.x < SCREEN_WIDTH - paddle.rect.width) {
                    paddle.rect.x += paddle.speed;
                }

                // 공 이동
                ball.position.x += ball.speed.x;
                ball.position.y += ball.speed.y;

                // 공과 벽 충돌 처리
                if (ball.position.x - ball.radius <= 0 || ball.position.x + ball.radius >= SCREEN_WIDTH) {
                    ball.speed.x *= -1;
                }
                if (ball.position.y - ball.radius <= 0) {
                    ball.speed.y *= -1;
                }

                // 공과 패들 충돌 처리
                if (CheckCollisionCircleRec(ball.position, ball.radius, paddle.rect)) {
                    ball.speed.y *= -1;
                    ball.position.y = paddle.rect.y - ball.radius;  // 패들과 충돌 시 공의 위치 조정
                }

                // 공과 블록 충돌 처리
                for (int i = 0; i < rows * cols; i++) {
                    if (blocks[i].active && CheckCollisionCircleRec(ball.position, ball.radius, blocks[i].rect)) {
                        ball.speed.y *= -1;
                        blocks[i].active = false;  // 블록 제거
                        score += 10;  // 점수 추가

                        // 사운드 재생 (순차적으로)
                        PlaySound(brickSounds[soundIndex]);
                        soundIndex = (soundIndex + 1) % MAX_BRICK_SOUNDS;  // 인덱스 순차적으로 증가
                    }
                }

                // 공이 화면 아래로 떨어졌을 때 게임 종료
                if (ball.position.y + ball.radius > SCREEN_HEIGHT) {
                    gameOver = true;
                    StopMusicStream(backgroundMusic);
                    // 게임 오버 효과음 재생
                    PlaySound(defeatSound);  // 효과음 재생
                }

                // 블록이 모두 사라졌을 때 게임 종료
                bool allBlocksCleared = true;
                for (int i = 0; i < rows * cols; i++) {
                    if (blocks[i].active) {
                        allBlocksCleared = false;
                        break;
                    }
                }
                if (allBlocksCleared) {
                    gameOver = true;
                }
            }

            // 화면 그리기
            BeginDrawing();

            ClearBackground(RAYWHITE);  // 게임 화면 배경은 하얀색으로 설정


            if (gameOver) {
                // 게임 오버 화면
                DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 40, DARKGRAY);
                DrawText("Press Enter to Restart", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 + 20, 20, DARKGRAY);
            }
            else {
                // 게임 중에는 패들, 공, 블록 그리기
                DrawRectangleRec(paddle.rect, BLUE);
                DrawCircleV(ball.position, ball.radius, RED);

                // 블록 그리기
                for (int i = 0; i < rows * cols; i++) {
                    if (blocks[i].active) {
                        DrawRectangleRec(blocks[i].rect, GREEN);
                    }
                }

                // 점수 표시
                char scoreText[20];
                sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);
                DrawText(scoreText, 10, 10, 20, DARKGRAY);
            }


            // 패들 그리기
            DrawRectangleRec(paddle.rect, BLUE);

            // 공 그리기
            DrawCircleV(ball.position, ball.radius, RED);

            // 블록 그리기
            for (int i = 0; i < rows * cols; i++) {
                if (blocks[i].active) {
                    DrawRectangleRec(blocks[i].rect, GREEN);
                }
            }

            // 점수 표시 (sprintf_s 사용)
            char scoreText[20];
            sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);  // sprintf_s 사용
            DrawText(scoreText, 10, 10, 20, DARKGRAY);

            EndDrawing();

            // 게임 오버 시 Enter 키로 게임 재시작
            if (gameOver && IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                ball.position = { SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50 };  // 중괄호 사용
                ball.speed = { 5.0f, -5.0f };  // 중괄호 사용
                score = 0;
                InitBlocks(blocks, rows, cols, blockWidth, blockHeight);
                // 'defeatSound'가 재생 중이면 멈추기
                StopSound(defeatSound);
                PlayMusicStream(backgroundMusic);  // 음악 재생
                SetMusicVolume(backgroundMusic, 0.2f);
            }
        }
    }

    // 게임 종료
    UnloadMusicStream(backgroundMusic);  // 음악 스트림 해제
    CloseAudioDevice();  // 오디오 장치 종료
    for (int i = 0; i < MAX_BRICK_SOUNDS; i++) {
        UnloadSound(brickSounds[i]);
    } //효과음 종료
    CloseWindow();
    return 0;
}

void InitBlocks(Block blocks[], int rows, int cols, int blockWidth, int blockHeight) {
    for (int i = 0; i < rows * cols; i++) {
        blocks[i].rect.x = (i % cols) * (blockWidth + 5) + 50;
        blocks[i].rect.y = (i / cols) * (blockHeight + 5) + 50;
        blocks[i].rect.width = blockWidth;
        blocks[i].rect.height = blockHeight;
        blocks[i].active = true;
    }
}

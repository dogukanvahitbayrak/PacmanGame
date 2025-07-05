// PAC-MAN CLONE USING RAYLIB
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ROWS 30
#define MAX_COLS 32
#define MAX_GHOSTS 6

// Game states
typedef enum {
    MENU,
    GAME,
    GAMEOVER,
    GAMEWIN,
    PAUSED
} GameState;

GameState state = MENU;

// Pac-Man position and direction
int pacmanX = 0, pacmanY = 0;
int startPacmanX = 0, startPacmanY = 0;
int pacmanDirection = 0; // 0-right, 1-left, 2-up, 3-down

// Level tracking
int currentLevel = 1;

// Movement timers
float ghostMoveTimer = 0;
float ghostMoveDelay = 0.6f;
float pacmanMoveTimer = 0.0f;
float pacmanMoveDelay = 0.12f;

// Map and gameplay data
int mapRows = 0, mapCols = 0;
static char map[MAX_ROWS][MAX_COLS];
int score = 0;
int highScore = 0;
int lives = 3;
bool pause = false;
bool gameOver = false;
bool gameWon = false;

// Power-up system
bool isPoweredUp = false;
float powerUpTimer = 0.0f;
const float POWER_UP_DURATION = 1.0f;

// Ghost data
typedef struct {
    int x, y;
} Ghost;

Ghost ghosts[MAX_GHOSTS];
Ghost startGhosts[MAX_GHOSTS];
int ghostCount = 0;

// Invincibility after collision
float invincibleTimer = 0.0f;
const float INVINCIBLE_DURATION = 0.5f;

// Sound and music
Music backgroundMusic;
Sound gameOverSound;

// Check if a ghost exists at given coordinates
bool IsGhostAt(int x, int y) {
    for (int i = 0; i < ghostCount; i++)
        if (ghosts[i].x == x && ghosts[i].y == y) return true;
    return false;
}

// Draw a ghost with eyes
void DrawGhost(int x, int y, Color color) {
    
    DrawCircle(x + 16, y + 16, 16, color);
    DrawRectangle(x, y + 16, 32, 16, color);

    DrawCircle(x + 10, y + 10, 4, WHITE);
    DrawCircle(x + 22, y + 10, 4, WHITE);

    DrawCircle(x + 10, y + 10, 2, BLACK);
    DrawCircle(x + 22, y + 10, 2, BLACK);
}

// Draw game level including map, player, ghosts and UI
void DrawLevel(void) {
    ClearBackground(DARKGRAY);

    for (int y = 0; y < mapRows; y++) {
        for (int x = 0; x < mapCols; x++) {
            char tile = map[y][x];
            if (tile == '#') {
                DrawRectangle(x * 32, y * 32, 32, 32, BLUE);
            } else if (tile == '.') {
                DrawCircle(x * 32 + 16, y * 32 + 16, 4, YELLOW);
            } else if (tile == '*') {
                DrawCircle(x * 32 + 16, y * 32 + 16, 8, SKYBLUE);
            }
        }
    }

    float startAngle = 30.0f, endAngle = 330.0f;
    if (pacmanDirection == 1) { startAngle = 210; endAngle = 150 + 360; }
    else if (pacmanDirection == 2) { startAngle = 120; endAngle = 60 + 360; }
    else if (pacmanDirection == 3) { startAngle = 300; endAngle = 240 + 360; }

    Color pacColor = isPoweredUp ? WHITE : YELLOW;
    DrawCircleSector((Vector2){ pacmanX * 32 + 16, pacmanY * 32 + 16 }, 16, startAngle, endAngle, 32, pacColor);

    Color ghostColors[MAX_GHOSTS] = { RED, GREEN, ORANGE, PURPLE, WHITE, BROWN };
    for (int i = 0; i < ghostCount; i++) {
        Color color = ghostColors[i % MAX_GHOSTS];
        DrawGhost(ghosts[i].x * 32, ghosts[i].y * 32, color);
    }

    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
    DrawText(TextFormat("High Score: %d", highScore), 10, 35, 20, PINK);
    DrawText(TextFormat("Lives: %d", lives), 10, 60, 20, RED);
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 80, 20, GREEN);

    if (gameOver) {
        DrawText(gameWon ? "GAME WIN!" : "GAME OVER", 400, 200, 40, gameWon ? GREEN : BLACK);
    }

    DrawText("Press P to Pause/Resume", 800, 10, 20, WHITE);
    if (pause && !gameOver) {
        DrawText("PAUSED", 450, 250, 40, LIGHTGRAY);
    }
}

// Draws the animated main menu
void DrawMenu(void) {
    ClearBackground(DARKBLUE);
    DrawText("PAC-MAN", 401, 101, 60, BLACK);
    DrawText("PAC-MAN", 400, 100, 60, YELLOW);
    DrawRectangle(360, 250, 380, 50, DARKGRAY);
    DrawText("PRESS ENTER TO START", 370, 260, 20, WHITE);
    DrawRectangle(360, 320, 380, 50, DARKGRAY);
    DrawText("PRESS ESC TO QUIT", 390, 330, 20, LIGHTGRAY);

    float time = GetTime();
    float angleOffset = 30 * sinf(time * 5);
    DrawCircleSector((Vector2){600, 200}, 40, angleOffset, 360 - angleOffset, 32, YELLOW);
}



void LoadLevel(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Harita dosyası açılamadı: %s\n", filename);
        return;
    }

    char line[MAX_COLS];
    int row = 0;
    ghostCount = 0;

    while (fgets(line, sizeof(line), file)) {
        int col = 0;
        while (line[col] != '\n' && line[col] != '\0') {
            map[row][col] = line[col];
            if (line[col] == 'P') {
                pacmanX = startPacmanX = col;
                pacmanY = startPacmanY = row;
            }
            if (line[col] == 'G' && ghostCount < MAX_GHOSTS) {
                ghosts[ghostCount].x = col;
                ghosts[ghostCount].y = row;
                

                // Başlangıç konumunu kaydet
                startGhosts[ghostCount] = ghosts[ghostCount];

               ghostCount++;
              }
            col++;
        }
        if (col > mapCols) mapCols = col;
        row++;
        if (row >= MAX_ROWS) break;
    }

    mapRows = row;
    fclose(file);
}

void LoadLevelByNumber(int level) {
    char filename[64];
    sprintf(filename, "levels/level%d.txt", level);
    LoadLevel(filename);
}

void PrintLevelMap() {
    for (int y = 0; y < mapRows; y++) {
        printf("%s\n", map[y]);
    }
}

void UpdateGhosts() {
    for (int i = 0; i < ghostCount; i++) {
        int bestX = ghosts[i].x;
        int bestY = ghosts[i].y;
        int minDistance = 9999;

        int directions[4][2] = {
            {1, 0},   // Sağ
            {-1, 0},  // Sol
            {0, -1},  // Yukarı
            {0, 1}    // Aşağı
        };

        for (int d = 0; d < 4; d++) {
            int nx = ghosts[i].x + directions[d][0];
            int ny = ghosts[i].y + directions[d][1];

            // Harita sınırı içinde mi ve duvar değil mi ve başka hayalet yok mu?
            if (nx >= 0 && nx < mapCols && ny >= 0 && ny < mapRows && map[ny][nx] != '#' && !IsGhostAt(nx, ny)) {

                int dx = pacmanX - nx;
                int dy = pacmanY - ny;
                int distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);  // Manhattan mesafesi

                if (distance < minDistance) {
                    minDistance = distance;
                    bestX = nx;
                    bestY = ny;
                }
            }
        }

        ghosts[i].x = bestX;
        ghosts[i].y = bestY;
    }
}

void CheckCollisionWithGhosts() {
    if (isPoweredUp) return; // power-up varsa hayaletten etkilenme
    if (invincibleTimer > 0.0f) return;

    for (int i = 0; i < ghostCount; i++) {
        if (ghosts[i].x == pacmanX && ghosts[i].y == pacmanY) {
            lives--;
            invincibleTimer = INVINCIBLE_DURATION;

            if (lives <= 0) {
                gameOver = true;
                PlaySound(gameOverSound);
            } else {
                // Pac-Man başlangıç pozisyonuna dönsün
                pacmanX = startPacmanX;
                pacmanY = startPacmanY;

                // Hayaletler de başlangıca dönsün
                for (int j = 0; j < ghostCount; j++) {
                    ghosts[j] = startGhosts[j];
                }
            }

            break;
        }
    }
}

bool AreAllDotsEaten() {
    for (int y = 0; y < mapRows; y++)
        for (int x = 0; x < mapCols; x++)
            if (map[y][x] == '.') return false;
    return true;
}

void UpdateGame() {
    int newX = pacmanX, newY = pacmanY;
    pacmanMoveTimer += GetFrameTime();
    if (pacmanMoveTimer < pacmanMoveDelay) return;
    pacmanMoveTimer = 0;

    if (invincibleTimer > 0.0f) {
        invincibleTimer -= GetFrameTime();
        printf("invincibleTimer = %f\n", invincibleTimer);
        if (invincibleTimer < 0.0f) invincibleTimer = 0.0f;
    }

    if (IsKeyDown(KEY_RIGHT)) { newX++; pacmanDirection = 0; }
    if (IsKeyDown(KEY_LEFT))  { newX--; pacmanDirection = 1; }
    if (IsKeyDown(KEY_UP))    { newY--; pacmanDirection = 3; }
    if (IsKeyDown(KEY_DOWN))  { newY++; pacmanDirection = 2; }

    if (newX >= 0 && newX < mapCols && newY >= 0 && newY < mapRows) {
        if (map[newY][newX] != '#') {
            pacmanX = newX;
            pacmanY = newY;
            if (map[newY][newX] == '.') {
                map[newY][newX] = ' ';
                score += 10;
            }
            else if (map[newY][newX] == '*') {
                map[newY][newX] = ' ';
                isPoweredUp = true;
                powerUpTimer = POWER_UP_DURATION;
                score += 50;
            }
        }
    }
     if (isPoweredUp) {
    powerUpTimer -= GetFrameTime();
    if (powerUpTimer <= 0.0f) {
        isPoweredUp = false;
    }
    }
    if (!gameOver) CheckCollisionWithGhosts();

   if (AreAllDotsEaten()) {
    currentLevel++;
    if (currentLevel > 4) {
        gameOver = true;
        gameWon = true; 
    } else {
        LoadLevelByNumber(currentLevel);
    }
 }
    if (lives <= 0) {
    gameOver = true;
 }
 if (gameOver) {
    StopMusicStream(backgroundMusic); 
    PlaySound(gameOverSound);         
 }
}

// Main entry point
int main(void) {
   



    InitWindow(1100, 600, "Pac-Man");
    InitAudioDevice(); 
    
    backgroundMusic = LoadMusicStream("assets/415806__sunsai__desert-background-music.wav");
    gameOverSound = LoadSound("assets/76376__deleted_user_877451__game_over.wav");

    PlayMusicStream(backgroundMusic); 
    SetTargetFPS(60);

    LoadLevelByNumber(currentLevel);


    FILE *file = fopen("score.txt", "r");
    if (file) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    }

  while (!WindowShouldClose()) {
    UpdateMusicStream(backgroundMusic);
    BeginDrawing();
    ClearBackground(DARKGRAY);

    if (state == MENU) {
        DrawMenu();
        if (IsKeyPressed(KEY_ENTER)) {
            state = GAME;
            score = 0;
            lives = 3;
            currentLevel = 1;
            gameOver = false;
            gameWon = false;
            pause = false;
            LoadLevelByNumber(currentLevel);
            SeekMusicStream(backgroundMusic, 0.0f);
            PlayMusicStream(backgroundMusic);
        }

    }

    else if (state == GAME) {
        if (IsKeyPressed(KEY_P)) pause = !pause;

        if (!pause && !gameOver) {
            UpdateGame();
            ghostMoveTimer += GetFrameTime();
            if (ghostMoveTimer >= ghostMoveDelay) {
                UpdateGhosts();
                ghostMoveTimer = 0;
            }
        }

        DrawLevel();

        if (pause && !gameOver) {
            DrawText("PAUSED", 450, 250, 40, LIGHTGRAY);
        }

        DrawText("Press P to Pause/Resume", 800, 10, 20, WHITE);

        if (gameOver) {
            state = gameWon ? GAMEWIN : GAMEOVER;
        }
    }

    else if (state == GAMEOVER) {
        DrawLevel();
        DrawText("GAME OVER", 400, 250, 40, RED);
        DrawText("Press SPACE to return to menu", 300, 300, 20, WHITE);
        if (IsKeyPressed(KEY_SPACE)) state = MENU;
    }

    else if (state == GAMEWIN) {
        DrawLevel();
        DrawText("YOU WIN!", 400, 250, 40, GREEN);
        DrawText("Press SPACE to return to menu", 300, 300, 20, WHITE);
        if (IsKeyPressed(KEY_SPACE)) state = MENU;
    }

    EndDrawing();
}



    if (score > highScore) {
        FILE *file = fopen("score.txt", "w");
        if (file) {
            fprintf(file, "%d", score);
            fclose(file);
        }
    }
    CloseWindow();
    return 0;
}


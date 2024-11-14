#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "menu.h"

#define MAP_WIDTH 40
#define MAP_HEIGHT 20
#define MAX_ENEMIES 5  // Número de inimigos
#define MAX_FIREBALLS 5  // Número máximo de bolas de fogo

// Cores para os elementos do mapa
#define COLOR_WALL RED
#define COLOR_DOOR YELLOW
#define COLOR_FLOOR LIGHTGRAY
#define COLOR_PLAYER GREEN
#define COLOR_ENEMY MAGENTA
#define COLOR_ATTACK CYAN
#define COLOR_FIREBALL RED

// Definição do mapa ampliado
char map[MAP_HEIGHT][MAP_WIDTH] = {
    "########################################",
    "#          D                     D    #",
    "#                                     #",
    "#                                     #",
    "#         D            #              #",
    "#                                     #",
    "#       D                             #",
    "#                                     #",
    "#                                     #",
    "#                         D           #",
    "#                                     #",
    "#                                     #",
    "#   D                                 #",
    "#                                     #",
    "#                                     #",
    "#                      D              #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "########################################"
};

// Posição inicial do jogador
int playerX = 1;
int playerY = 1;

// Estrutura para os inimigos
typedef struct {
    int x, y;
    int alive;
} Enemy;

// Estrutura para as bolas de fogo
typedef struct {
    int x, y;
    int active;
    time_t createdAt;
    int direction;
} Fireball;

Enemy enemies[MAX_ENEMIES] = {
    {5, 5, 1}, {8, 2, 1}, {15, 7, 1}, {30, 15, 1}, {35, 10, 1}
};

Fireball fireballs[MAX_FIREBALLS];

// Função para desenhar o mapa
void screenDrawMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char cell = map[y][x];

            // Escolhe a cor para cada tipo de célula
            switch(cell) {
                case '#':
                    screenSetColor(COLOR_WALL, BLACK); break;
                case 'D':
                    screenSetColor(COLOR_DOOR, BLACK); break;
                default:
                    screenSetColor(COLOR_FLOOR, BLACK); break;
            }

            screenGotoxy(x, y);
            printf("%c", cell);
        }
    }
    screenSetColor(WHITE, BLACK);
    fflush(stdout);
}

// Função para desenhar o jogador
void PrintMago() {
    screenSetColor(COLOR_PLAYER, BLACK);
    screenGotoxy(playerX, playerY);
    printf("🧙");
    fflush(stdout);
}

// Função para desenhar os inimigos
void drawEnemies() {
    screenSetColor(COLOR_ENEMY, BLACK);
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive) {
            screenGotoxy(enemies[i].x, enemies[i].y);
            printf("E");
        }
    }
    fflush(stdout);
}

// Função para mover o jogador
void movePlayer(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;

    if (map[newY][newX] != '#') {
        screenGotoxy(playerX, playerY);
        printf(" ");

        playerX = newX;
        playerY = newY;

        PrintMago();
    }
}

// Função para verificar se uma posição está ocupada por outro inimigo
int isOccupiedByEnemy(int x, int y) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive && enemies[i].x == x && enemies[i].y == y) {
            return 1;
        }
    }
    return 0;
}

// Função para mover inimigos em direção ao jogador
void moveEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;

        screenGotoxy(enemies[i].x, enemies[i].y);
        printf(" ");

        int newX = enemies[i].x;
        int newY = enemies[i].y;

        if (enemies[i].x < playerX && map[enemies[i].y][enemies[i].x + 1] != '#' && !isOccupiedByEnemy(enemies[i].x + 1, enemies[i].y)) {
            newX++;
        } else if (enemies[i].x > playerX && map[enemies[i].y][enemies[i].x - 1] != '#' && !isOccupiedByEnemy(enemies[i].x - 1, enemies[i].y)) {
            newX--;
        }

        if (enemies[i].y < playerY && map[enemies[i].y + 1][enemies[i].x] != '#' && !isOccupiedByEnemy(enemies[i].x, enemies[i].y + 1)) {
            newY++;
        } else if (enemies[i].y > playerY && map[enemies[i].y - 1][enemies[i].x] != '#' && !isOccupiedByEnemy(enemies[i].x, enemies[i].y - 1)) {
            newY--;
        }

        enemies[i].x = newX;
        enemies[i].y = newY;
    }
    drawEnemies();
}

// Função para exibir feedback visual do ataque ao redor do jogador
void showAttackFeedback() {
    screenSetColor(COLOR_ATTACK, BLACK);
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int x = playerX + dx;
            int y = playerY + dy;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT && map[y][x] != '#') {
                screenGotoxy(x, y);
                printf("*");
            }
        }
    }
    fflush(stdout);
}

// Função para ataque com feedback visual
void playerAttack() {
    showAttackFeedback();

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;

        if (abs(enemies[i].x - playerX) <= 1 && abs(enemies[i].y - playerY) <= 1) {
            screenGotoxy(enemies[i].x, enemies[i].y);
            printf(" ");
            enemies[i].alive = 0;
        }
    }

    screenGotoxy(playerX, playerY);
    PrintMago();
    
    screenDrawMap();
    PrintMago();
    drawEnemies();
}

// Inicializa as bolas de fogo
void initFireballs() {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        fireballs[i].active = 0;
    }
}

// Cria uma nova bola de fogo
void createFireball() {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) {
            // Limpa a posição anterior da bola de fogo
            screenGotoxy(fireballs[i].x, fireballs[i].y);
            printf(" ");
            
            // Define a posição inicial da bola de fogo uma posição à frente do mago
            fireballs[i].x = playerX + 1;
            fireballs[i].y = playerY;
            
            // Verifica se a posição é válida (não é parede)
            if (map[fireballs[i].y][fireballs[i].x] == '#') {
                continue;  // Não cria a bola de fogo se estiver bloqueada por parede
            }
            
            fireballs[i].active = 1;
            fireballs[i].createdAt = time(NULL);
            fireballs[i].direction = 1;
            
            // Desenha a bola de fogo em sua posição inicial
            screenSetColor(COLOR_FIREBALL, BLACK);
            screenGotoxy(fireballs[i].x, fireballs[i].y);
            printf("🔥");
            screenSetColor(WHITE, BLACK);
            
            // Mantém o mago em sua posição
            PrintMago();
            break;
        }
    }
}

// Atualiza a posição das bolas de fogo e verifica colisões
void updateFireballs() {
    time_t currentTime = time(NULL);
    
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (fireballs[i].active) {
            // Limpa a posição atual da bola de fogo
            screenGotoxy(fireballs[i].x, fireballs[i].y);
            printf(" ");

            // Verifica se a bola de fogo expirou (2 segundos)
            if (difftime(currentTime, fireballs[i].createdAt) >= 2) {
                fireballs[i].active = 0;
                continue;
            }

            // Calcula a nova posição
            int newX = fireballs[i].x + fireballs[i].direction;

            // Verifica colisão com parede
            if (map[fireballs[i].y][newX] == '#') {
                fireballs[i].active = 0;
                continue;
            }

            // Verifica colisão com inimigos
            int hitEnemy = 0;
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].alive && enemies[j].x == newX && enemies[j].y == fireballs[i].y) {
                    enemies[j].alive = 0;
                    screenGotoxy(enemies[j].x, enemies[j].y);
                    printf(" ");
                    fireballs[i].active = 0;
                    hitEnemy = 1;
                    break;
                }
            }

            // Se não houve colisão, move a bola de fogo
            if (fireballs[i].active && !hitEnemy) {
                fireballs[i].x = newX;
                screenSetColor(COLOR_FIREBALL, BLACK);
                screenGotoxy(fireballs[i].x, fireballs[i].y);
                printf("🔥");
                screenSetColor(WHITE, BLACK);
            }
        }
    }
}

int main() {
    displayOpeningArt();
    keyboardInit();
    screenInit(0);
    
    initFireballs();
    
    screenDrawMap();
    PrintMago();
    drawEnemies();

    time_t lastEnemyMove = time(NULL);

    while (1) {
        if (keyhit()) {
            char key = readch();

            switch (key) {
                case 'w': movePlayer(0, -1); break;
                case 's': movePlayer(0, 1); break;
                case 'a': movePlayer(-1, 0); break;
                case 'd': movePlayer(1, 0); break;
                case 'f': createFireball(); break;
                case ' ': playerAttack(); break;
                case 'q':
                    keyboardDestroy();
                    screenDestroy();
                    return 0;
            }
        }

        updateFireballs();

        if (difftime(time(NULL), lastEnemyMove) >= 1) {
            moveEnemies();
            lastEnemyMove = time(NULL);
        }

        screenGotoxy(0, MAP_HEIGHT);
        fflush(stdout);
    }
}
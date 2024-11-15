#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include <unistd.h>
#include "menu.h"

#define MAP_WIDTH 60   // Novo tamanho da largura do mapa
#define MAP_HEIGHT 30  // Novo tamanho da altura do mapa
#define MAX_ENEMIES 1  // Número de inimigos
#define MAX_FIREBALLS 5

// Cores para os elementos do mapa
#define COLOR_WALL RED
#define COLOR_DOOR YELLOW
#define COLOR_FLOOR LIGHTGRAY
#define COLOR_PLAYER GREEN
#define COLOR_ENEMY MAGENTA
#define COLOR_ATTACK CYAN
#define COLOR_FIREBALL RED
#define ENEMY_MOVE_INTERVAL 1000
#define FIREBALL_MOVE_INTERVAL 100  

// Definição do mapa ampliado
char map[MAP_HEIGHT][MAP_WIDTH] = {
    "############################################################",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "#                                                          #",
    "############################################################"
};


// Posição inicial do jogador
int playerX = 30;
int playerY = 10;

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
    {5, 5, 1}
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
            printf("🦹");
        }
    }
    fflush(stdout);
}


// Função para mover o jogador
// Função para mover o jogador
void movePlayer(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;

    // Verifica se o movimento é para uma posição válida
    if (map[newY][newX] != '#') {
        // Limpa a posição antiga do mago
        screenGotoxy(playerX, playerY);
        printf(" ");

        // Atualiza a posição do jogador
        playerX = newX;
        playerY = newY;

        // Desenha o mago na nova posição
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

        // Adiciona uma pequena chance de o inimigo se mover de maneira aleatória
        if (rand() % 4 == 0) {  // 25% de chance de se mover aleatoriamente
            int randDirection = rand() % 4;
            switch(randDirection) {
                case 0: if (map[newY][newX + 1] != '#' && !isOccupiedByEnemy(newX + 1, newY)) newX++; break; // direita
                case 1: if (map[newY][newX - 1] != '#' && !isOccupiedByEnemy(newX - 1, newY)) newX--; break; // esquerda
                case 2: if (map[newY + 1][newX] != '#' && !isOccupiedByEnemy(newX, newY + 1)) newY++; break; // abaixo
                case 3: if (map[newY - 1][newX] != '#' && !isOccupiedByEnemy(newX, newY - 1)) newY--; break; // acima
            }
        } else {  // Movimento padrão em direção ao jogador
            if (enemies[i].x < playerX && map[newY][newX + 1] != '#' && !isOccupiedByEnemy(newX + 1, newY)) {
                newX++;
            } else if (enemies[i].x > playerX && map[newY][newX - 1] != '#' && !isOccupiedByEnemy(newX - 1, newY)) {
                newX--;
            }

            if (enemies[i].y < playerY && map[newY + 1][newX] != '#' && !isOccupiedByEnemy(newX, newY + 1)) {
                newY++;
            } else if (enemies[i].y > playerY && map[newY - 1][newX] != '#' && !isOccupiedByEnemy(newX, newY - 1)) {
                newY--;
            }
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
void updateFireballs(struct timespec *lastFireballMove) {
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    long elapsed_ms = (currentTime.tv_sec - lastFireballMove->tv_sec) * 1000 +
                      (currentTime.tv_nsec - lastFireballMove->tv_nsec) / 1000000;

    if (elapsed_ms < FIREBALL_MOVE_INTERVAL) {
        return;  // Se ainda não passou o intervalo, não move as bolas de fogo
    }
    
    *lastFireballMove = currentTime;  // Atualiza o último tempo de movimento

    time_t currentTimeSimple = time(NULL);
    
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (fireballs[i].active) {
            // Limpa a posição atual da bola de fogo
            screenGotoxy(fireballs[i].x, fireballs[i].y);
            printf(" ");

            // Verifica se a bola de fogo expirou (2 segundos)
            if (difftime(currentTimeSimple, fireballs[i].createdAt) >= 2) {
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
    // Redesenha o mago na posição atual
    PrintMago();
}



int main() {
    displayOpeningArt();
    keyboardInit();
    screenInit(0);

    initFireballs();

    screenDrawMap();
    PrintMago();
    drawEnemies();

    struct timespec lastEnemyMove, currentTime;
    clock_gettime(CLOCK_MONOTONIC, &lastEnemyMove); // Inicializa o tempo de última movimentação do inimigo

    struct timespec lastFireballMove;
    clock_gettime(CLOCK_MONOTONIC, &lastFireballMove);

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

        updateFireballs(&lastFireballMove);

        // Atualiza o tempo e move o inimigo se o intervalo for atingido
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        long elapsed_ms = (currentTime.tv_sec - lastEnemyMove.tv_sec) * 1000 +
                          (currentTime.tv_nsec - lastEnemyMove.tv_nsec) / 1000000;

        if (elapsed_ms >= ENEMY_MOVE_INTERVAL) {
            moveEnemies();
            lastEnemyMove = currentTime; // Atualiza o tempo de última movimentação
        }

        usleep(1000);  // Reduz uso de CPU com pausa curta (1 ms)

        screenGotoxy(0, MAP_HEIGHT);
        fflush(stdout);
    }
}

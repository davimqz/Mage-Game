/**
 * main.c
 * Jogo do Mago móvel usando WASD
 */

#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

int mageX = 34, mageY = 12;

void printMage()
{
    screenSetColor(GREEN, BLACK);  // Cor do mago
    screenGotoxy(mageX, mageY);
    printf("🧙");
}

void clearMage()
{
    screenSetColor(BLACK, BLACK);  // Define cor do fundo para preto
    screenGotoxy(mageX, mageY);
    printf("  ");  // dois espaços para limpar o emoji
}

void drawBackground()
{
    screenSetColor(BLACK, BLACK);
    for(int y = MINY; y <= MAXY; y++) {
        for(int x = MINX; x <= MAXX; x++) {
            screenGotoxy(x, y);
            printf(" ");
        }
    }
}

int main() 
{
    int ch = 0;
    
    screenInit(1);
    keyboardInit();
    timerInit(50);

    // Limpa a tela com fundo preto
    drawBackground();
    printMage();
    
    // Instruções
    screenSetColor(YELLOW, BLACK);
    screenGotoxy(2, MAXY + 1);
    printf("Use WASD para mover o mago. ENTER para sair.");
    
    screenUpdate();

    while (ch != 10)  // Enter para sair
    {
        if (keyhit()) 
        {
            ch = readch();
            
            // Move o mago baseado na tecla pressionada
            if (ch == 'w' || ch == 'W') {
                if (mageY > MINY + 1) {
                    clearMage();
                    mageY--;
                    printMage();
                }
            }
            else if (ch == 's' || ch == 'S') {
                if (mageY < MAXY - 1) {
                    clearMage();
                    mageY++;
                    printMage();
                }
            }
            else if (ch == 'a' || ch == 'A') {
                if (mageX > MINX + 1) {
                    clearMage();
                    mageX -= 2;
                    printMage();
                }
            }
            else if (ch == 'd' || ch == 'D') {
                if (mageX < MAXX - 2) {
                    clearMage();
                    mageX += 2;
                    printMage();
                }
            }
            
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
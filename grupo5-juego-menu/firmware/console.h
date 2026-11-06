#ifndef CONSOLA_GRUPO5_H
#define CONSOLA_GRUPO5_H

#include <stdint.h>

#define SCREEN_W 64u
#define SCREEN_H 64u
#define GRID_W 16u
#define GRID_H 14u
#define GRID_SIZE (GRID_W * GRID_H)
#define SNAKE_W 32u
#define SNAKE_H 28u
#define SNAKE_SIZE (SNAKE_W * SNAKE_H)
#define SNAKE_MAX 128u

/* Colores lógicos de 3 bits: bit2=R, bit1=G, bit0=B.
   El grupo 4 debe confirmar si este formato coincide con su framebuffer. */
enum { COL_BLACK=0, COL_BLUE=1, COL_GREEN=2, COL_CYAN=3,
       COL_RED=4, COL_MAGENTA=5, COL_YELLOW=6, COL_WHITE=7 };

/* Valores MCP3008 de 10 bits. El centro es 512; el dead zone evita deriva. */
#define JOYSTICK_CENTER 512u
#define JOYSTICK_DEADZONE 128u
enum { JOY_BUTTON_1=1u<<0, JOY_BUTTON_2=1u<<1,
       JOY_BUTTON_BOTH=JOY_BUTTON_1|JOY_BUTTON_2 };
typedef struct {
    uint16_t x[2]; /* J1_X/CH0, J2_X/CH2 */
    uint16_t y[2]; /* J1_Y/CH1, J2_Y/CH3 */
    uint8_t buttons; /* bit 0: J1, bit 1: J2 */
} JoystickInput;

/* PAC-MAN individual/cooperativo y Snake individual/competitivo. */
typedef enum { MODE_MENU=0, MODE_PACMAN=1, MODE_SNAKE=2, MODE_RESULT=3 } Mode;
typedef enum { RESULT_NONE=0, RESULT_PAC_WIN, RESULT_PAC_LOSE,
               RESULT_P1_WIN, RESULT_P2_WIN, RESULT_DRAW, RESULT_SOLO_LOSE } Result;
typedef enum { DIR_UP=0, DIR_RIGHT=1, DIR_DOWN=2, DIR_LEFT=3 } Direction;
typedef enum { SOUND_NONE=0, SOUND_MENU, SOUND_PICKUP, SOUND_HIT, SOUND_FINISH } Sound;
typedef struct { uint8_t x, y; } Cell;
typedef struct { Cell body[SNAKE_MAX]; uint8_t length, direction, alive; } Snake;

typedef struct {
    Mode mode;
    Mode last_game;
    Result result;
    uint8_t last_buttons, last_directions[2];
    uint8_t selection, paused, players;
    uint16_t records[2]; /* Récords de sesión; sin persistencia EEPROM. */
    Sound sound; /* Evento del último paso; consumir una sola vez. */
    uint8_t walls[GRID_SIZE], pellets[GRID_SIZE];
    uint16_t pellets_left;
    uint8_t lives;
    Cell pac[2], ghosts[2];
    uint8_t pac_direction[2];
    Snake snake[2];
    Cell apple;
    uint16_t score[2];
    uint32_t ticks, random_state;
} Console;

void console_init(Console *c);
/* Una llamada corresponde a un paso de lógica (propuesta: 150 ms).
   Ejes: 0..1023, centro 512; y bajo=arriba y alto=abajo. */
void console_step(Console *c, JoystickInput input);
/* Genera exactamente 4096 bytes de colores 0..7. */
void console_render(const Console *c, uint8_t framebuffer[SCREEN_W*SCREEN_H]);

#endif

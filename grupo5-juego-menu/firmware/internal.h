#ifndef G5_INTERNAL_H
#define G5_INTERNAL_H
#include "console.h"
static inline uint16_t cell_id(unsigned x, unsigned y) { return (uint16_t)(y*GRID_W+x); }
static inline int equal(Cell a, Cell b) { return a.x==b.x && a.y==b.y; }
static inline Cell moved(Cell p, unsigned d) {
    if(d==DIR_UP) --p.y;
    else if(d==DIR_DOWN) ++p.y;
    else if(d==DIR_LEFT) --p.x;
    else if(d==DIR_RIGHT) ++p.x;
    return p;
}
static inline int inside(Cell p) { return p.x<GRID_W && p.y<GRID_H; }
static inline int inside_snake(Cell p) { return p.x<SNAKE_W && p.y<SNAKE_H; }
static inline int passable(const Console *c, Cell p) {
    return inside(p) && !c->walls[cell_id(p.x,p.y)];
}
uint8_t joystick_direction_mask(const JoystickInput *input, unsigned player);
int joystick_direction(const JoystickInput *input, unsigned player, uint8_t *dir);
void set_result(Console *c, Result result);
void start_pacman(Console *c);
void pacman_tick(Console *c, const JoystickInput *input);
void start_snake(Console *c);
void snake_tick(Console *c, const JoystickInput *input);
#endif

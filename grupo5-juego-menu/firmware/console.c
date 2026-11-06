#include "internal.h"

void console_init(Console *c) {
    uint8_t *p=(uint8_t *)c;
    for(unsigned i=0;i<sizeof(*c);++i) p[i]=0;
    c->mode=MODE_MENU; c->last_game=MODE_MENU; c->players=2;
    c->random_state=0xACE1u;
}
void console_step(Console *c,JoystickInput input) {
    c->sound=SOUND_NONE;
    uint8_t buttons=(uint8_t)(input.buttons&JOY_BUTTON_BOTH);
    uint8_t button_edges=(uint8_t)(buttons&(uint8_t)~c->last_buttons);
    uint8_t directions[2]={
        joystick_direction_mask(&input,0),joystick_direction_mask(&input,1)
    };
    uint8_t menu_edges=(uint8_t)(directions[0]&(uint8_t)~c->last_directions[0]);
    int return_to_menu=(buttons==JOY_BUTTON_BOTH &&
                        c->last_buttons!=JOY_BUTTON_BOTH);
    c->last_buttons=buttons;
    c->last_directions[0]=directions[0];
    c->last_directions[1]=directions[1];
    if(c->mode==MODE_MENU) {
        if(menu_edges&(1u<<DIR_LEFT)) c->players=1;
        if(menu_edges&(1u<<DIR_RIGHT)) c->players=2;
        if(menu_edges&(1u<<DIR_UP)) c->selection=0;
        if(menu_edges&(1u<<DIR_DOWN)) c->selection=1;
        if(button_edges&JOY_BUTTON_1) {
            if(c->selection==0) start_pacman(c); else start_snake(c);
        }
        return;
    }
    if(return_to_menu) {
        c->mode=MODE_MENU; c->paused=0; return;
    }
    if(c->mode==MODE_RESULT) {
        if(button_edges&JOY_BUTTON_1) {
            if(c->last_game==MODE_PACMAN) start_pacman(c);
            else start_snake(c);
        }
        return;
    }
    if(button_edges&JOY_BUTTON_2) {
        if(c->mode==MODE_PACMAN) start_pacman(c); else start_snake(c);
        return;
    }
    if(button_edges&JOY_BUTTON_1) {
        c->paused=(uint8_t)!c->paused; c->sound=SOUND_MENU; return;
    }
    if(c->paused) return;
    ++c->ticks;
    if(c->mode==MODE_PACMAN) pacman_tick(c,&input);
    else if(c->mode==MODE_SNAKE) snake_tick(c,&input);
}

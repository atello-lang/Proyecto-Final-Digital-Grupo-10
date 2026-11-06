/* Fixtures para comparar exclusivamente los 4096 píxeles, sin CSS exterior. */
#include "../firmware/console.h"
#include <stdio.h>
static int emit(Console *c) {
    uint8_t f[4096]; console_render(c,f);
    return fwrite(f,1,sizeof(f),stdout)==sizeof(f);
}
static void start_game(Console *c,unsigned game) {
    JoystickInput input={{JOYSTICK_CENTER,JOYSTICK_CENTER},
                         {JOYSTICK_CENTER,JOYSTICK_CENTER},0};
    if(game==2) input.y[0]=1023;
    console_step(c,input);
    input.x[0]=JOYSTICK_CENTER; input.y[0]=JOYSTICK_CENTER;
    console_step(c,input);
    input.buttons=JOY_BUTTON_1;
    console_step(c,input);
}
int main(void) {
    Console c;
    console_init(&c); if(!emit(&c)) return 1;
    for(unsigned players=1;players<=2;++players) {
        console_init(&c); c.players=(uint8_t)players;
        start_game(&c,1);
        for(unsigned dir=0;dir<4;++dir) for(unsigned frame=0;frame<2;++frame) {
            c.pac_direction[0]=(uint8_t)dir;
            c.pac_direction[1]=(uint8_t)((dir+2)%4);
            c.ticks=frame; c.score[0]=67;
            if(!emit(&c)) return 1;
        }
        console_init(&c); c.players=(uint8_t)players;
        start_game(&c,2);
        if(!emit(&c)) return 1;
        {
            JoystickInput neutral={{JOYSTICK_CENTER,JOYSTICK_CENTER},
                                   {JOYSTICK_CENTER,JOYSTICK_CENTER},0};
            console_step(&c,neutral);
        }
        if(!emit(&c)) return 1;
    }
    return 0;
}

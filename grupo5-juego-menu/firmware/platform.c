#include "platform.h"
void console_frame(Console *c, const ConsolePlatform *p, uint8_t pixels[SCREEN_W*SCREEN_H]) {
    JoystickInput input={{JOYSTICK_CENTER,JOYSTICK_CENTER},
                         {JOYSTICK_CENTER,JOYSTICK_CENTER},0};
    if(p->read_joysticks) input=p->read_joysticks(p->context);
    console_step(c,input);
    console_render(c,pixels);
    if(p->present) p->present(p->context,pixels);
    if(p->play_sound && c->sound!=SOUND_NONE) p->play_sound(p->context,c->sound);
    c->sound=SOUND_NONE;
}

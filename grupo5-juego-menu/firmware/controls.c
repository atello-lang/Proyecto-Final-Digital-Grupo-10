#include "internal.h"

static unsigned magnitude(int value) {
    return (unsigned)(value<0?-value:value);
}
int joystick_direction(const JoystickInput *input, unsigned player, uint8_t *dir) {
    int x=(int)input->x[player]-(int)JOYSTICK_CENTER;
    int y=(int)input->y[player]-(int)JOYSTICK_CENTER;
    unsigned ax=magnitude(x), ay=magnitude(y);
    if(ax<=JOYSTICK_DEADZONE && ay<=JOYSTICK_DEADZONE) return 0;
    if(ax>ay) *dir=x<0?DIR_LEFT:DIR_RIGHT;
    else *dir=y<0?DIR_UP:DIR_DOWN;
    return 1;
}
uint8_t joystick_direction_mask(const JoystickInput *input, unsigned player) {
    uint8_t dir;
    return joystick_direction(input,player,&dir)?(uint8_t)(1u<<dir):0;
}

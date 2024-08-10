#include "mrc_base.h"
#include "mrc_exb.h"

int screen_height = 320;
int screen_width = 240;

void draw_point_yingshaoxo(int y, int x, int r, int g, int b) {
    mrc_drawPointEx(x, y, r, g, b);
}

void draw_text_yingshaoxo(char* text_address_or_first_char, int y, int x, int r, int g, int b) {
    mrc_drawText(text_address_or_first_char, x, y, r, g, b, 0, MR_FONT_MEDIUM);
}

void render_screen_yingshaoxo() {
    mrc_refreshScreen(0, 0, screen_width, screen_height);
}

void show_cube() {
    int y = 0;
    while (y < screen_height) {
        int x = 0;
        while (x < screen_width) {
            draw_point_yingshaoxo(y, x, 0, 0, 0);
            x += 1;
        }
        y += 1;
    }
}

void show_a_keyboard() {
    const char* keyboard_string = "1234567890\nqwertyuiop\nasdfghjkl\nzxcvbnmX\n?, .E";
    int char_height = 16;
    int char_width = 8;
    int start_y = screen_height - (screen_height / 4);
    int index = 0;
    int start_x = 0;
    while (1) {
        // garbage mrp compiler, do not support basic c syntax
        // if you use if check in here, error happens, why? because the compiler will insert backdoor code in here
        /*
        if (index > 10) {
            break;
        }
        */

        char temp_string[2] = "x\0";
        temp_string[0] = keyboard_string[index];
        draw_text_yingshaoxo(temp_string, start_y, start_x, 255, 255, 255);

        start_x += char_width;
        index += 1;

        if ((uint8)(keyboard_string[index]) == (uint8)('\0')) {
            break;
        }

        if (keyboard_string[index] == '\n') {
            start_y += char_height;
        }
    }
}


void initGame(){
    // reset random seed
    //mrc_sand(mrc_getUptime());
    //mrc_sand(mrc_rand()%mrc_getUptime());

    char* a_text = "Hi, you";

    show_cube();

    draw_text_yingshaoxo(a_text, screen_height/2, screen_width/2, 255, 255, 255);

    show_a_keyboard();

    render_screen_yingshaoxo();
}

//Entry Function
int32 MRC_EXT_INIT(void)
{
    initGame();
    return MR_SUCCESS;
}

int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
    mrc_printf("mrc_appEvent(%d, %d, %d)\r\n", code, param0, param1);
    if(code == MR_KEY_PRESS){
        if (param0 == MR_KEY_SOFTRIGHT) {
            // esc key in desktop, don't know what it really links
            mrc_exit();
        }
    }
    return MR_SUCCESS;
}

int32 mrc_appPause()
{
    mrc_printf("mrc_appPause();\r\n");
    return MR_SUCCESS;	
}

int32 mrc_appResume()
{
    mrc_printf("mrc_appResume();\r\n");
    return MR_SUCCESS;
}

int32 MRC_EXT_EXIT(void)
{
    mrc_printf("MRC_EXT_EXIT();\r\n");
    return MR_SUCCESS;
}
#include "lcd.h"
#include "bmp.h"
#include "touch.h"
#include "filelist.h"
#include "game.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
int main()
{
    //初始化屏幕，触摸外屏，蜂鸣器。
    init_lcd();
    touch_init();
    //updata_rank(83);
    // int rank[10] = {0};
    // read_record(rank);
    //进入循环，通过页面接口进入游戏
    while(1)
    {
        page_change();
        game_defeat();
    }
    deinit_lcd();
    touch_deinit();
    return 0;
}

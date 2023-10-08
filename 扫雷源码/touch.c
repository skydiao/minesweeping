#include "touch.h"
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
int touch_fd;

struct input_event touch;

void touch_init()
{
    touch_fd = open("/dev/input/event0",O_RDONLY);
    if(touch_fd == -1)
    {
        perror("open touch error");
        return;
    }
}

void touch_deinit()
{
    close(touch_fd);
}

/*  
    struct input_event {
        struct timeval time;
        __u16 type;
        __u16 code;
        __s32 value;
    };
    算法思路，判断触摸事件，将坐标转化出来通过x，y保存
    再判断 按键事件，按下和松开时分开保存一次坐标，松开后返回。
*/

void get_touch_data(Touch *touch_data)
{
    int x = -1,y = -1;
    while(1)
    {
        //读取事件
        read(touch_fd,&touch,sizeof(struct input_event));
        if(touch.type == EV_ABS)//触摸事件，转换坐标
        {
            if(touch.code == ABS_X)
            {
                x = touch.value*799.0/1023.0;
            }
            if(touch.code == ABS_Y)
            {
                y = touch.value*479.0/599.0;
            }
        }
        if(touch.type == EV_KEY&&touch.code == BTN_TOUCH)
        {//按键事件，保存按下，松开时的坐标
            if(touch.value == 1)
            {
                touch_data->x0 = x;
                touch_data->y0 = y;
            }
            else
            {
                touch_data->x1 = x;
                touch_data->y1 = y;
                return; 
            }
        }
    }
}
/*
    判断在屏幕上滑动的方向
    算法是，计算k值，也就是判断x的变化和y的变化谁比较明显，再通过x y的差值判断方向
*/
int get_touch_move(Touch* touch_data)
{
    get_touch_data(touch_data);
    //计算k值，也就是x，y的变化率
    int k = abs((touch_data->x1-touch_data->x0)*1.0/(touch_data->y1-touch_data->y0)*1.0);
    int x = touch_data->x1-touch_data->x0;
    int y = touch_data->y1-touch_data->y0;
    //以下分别表示上下左右
    if(k>1 && x>0)
    {
        return 4;
    }
    else if(k>1 && x<0)
    {
        return 3;
    }
    else if(k<1 && y>0)
    {
        return 1;
    }
    else if(k<1 && y<0)
    {
        return 2;
    }
}

//将坐标转换为数组下标
Array *Coord_conv_index(Touch *touch_data)
{
    Array *index = (Array *)malloc(sizeof(Array));
    index->x = touch_data->x1/50;
    index->y = (touch_data->y1 - 15)/50;
    return index;
}
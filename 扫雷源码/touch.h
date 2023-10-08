#ifndef __TOUCH_H__
#define __TOUCH_H__
//保存接触屏幕时的坐标和松开屏幕时的坐标
typedef struct touch
{
    int x0;
    int y0;
    int x1;
    int y1;
}Touch;

typedef struct array
{
    int x;
    int y;
}Array;
/*
    获取按下屏幕时的坐标和松开屏幕时的坐标
*/
void get_touch_data(Touch *touch_data);
/*
    初始化触摸屏
*/
void touch_init();
/*
    关闭触摸屏
*/
void touch_deinit();
/*
    判断触摸方向
    @touch_data 保存坐标数据的结构体
*/
int get_touch_move(Touch* touch_data);
/*
    将获取到的坐标转换为数组下标
    @touch_data 保存坐标数据的结构体
*/
Array *Coord_conv_index(Touch *touch_data);
#endif
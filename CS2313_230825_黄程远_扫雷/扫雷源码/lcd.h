#ifndef __LCD_H__
#define __LCD_H__
/*
    初始化显示屏幕
*/
int init_lcd();
/*
    关闭显示屏幕
*/
void deinit_lcd();
/*
    画点
    @x     x坐标
    @y     y坐标
    @color 颜色
*/
void lcd_draw_point(int x,int y,int color);
/*
    显示bmp图片
    @x            x坐标
    @y            y坐标
    @bmp_path     图片路径
*/
void show_bmp(int x,int y,char *bmp_path);

#endif
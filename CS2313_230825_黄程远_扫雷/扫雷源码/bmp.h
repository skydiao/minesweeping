#ifndef __BMP_H__
#define __BMP_H__
//图库结构体
typedef struct pic
{
    char *pic_path;
    struct pic *next;
    struct pic *pre;
}Pic;
//头结点
typedef struct head
{
    Pic *first;
    Pic *last;
}Head;
/*
    创建图库
*/
Head *creat_pic(void);
/*
    插入图片
    @p 头指针
    @pic_path 图片的路径
*/
void insert_pic(Head *p,char *pic_path);
/*
    从文件路径中获取图片
    @p 头指针
*/
void get_bmp_pic(Head *p);
/*
    通过点击切换图片
*/
void touch_control_picture();
#endif
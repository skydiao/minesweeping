#include "bmp.h"
#include "touch.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
//保存上下左右
#define up 1
#define down 2
#define left 3
#define right 4
char *pic_path = "/home/project/pic";
char *return_page = "/home/project/12.bmp";
//建立链表
Head *creat_pic()
{
    Head *p = (Head *)malloc(sizeof(Head));
    p->first = NULL;
    p->last = NULL;
    return p;
}
//链表插入
void insert_pic(Head *p,char *pic_path)
{
    if(!pic_path)
    {
        return;
    }
    Pic *insert_pic = (Pic *)malloc(sizeof(Pic));
    insert_pic->pic_path = (char *)malloc(sizeof(char)*(strlen(pic_path)+1));
    strncpy(insert_pic->pic_path,pic_path,strlen(pic_path));
    if(!p->first)
    {
        p->first = insert_pic;
        p->last = insert_pic;
        insert_pic->next = insert_pic;
        insert_pic->pre = insert_pic;
        return;
    }
    else
    {
        insert_pic->next = p->first;
        insert_pic->pre = p->last;
        p->last->next = insert_pic;
        p->first->pre = insert_pic;
        p->last = insert_pic;
        return;
    }
}
//直接从文件夹中获取图片
void get_bmp_pic(Head *p)
{
    DIR *pic_dir = opendir(pic_path); //打开一个目录
    if(!pic_dir)
    {
        perror("open pic_dir error");
    }
    char filename[512];
    struct dirent* dirp = NULL;
    while(dirp = readdir(pic_dir))
    {
        if(!strcmp(dirp->d_name,".") || !strcmp(dirp->d_name,".."))
        {
            continue;
        }
        sprintf(filename,"%s/%s",pic_path,dirp->d_name);
        insert_pic(p,filename);
    }
    return;
}
//通过滑动方向切换图片
void touch_control_picture()
{
    Touch touch_data;
    Head *pic = creat_pic();
    get_bmp_pic(pic);
    Pic *p = pic->first;
    show_bmp(0,0,p->pic_path);
    show_bmp(0,430,return_page);
    while(1)
    {
        int position = get_touch_move(&touch_data);
        if(touch_data.x1 > 0 && touch_data.x1 < 133 && touch_data.y1 > 387)
        {
            return;
        }
        if(position == right)
        {
            p = p->next;
            show_bmp(0,0,p->pic_path);
            show_bmp(0,430,return_page);
            printf("%d %d\n",touch_data.x1,touch_data.y1);
        }
        else if(position == left)
        {
            p = p->pre;
            show_bmp(0,0,p->pic_path);
            show_bmp(0,430,return_page);
            printf("%d %d\n",touch_data.x1,touch_data.y1);
        }
    }
}
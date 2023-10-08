#ifndef __GAME_H__
#define __GAME_H__
#include "touch.h"
/*
    初始化游戏，根据随机函数创建雷
    数字和空白处。
*/
void creat_game();
/*
    通过雷的位置初始化雷周围的数字
    @rows 行
    @cols 列
*/
void get_num_ByMines(int rows,int cols);
/*
    新一局游戏开始前，清除所有的游戏数据
*/
void clean();
/*
    游戏主进程
*/
void game_run();
/*
    点击相应位置时的显示效果
    @index 你点击的位置转换成的相应数组下标
*/
int game_show(Array *index);
/*
    游戏失败的显示界面
*/
void game_defeat();
/*
    但你点击到空白处时，显示空白周围一切非雷的块
    @x 数组下标
    @y 数组下标
*/
void game_show_blank(int x,int y);
/*
    游戏胜利时的界面
*/
void game_victoty();
/*
    当游戏为插旗模式时，你点击屏幕的效果
    @index 你点击的位置转换成的相应数组下标
*/
void game_show_flag(Array *index);
/*
    获取排行榜信息
*/
void read_record(int *rank);
/*
    排行榜界面
*/
void show_rank();
/*
    更新排行榜信息
*/
int updata_rank(unsigned int time);
#endif 
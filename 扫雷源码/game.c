#include "game.h"
#include "lcd.h"
#include "bmp.h"
#include "filelist.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//插旗模式 对应的值
#define Mode_flag 1
//游戏模式 对应的值
#define Mode_game 2
//总格子数
#define Game_difficulty 81
//判断游戏结束的标志
unsigned int flag = -1;
//游戏时间
unsigned int time_ = 0;
//格子总数，当它为10时判断游戏胜利
unsigned char game_win = 81;
//扫雷雷盘大小 0 为空白 1-8 为周围雷数 9 代表这个区域已经点击过 10 表示为雷 11 表示已经插旗
unsigned int game_board[9][9] = {0};
//判断是否被插旗
unsigned char if_flag[9][9] = {0};
//设置雷的个数
unsigned int Mines = 10;
//设置现在的游戏模式
unsigned char game_Mode = 2; 
//图片的路径
unsigned char* pic_start = "./pic/start.bmp";//游戏界面
unsigned char* pic_num_time[10] = {
    "./pic/num0.bmp",
    "./pic/num1.bmp",
    "./pic/num2.bmp",
    "./pic/num3.bmp",
    "./pic/num4.bmp",
    "./pic/num5.bmp",
    "./pic/num6.bmp",
    "./pic/num7.bmp",
    "./pic/num8.bmp",
    "./pic/num9.bmp"
};//显示时间的数字
unsigned char* pic_num[8] = {
    "./pic/1.bmp",
    "./pic/2.bmp",
    "./pic/3.bmp",
    "./pic/4.bmp",
    "./pic/5.bmp",
    "./pic/6.bmp",
    "./pic/7.bmp",
    "./pic/8.bmp"
};//扫雷的数字
unsigned char* pic_nothing = "./pic/nothing.bmp";//空白
unsigned char* pic_defeat = "./pic/defeat.bmp";//失败背景
unsigned char* pic_bg = "./pic/bg.bmp";//游戏背景
unsigned char* pic_win = "./pic/win.bmp";//胜利背景
unsigned char* pic_flag = "./pic/flag.bmp";//旗子
unsigned char* pic_choose = "./pic/choose.bmp";//单个格子
unsigned char* pic_Mode_Mines = "./pic/Mode_Mines.bmp";//雷
unsigned char* pic_Mode_flag = "./pic/Mode_flag.bmp";//模式 插旗
unsigned char* txt_record = "./record.txt";//排行榜保存文件
unsigned char* pic_rank = "./pic/rank.bmp";//排行榜背景图
unsigned char* pic_new_record = "./pic/new_record.bmp";//提醒成绩更新
/*
    线程函数，完成计时功能
*/
void *func(void *t)
{
    int num_1 = 0;
    int num_2 = 0;
    int num_0 = 0;
    while(1)
    {
        if(flag == 0)
        {
            break;
        }
        num_0 = time_/100;
        num_1 = time_%100/10;
        num_2 = time_%10;
        show_bmp(500,92,pic_num_time[num_0]);
        show_bmp(526,92,pic_num_time[num_1]);
        show_bmp(552,92,pic_num_time[num_2]);
        usleep(1000*980);
        time_++;
        if(flag == 0)
        {
            break;
        }
    }
}


//开启新一局游戏，把所有数据清空
void clean()
{
    //将雷数，胜利条件，模式全部还原,雷分布
    flag = -1;
    // time_ = 0;
    Mines = 10;
    game_win = 81;
    game_Mode = 2;
    for(int i = 0;i<9;i++)
    {
        for(int j = 0;j<9;j++)
        {
            game_board[i][j] = 0;
            if_flag[i][j] = 0;
        }
    }
}

void creat_game()
{
    //先清除棋盘,重置雷数
    clean();
    //设置随机数种子
    srand((unsigned int)time(NULL));
    //循环生成10个雷
    while(Mines--)
    {
        while(1)
        {
            //获取0到80的随机数
            int ret = rand();
            ret = ret % Game_difficulty;
            //在随机的位置生成雷
            if(game_board[ret/9][ret%9] == 0)
            {
                game_board[ret/9][ret%9] = 10;
                //根据周围的雷数来确定数字
                get_num_ByMines(ret/9,ret%9);
                printf("%d\n",ret);
                break;
            }
        }
    }
}

//根据周围的雷数来确定数字
void get_num_ByMines(int rows,int cols)
{
    //遍历雷一周的所有格子，非雷的格子数字加一
    if(rows-1 >= 0 && cols-1 >= 0)
    {
        if(game_board[rows-1][cols-1] != 10)
        {
            game_board[rows-1][cols-1]++;
        }
    }
    if(rows-1 >= 0)
    {
        if(game_board[rows-1][cols] != 10)
        {
            game_board[rows-1][cols]++;
        }
    }
    if(cols-1 >= 0)
    {
        if(game_board[rows][cols-1] != 10)
        {
            game_board[rows][cols-1]++;
        }
    }
    if(rows-1 >= 0 && cols+1 <= 8)
    {
        if(game_board[rows-1][cols+1] != 10)
        {
            game_board[rows-1][cols+1]++;
        }
    }
    if(rows+1 <= 8 && cols-1 >= 0)
    {
        if(game_board[rows+1][cols-1] != 10)
        {
            game_board[rows+1][cols-1]++;
        }
    }
    if(rows+1 <= 8 && cols+1 <= 8)
    {
        if(game_board[rows+1][cols+1] != 10)
        {
            game_board[rows+1][cols+1]++;
        }
    }
    if(rows+1 <= 8)
    {
        if(game_board[rows+1][cols] != 10)
        {
            game_board[rows+1][cols]++;
        }
    }
    if(cols+1 <= 8)
    {
        if(game_board[rows][cols+1] != 10)
        {
            game_board[rows][cols+1]++;
        }
    }
}
//游戏运行主进程
void game_run()
{
    creat_game();
    //输出雷 空白 数字的排布
    for(int i = 0;i<9;i++)
    {
        for(int j = 0;j<9;j++)
        {
            printf("%2d ",game_board[i][j]);
        }
        printf("\n");
    }
    show_bmp(0,0,pic_bg);
    show_bmp(0,15,pic_start);
    show_bmp(450,15,pic_Mode_Mines);
    Touch touch_data;//初始化坐标结构体
    Array *index = NULL;
    flag = -1;
    //开线程
    pthread_t th = -1;
    pthread_create(&th,NULL,func,NULL);
    pthread_detach(th);
    while(1)
    {
        //达到游戏胜利条件，执行胜利函数
        if(game_win == 10)
        {
            flag = 0;
            usleep(500*1000);
            free(index);
            game_victoty();
            return;
        }
        get_touch_data(&touch_data);
        //模式切换
        if(touch_data.x1 > 450 && touch_data.x1 < 525 && touch_data.y1 > 15 && touch_data.y1 < 90)//点击这里切换游戏模式
        {
            //从扫雷模式切换为插旗模式
            if(game_Mode == Mode_game)
            {
                game_Mode = Mode_flag;
                show_bmp(450,15,pic_Mode_flag);
            }
            else if(game_Mode == Mode_flag)//从插旗模式到扫雷模式
            {
                game_Mode = Mode_game;
                show_bmp(450,15,pic_Mode_Mines);
            }
            //game_Mode = game_Mode==Mode_game?Mode_flag:Mode_game;
        }
        if(touch_data.x1 > 0 && touch_data.x1 < 450 && touch_data.y1 > 15 && touch_data.y1 < 465)
        {
            //先转换坐标为数组下标
            index = Coord_conv_index(&touch_data);
            //判断游戏模式以此来调用不同的显示函数
            if(game_Mode == Mode_game)
            {
                //这里是正常游戏模式
                flag = game_show(index);
                if(flag == 0)
                {
                    usleep(500*1000);
                    free(index);
                    game_defeat();
                    return;
                }
            }
            else if(game_Mode == Mode_flag)
            {
                game_show_flag(index);//这里进入插旗模式
            }
        }
    }
}

//游戏的显示效果
int game_show(Array *index)
{
    printf("%d %d \n",index->y,index->x);
    //点击到雷并且未插旗判断为失败
    if(game_board[index->y][index->x] == 10 && if_flag[index->y][index->x] != 1)
    {
        return 0;
    }//点击到空白处而且未插旗
    else if(game_board[index->y][index->x] == 0 && if_flag[index->y][index->x] != 1)//未插旗才允许点击
    {
        show_bmp(index->x*50,index->y*50+15,pic_nothing);
        game_board[index->y][index->x] = 9;
        game_win--;
        game_show_blank(index->y,index->x);
        return 1;
    }//点击到数字处而且未插旗
    else if(game_board[index->y][index->x] <= 8 && if_flag[index->y][index->x] != 1)//未插旗才允许点击
    {
        show_bmp(index->x*50,index->y*50+15,pic_num[game_board[index->y][index->x]-1]);
        game_board[index->y][index->x] = 9;
        game_win--;
        return 1;
    }
}
//插旗模式下的点击效果
void game_show_flag(Array *index)
{
    //未被点击并且未被插旗
    if(game_board[index->y][index->x] != 9 && if_flag[index->y][index->x] == 0)
    {
        //插旗
        show_bmp(index->x*50,index->y*50+15,pic_flag);
        if_flag[index->y][index->x] = 1;
        return;
    }
    //未被点击并且已经插旗
    if(game_board[index->y][index->x] != 9 && if_flag[index->y][index->x] == 1)
    {
        //消旗
        show_bmp(index->x*50,index->y*50+15,pic_choose);
        if_flag[index->y][index->x] = 0;
        return;
    }
}
//点击到无雷块，要显示周围一片区域
void game_show_blank(int x,int y)
{
    //9表示这个位置已经被扫过了
    //遍历周围每一个格子，是数字就显示，是雷就不动，是旗子也不动，是空白则调用本身
    if(x-1 >= 0 && y-1 >= 0 && game_board[x-1][y-1] < 9 && if_flag[x-1][y-1] != 1)//左上
    {
        if(game_board[x-1][y-1] == 0)
        {
            show_bmp((y-1)*50,(x-1)*50+15,pic_nothing);
            game_board[x-1][y-1] = 9;
            game_win--;
            game_show_blank(x-1,y-1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y-1)*50,(x-1)*50+15,pic_num[game_board[x-1][y-1]-1]);
            game_board[x-1][y-1] = 9;
            game_win--;
        }
    }

    if(x-1 >= 0 && game_board[x-1][y] < 9 && if_flag[x-1][y] != 1)//左
    {
        if(game_board[x-1][y] == 0)
        {
            show_bmp((y)*50,(x-1)*50+15,pic_nothing);
            game_board[x-1][y] = 9;
            game_win--;
            game_show_blank(x-1,y);//有空白递归调用本身
        }
        else
        {
            show_bmp((y)*50,(x-1)*50+15,pic_num[game_board[x-1][y]-1]);
            game_board[x-1][y] = 9;
            game_win--;
        }
    }

    if(x-1 >= 0 && y+1 <= 8 && game_board[x-1][y+1] < 9 && if_flag[x-1][y+1] != 1)//左下
    {
        if(game_board[x-1][y+1] == 0)
        {
            show_bmp((y+1)*50,(x-1)*50+15,pic_nothing);
            game_board[x-1][y+1] = 9;
            game_win--;
            game_show_blank(x-1,y+1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y+1)*50,(x-1)*50+15,pic_num[game_board[x-1][y+1]-1]);
            game_board[x-1][y+1] = 9;
            game_win--;
        }
    }

    if(x+1 <= 8 && y-1 >= 0 && game_board[x+1][y-1] < 9 && if_flag[x+1][y-1] != 1)//右上
    {
        if(game_board[x+1][y-1] == 0)
        {
            show_bmp((y-1)*50,(x+1)*50+15,pic_nothing);
            game_board[x+1][y-1] = 9;
            game_win--;
            game_show_blank(x+1,y-1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y-1)*50,(x+1)*50+15,pic_num[game_board[x+1][y-1]-1]);
            game_board[x+1][y-1] = 9;
            game_win--;
        }
    }

    if(x+1 <= 8 && game_board[x+1][y] < 9 && if_flag[x+1][y] != 1)//右
    {
        if(game_board[x+1][y] == 0)
        {
            show_bmp((y)*50,(x+1)*50+15,pic_nothing);
            game_board[x+1][y] = 9;
            game_win--;
            game_show_blank(x+1,y);//有空白递归调用本身
        }
        else
        {
            show_bmp((y)*50,(x+1)*50+15,pic_num[game_board[x+1][y]-1]);
            game_board[x+1][y] = 9;
            game_win--;
        }
    }

    if(x+1 <= 8 && y+1 <= 8 && game_board[x+1][y+1] < 9 && if_flag[x+1][y+1] != 1)//右上
    {
        if(game_board[x+1][y+1] == 0)
        {
            show_bmp((y+1)*50,(x+1)*50+15,pic_nothing);
            game_board[x+1][y+1] = 9;
            game_win--;
            game_show_blank(x+1,y+1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y+1)*50,(x+1)*50+15,pic_num[game_board[x+1][y+1]-1]);
            game_board[x+1][y+1] = 9;
            game_win--;
        }
    }

    if(y-1 >= 0 && game_board[x][y-1] < 9 && if_flag[x][y-1] != 1)//上
    {
        if(game_board[x][y-1] == 0)
        {
            show_bmp((y-1)*50,(x)*50+15,pic_nothing);
            game_board[x][y-1] = 9;
            game_win--;
            game_show_blank(x,y-1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y-1)*50,(x)*50+15,pic_num[game_board[x][y-1]-1]);
            game_board[x][y-1] = 9;
            game_win--;
        }
    }

    if(y+1 <= 8 && game_board[x][y+1] < 9 && if_flag[x][y+1] != 1)//下
    {
        if(game_board[x][y+1] == 0)
        {
            show_bmp((y+1)*50,(x)*50+15,pic_nothing);
            game_board[x][y+1] = 9;
            game_win--;
            game_show_blank(x,y+1);//有空白递归调用本身
        }
        else
        {
            show_bmp((y+1)*50,(x)*50+15,pic_num[game_board[x][y+1]-1]);
            game_board[x][y+1] = 9;
            game_win--;
        }
    }
}

//游戏失败
void game_defeat()
{
    time_ = 0;
    Touch touch_data;
    loop:
    show_bmp(0,0,pic_defeat);
    int x,y;
    while(1)
    {
        get_touch_data(&touch_data);
        x = touch_data.x1;
        y = touch_data.y1;
        //退出
        if(x > 199 && x < 325 && y > 249 && y < 310)
        {
            page_change();
        }
        else if(x > 410 && x < 535 && y > 249 && y < 310)//再来一局
        {
            game_run();
            goto loop;
        }
    }
}
//游戏胜利
void game_victoty()
{
    Touch touch_data;
    int num_0 = time_/100;
    int num_1 = time_/10;
    int num_2 = time_%10;
    show_bmp(0,0,pic_win);
    show_bmp(436,377,pic_num_time[num_0]);
    show_bmp(459,377,pic_num_time[num_1]);
    show_bmp(482,377,pic_num_time[num_2]);
    int x,y;
    int if_updata = updata_rank(time_);
    if(if_updata)
    {
        show_bmp(234,439,pic_new_record);
    }
    time_ = 0;
    while(1)
    {
        get_touch_data(&touch_data);
        int x = touch_data.x1;
        int y = touch_data.y1;
        //退出
        if(x > 189 && x < 311 && y > 279 && y < 342)
        {
            page_change();
        }
        else if(x > 458 && x < 587 && y > 276 && y < 345)//再来一局
        {
            game_run();
        }
    }
}

//读取排行榜txt文件,返回前三名信息
void read_record(int *rank)
{
    FILE *fl = NULL;
    fl = fopen(txt_record, "r+");
    char record[10] = {0};
    int count = 0;
    while(!feof(fl))
    {
        fgets(record, 10, fl);
        rank[count] = atoi(record);
        //printf("%c %c %c %d\n",record[0],record[1],record[2],rank[count]);
        count++;
    }
    return;
}

//显示排行榜信息
void show_rank()
{
    show_bmp(0,0,pic_rank);
    int rank[10] = {0};
    read_record(rank);
    //第一名
    show_bmp(270,61,pic_num_time[1]);
    show_bmp(432,61,pic_num_time[rank[0]/100]);
    show_bmp(458,61,pic_num_time[rank[0]%100/10]);
    show_bmp(484,61,pic_num_time[rank[0]%10]);
    //第二名
    show_bmp(270,159,pic_num_time[2]);
    show_bmp(432,159,pic_num_time[rank[1]/100]);
    show_bmp(458,159,pic_num_time[rank[1]%100/10]);
    show_bmp(484,159,pic_num_time[rank[1]%10]);
    //第三名
    show_bmp(270,257,pic_num_time[3]);
    show_bmp(432,257,pic_num_time[rank[2]/100]);
    show_bmp(458,257,pic_num_time[rank[2]%100/10]);
    show_bmp(484,257,pic_num_time[rank[2]%10]);

    Touch touch_data;//初始化坐标结构体
    while(1)
    {
        get_touch_data(&touch_data);
        if(touch_data.x0 > 0 && touch_data.y0 >0)
        {
            break;
        }
    }

}

//更新排行榜信息
int updata_rank(unsigned int time)
{
    FILE *fl = NULL;
    fl = fopen(txt_record, "r+");
    char record[10] = {0};
    int rec[3] = {0};
    int count = 0;
    int flag = 0;
    int mid = 999;
    while(!feof(fl))
    {
        fgets(record, 10, fl);
        rec[count] = atoi(record);
        if(mid < rec[count])
        {
            int t;
            t = rec[count];
            rec[count] = mid;
            mid = t;
        }
        if(rec[count] > time && flag == 0)
        {
            flag = 1;
            mid = rec[count];
            rec[count] = time;
        }
        count++;
    }
    //fputs(const char *s, FILE *stream);
    fclose(fl);
    char new_record[5];
    fl = fopen(txt_record, "w+");
    count = 0;
    while(count <= 2)
    {
        new_record[0] = 48 + rec[count]/100;
        new_record[1] = 48 + rec[count]%100/10;
        new_record[2] = 48 + rec[count]%10;
        new_record[3] = '\n';
        new_record[4] = '\0';
        printf("%s\n",new_record);
        fputs(new_record, fl);
        count++;
    }
    fclose(fl);
    return flag;
}
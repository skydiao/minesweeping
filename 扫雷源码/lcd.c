#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <math.h>
#include <stdlib.h>
#include <sys/mman.h>
int fd;
unsigned int *pmap;
//初始化lcd
void init_lcd()
{
    fd = open("/dev/fb0",O_RDWR);
    if(fd < 0)
    {
        perror("open lcd error!");
    }
    pmap = mmap(NULL,800*480*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(pmap == MAP_FAILED)
    {
        perror("mmap error");
    }
}
//关闭lcd
void deinit_lcd()
{
    munmap(pmap,800*480*4);
    close(fd);
}
//画点函数
void lcd_draw_point(int x,int y,int color)
{
    if(x>=0 && x < 800 && y>=0 && y<480)
    {
        *(pmap + y*800 + x) = color;
    }
}
/*
    算法思路，通过bmp图信息来写，首先获取文件的类型判断是否为bmp类型
    然后获取图的宽度，高度，色深，以此来求得像素点数目，以此来读取每个像素点的颜色信息
    计算每一行的像素点数目时要判断是否能被4除尽，要补，差多少是4的倍数补多少个。
    然后一个个画点，处理argb时要注意这是小端模式，得移位处理。
*/
void show_bmp(int x,int y,char *bmp_path)
{
    //打开bmp文件
    int bmp_fd = open(bmp_path,O_RDONLY);
    if(bmp_fd < 0)
    {
        perror("open file error");
        return;
    }
    unsigned char pic_buf[4];
    int offset;//保存光标位置
    int bmp_width,bmp_depth;//保存图片的宽和长
    int bmp_depth_color;//保存图片的色深
    //读取打开的文件类型，不是bmp文件则退出
    read(bmp_fd,pic_buf,2);
    if(pic_buf[0] != 0x42 || pic_buf[1] != 0x4d)
    {
        printf("这不是一个bmp文件!\n");
        return;
    }
    //读取光标位置
    lseek(bmp_fd,0x0A,SEEK_SET);
    read(bmp_fd,&offset,4);
    //读取图片宽度
    //以下有 | 符号的都是对小端模式的处理，低地址存低字节。
    lseek(bmp_fd,0x12,SEEK_SET);
    read(bmp_fd,pic_buf,4);
    bmp_width = pic_buf[3]<<24 | pic_buf[2]<<16 | pic_buf[1]<<8 | pic_buf[0];
    //读取图片长度
    read(bmp_fd,pic_buf,4);
    bmp_depth = pic_buf[3]<<24 | pic_buf[2]<<16 | pic_buf[1]<<8 | pic_buf[0];
    //读取图片色深
    lseek(bmp_fd,0x1c,SEEK_SET);
    read(bmp_fd,pic_buf,2);
    bmp_depth_color = pic_buf[1]<<8 | pic_buf[0];
    //算总像素点数目
    //先算每行的像素点数目
    int fillbytes = 0;
    int line_bytes = bmp_width*bmp_depth_color/8;
    if(line_bytes%4)
    {
        fillbytes = 4-line_bytes%4;
        line_bytes+=(4-line_bytes%4);
    }
    int all_bytes = line_bytes*abs(bmp_depth);
    //读取每一个点的颜色
    unsigned char colorbuf[all_bytes];
    lseek(bmp_fd,0x36,SEEK_SET);
    read(bmp_fd,colorbuf,all_bytes);
    //设置argb，根据小端模式来一个个画点
    //注意实际上在地址中保存的是b g r a，所以要移位处理
    char a = 0,r,g,b;
    int count = 0,color;
    printf("pic_w=%d,pic_h=%d,pic_depth=%d\n",bmp_width,bmp_depth,bmp_depth_color);
    for(int i = 0;i<abs(bmp_depth);i++)
    {
        for(int j = 0;j<bmp_width;j++)
        {
            b = colorbuf[count++];
            g = colorbuf[count++];
            r = colorbuf[count++];
            if(bmp_depth_color == 32)
            {
                a = colorbuf[count++];
            }
            else
            {
                a = 0;
            }
            color = a <<24 | r << 16| g << 8 | b;
            if (bmp_depth > 0)
            {
                //bmp_depth > 0 时，从下往上画
                lcd_draw_point(j + x,bmp_depth-1-i+y,color);
            }
            else
            {
                lcd_draw_point(j + x,i + y,color);
            }
        }
        count+=fillbytes;
    }
    close(bmp_fd);
}
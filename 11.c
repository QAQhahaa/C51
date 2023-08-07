#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char

uchar code table[] = {
    0x3f, 0x06, 0x5b, 0x4f,
    0x66, 0x6d, 0x7d, 0x07,
    0x7f, 0x6f, 0x77, 0x7c,
    0x39, 0x5e, 0x79, 0x71
};

uchar minute = 0; // 分钟
uchar second = 0; // 秒钟

uchar timer_count = 0; // 定时器中断计数

sbit dula = P2^6;
sbit wela = P2^7;

sbit key1 = P3^4; // 暂停按键
sbit key2 = P3^5; // 继续按键
sbit key3 = P3^6; // 清零按键

bit paused = 0; // 暂停标志

void delay_ms(uint ms)
{
    uint i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

void display_time(uchar min, uchar sec)
{
    wela = 1;
    P0 = 0xfe; // 选择第一个数码管
    wela = 0;

    dula = 1;
    P0 = table[min / 10]; // 显示十位分钟
    dula = 0;
    delay_ms(1);

    wela = 1;
    P0 = 0xfd; // 选择第二个数码管
    wela = 0;

    dula = 1;
    P0 = table[min % 10]; // 显示个位分钟
    dula = 0;
    delay_ms(1);

    wela = 1;
    P0 = 0xfb; // 选择第三个数码管
    wela = 0;

    dula = 1;
    P0 = table[sec / 10]; // 显示十位秒钟
    dula = 0;
    delay_ms(1);

    wela = 1;
    P0 = 0xf7; // 选择第四个数码管
    wela = 0;

    dula = 1;
    P0 = table[sec % 10]; // 显示个位秒钟
    dula = 0;
    delay_ms(1);
}

void init_timer1()
{
    TMOD = 0x10;
    TH1 = 0x3C;
    TL1 = 0xB0;
    EA = 1;
    ET1 = 1;
    TR1 = 1;
}

void main()
{
    init_timer1();

    while(1)
    {
        if (!key1) // 暂停按键
        {
            paused = 1;
            while (!key1); // 等待按键释放
        }

        if (!key2) // 继续按键
        {
            paused = 0;
            while (!key2); // 等待按键释放
        }

        if (!key3) // 清零按键
        {
            paused = 0;
            minute = 0;
            second = 0;
            timer_count = 0;
            while (!key3); // 等待按键释放
        }

        if (!paused)
        {
            display_time(minute, second);
        }
        else
        {
            // 在暂停状态下，四位数码管都显示当前的时间
            display_time(minute, second);
        }
    }
}

void timer1_isr() interrupt 3
{
    TH1 = 0x3C;
    TL1 = 0xB0;

    if (!paused)
    {
        timer_count++;

        if (timer_count >= 20)
        {
            timer_count = 0;
            second++;

            if (second >= 60)
            {
                second = 0;
                minute++;

                if (minute >= 60)
                {
                    minute = 0;
                }
            }
        }
    }
}

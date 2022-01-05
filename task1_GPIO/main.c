#include "msp.h"
#include "Clock.h"
#include <stdio.h>


#define RED       0x01  //R
#define GREEN     0x02  //G
#define BLUE      0x04  //B
#define yellow    0x03  //RG-
#define sky blue  0x06  //-GB
#define white     0x07  //RGB
#define pink      0x05  //R-B

void Port23_Init(void){
  P2->SEL0 = 0x00;
  P2->SEL1 = 0x00; //configure P2.3-P2.0 as GPIO
  P2->DS = 0x0F;   // make P2.2-P2.0 high drive strength
  P2->DIR = 0x0F;  // make P2.2-P2.0 out
  P2->OUT = 0x00;  // all LEDs off
}
void main()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer停止看门狗定时器
    Clock_Init48MHz();
    Port23_Init();
    P1DIR &= 0xCC;//设置P1.0输出灯，设置P1.0,P1.1，P1.4，P1.5输入按钮1110  1100
    P1DIR |= 0x01;//设置输出灯，去掉P1.0，设置P1.1，P1.4，P1.5输入按钮0000 0001
    P1REN = 0x32;//使能，设置P1.1，P1.4,P1.5初始为高电位0001  0010
    P1OUT = 0x33;//按下时，设置P1.0，P1.1，P1.4,P1.5初始输出高电位0001  0011，为亮灯状态
    P2OUT |= 0x08;//s使P2.3为亮灯，其他不变
    P2OUT |= 0x01 ;
    int i=1;//LED2亮灯的顺序
    int j=1,z=1;//延时计数
    int t;//j的保护数，防止j被清零，导致无法执行最后得初始函数
    int s=1;//S3的按下次数
    int time_delay=0;
    int nan1=0,nan2=0,nan3=0;//状态判断0 = 未按下，1 = 按下
    while(1){
        /*延迟函数第一段*/
        if (nan1==1 & nan2==1)
        {
            j+=1;
            Clock_Delay1ms(20);
            if(j>50) t=j;
        }
        /*控制LED1的函数段*/
        if (P1IN  &  0x02)     //   P1.1没按下，为高电平//.......按下生效......
        {
           nan1=0;
           j=0;
        }
            else//按下
            {
            if(nan1==0){//表示之前没按
              P1OUT ^= 0x01;  //亮、灭灯0000  0001使P1.0输出情况翻转，其他不变
            }
               nan1=1;
            }

        /*控制LED2的函数段*/
        if (P1IN & 0x10)//表示没按下P1.4,为高电平//.......松开生效.......
        {
            if (nan2==1){
            i+=1;
            if (i>6)   i=3;
            }
            nan2=0;
            j=0;
            }
        else//按下去了
        {
            nan2=1;
        }
        /*延迟语句*/
       if(nan1==0 &nan2==0)//P1.1和P1.4同时没按下去了，这个时候才能执行恢复语句
        {
            if(t>50)  {
                i=1;
                P1OUT=0x13;
                t=0;
            }
        }
       /*亮灯语句*/
    if(i==1 | i==3)   {
        P2OUT &= ~0x07;//关闭P2.0~P2.2
        P2OUT |= 0x01 ;
    }
    else if(i==2) {
        P2OUT &= ~0x07;//关闭P2.0~P2.2
        P2OUT |= 0x02;
    }
    else if(i==4) {
        P2OUT &= ~0x07;//关闭P2.0~P2.2
        P2OUT|= 0x04;
    }
    else if(i==5)  {
        P2OUT &= ~0x07;//关闭P2.0~P2.2
        P2OUT |= 0x03;
    }
    else if(i==6){
        P2OUT &= ~0x07;//关闭P2.0~P2.2
        P2OUT |= 0x07;
    }
    /******S3的控制程序****/
    if(P1IN & 0x20){//S3未按下
            if(nan3==1){//松开生效,之前是按下的
                P2OUT |= 0x08;
                s+=1;
            }
            /****LED3闪烁程序****/
            if(z>43*s){
                P2OUT ^= 0x08;
                z=0;
            }
            /***长按归零程序**/
            if(time_delay>120){
                s=1;
                P2OUT |= 0x08;//s使P2.3为亮灯，其他不变
            }
            time_delay=0;
            Clock_Delay1ms(20);
            nan3=0;
            z+=1;
    }
    else//按下状态
    {
        P2OUT &= ~0x08;//关闭灯
        time_delay+=1;
        Clock_Delay1ms(20);
        nan3=1;
        z=0;
    }
}//
}

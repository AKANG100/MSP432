#include<stdio.h>
#include "msp.h"
#include"Reflectance.h"
#include"UART0.h"
#include"Clock.h"
#include"PWM.h"
#include"Motor.h"

#define DARK           0x00
#define RED            0x01
#define GREEN          0x02
#define YELLOW         0x03
#define BLUE           0x04
#define PINK           0x05
#define SKYBLUE        0x06
#define WHITE          0x07

void Port2_Init(void)
{
P2->SEL0 = 0x00;
P2->SEL1 = 0x00; //configure P2.2-P2.0 as GPIO
P2->DS = 0x07; // make P2.2-P2.0 high drive strength
P2->DIR = 0x07; // make P2.2-P2.0 out
P2->OUT = 0x00; // all LEDs off
}

void Port2_Output(uint8_t data)
{
P2->OUT = data; // write all of P2 outputs
}

void main()
{
     WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer停止看门狗定时器
     Clock_Init48MHz();
     P1DIR &= 0xEC;//设置P1.0输出灯，设置P1.0,P1.1，P1.4输入按钮1110  1100
     P1DIR |= 0x01;//设置输出灯，去掉P1.0，设置P1.1，P1.4输入按钮0000 0001
     P1REN = 0x12;//使能，设置P1.1，P1.4初始为高电位0001  0010
     P1OUT = 0x13;//按下时，设置P1.0，P1.1，P1.4初始输出高电位0001  0011，为亮灯状态
     Port2_Init();
     int nan1=0,nan_1=0;//状态判断0 = 未按下，1 = 按下。nan_1为按下次数
     int nan2=0,nan_2=0;//nan_2为按下次数0 = 白纸黑线 ，1 = 黑纸白线。
     int time_dalay_1;//延时计数
     while(1)
     {
        /*S1按钮控制*/
        if (P1IN  &  0x02)     //   P1.1没按下，为高电平//.......按下生效......
              {
            if(nan_1==0){//循迹没打开
                Clock_Delay1ms(30);
                P3OUT &= ~0xc0;
                time_dalay_1+=1;
                nan_2=0;//S2状态回复白纸黑线
                P2OUT=0x00;//关闭LED2灯
                //灯灭、马达停转
            }
            if(time_dalay_1>30){
                P1OUT ^= 0x01;//翻转
                time_dalay_1=0;//延时归零
            }
            nan1=0;
              }
        else//按下S1
        {
            if(nan1==0)
            {//之前没按
                P1OUT &= ~ 0x01;//关闭LED1
                nan_1  ^=1 ;//循迹模块的开闭情况反转
                time_dalay_1=0;//延时归零
            }
            nan1=1;
        }
        /********s2的控制********/
         if (P1IN & 0x10)//表示没按下P1.4,为高电平//.......松开生效.......
         {
             nan2=0;
         }
         else//开关打开
         {
             if(nan2==0 & nan_1==1)//循迹打开时才有效
             {
                 nan_2  ^=1 ;//白纸黑线的开闭情况反转
                 P1OUT ^=  0x01;//LED1灯情况反转
             }
             nan2=1;
         }


         if(nan_1==1)
         {
             //循迹模块
             uint8_t Data;
 //            uint8_t Data1;
             UART0_Initprintf();
             Clock_Init48MHz();
             Reflectance_Init();
             Motor_Init();
             PWM_Init34(10000, 5000, 5000);

 //                /*S1按钮控制*/
 //                if (P1IN  &  0x02)     //   P1.1没按下，为高电平//.......按下生效......
 //                {
 //                    nan1=0;
 //                }
 //                else//按下
 //                {
 //                    if(nan1==0)
 //                {//之前没按
 //                        //
 //                        nan_1=0;
 //                    }
 //
 //                }
             if(nan_2==0)
             {
                 Data = Reflectance_Read(500);
             }
             else if(nan_2==1)
             {
                 Data =  ~Reflectance_Read(500);
             }
//                 Data = Reflectance_Read(500);
//                 printf("%5x",Data);
//                 Clock_Delay1ms(500);
                 switch(Data)
                 {
                     /*无路径*/
                     case 0x00:
                         Port2_Output(WHITE);
                         break;

                     /*直行*/
                     case 0x18:
                     case 0x3c:
                         Port2_Output(GREEN);
                         break;

                     /*直行向左调整*/
                     case 0x10: //00010000
                         Port2_Output(GREEN);
                     case 0x30: //00110000
                         Port2_Output(GREEN);
                     case 0x60: //01100000
                         Port2_Output(GREEN);

                      /*直行向右调整*/
                     case 0x8: //00001000
                         Port2_Output(GREEN);
                     case 0xc: //00001100
                         Port2_Output(GREEN);
                     case 0x6: //00000110
                         Port2_Output(GREEN);
                         /*左转（优先）*/
                             case 0x80: //10000000
                             case 0xc0: //11000000
                             case 0xe0: //11100000
                             case 0xf0: //11110000
                             case 0xf8: //11111000
                             case 0xfc: //11111100
                             case 0xfe: //11111110
                                 Port2_Output(BLUE);
                                 break;

                             /*右转*/
                             case 0x1:  //00000001
                             case 0x3:  //00000011
                             case 0x7:  //00000111
                             case 0xf:  //00001111
                             case 0x1f: //0001111
                             case 0x3f: //0011111
                             case 0x7f: //0111111
                                 Port2_Output(RED);
                                 break;

                             /*T型路口*/
                             case 0xff:
         ////                        Motor_Backward(3000, 3000);
         //                        Data1 = Reflectance_Read(500);
         //                        if(Data1==0x18 || Data1==0x10 || Data1==0x08)
         //                        {
         //                            Port2_Output(YELLOW);
         //                            Motor_Left(3000, 3000);/************************全黑就视为T形********************/
                                 Port2_Output(YELLOW);
                                     break;



                             /*终点*/
                             case 0xbd: //10111101
                             case 0x99: //10011001
                             case 0x9d: //10011101
                             case 0xb9: //10111001
                             case 0x89: //10001001
                             case 0x91: //10010001
                             case 0xdb: //11011011
                             case 0xd3: //11010011
                             case 0xcb: //11001011
                             case 0xb1: //10110001
                             case 0xb3: //10110011
                             case 0xcd: //11001101
                             case 0xd9: //11011001
         //                    case 0xdf: //11011111
         //                    Port2_Output(SKYBLUE);
                             Port2_Output(DARK);
                                 break;

//                             default:
//                                 Port2_Output(PINK);
//         //                        Motor_Backward(3000, 3000);
//                                 break;
                         }//swith

             }//if(nan_1=0)
          }//while
         }

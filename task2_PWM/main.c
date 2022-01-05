#include "msp.h"
#include "Clock.h"
#include <stdio.h>
#include <PWM.h>
/*****知识点*****
 * 其中.6为右边，.7为左边
 * P1.6与1.7控制正反转，0=反，1=正向前
 * P3.6与P3.7控制开闭，0=停，1=动
 */
void Port2_Init(void){
  P2->SEL0 = 0x00;
  P2->SEL1 = 0x00; //设置P2.2-P2.0 as GPIO
  P2->DS = 0x07;   // make P2.2-P2.0 high drive strength
  P2->DIR = 0x07;  // make P2.2-P2.0 out
  P2->OUT = 0x00;  // all LEDs off
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    Clock_Init48MHz();
    Port2_Init();
    P1DIR &= 0xEC;//设置P1.0输出灯，设置P1.0,P1.1，P1.4输入按钮1110  1100
     P1DIR |= 0x01;//设置输出灯，去掉P1.0，设置P1.1，P1.4输入按钮0000 0001
    P1REN = 0x12;//使能，设置P1.1，P1.4初始为高电位0001  0010
     P1OUT = 0x13;//按下时，设置P1.0，P1.1，P1.4初始输出高电位0001  0011，为亮灯状态
    P2OUT = 0x00;//关闭LED2灯
    P1->DIR |= 0xC0;    // make P1.6 & P1.7 out  1100  0000左
    P1->OUT |= 0xC0;//使P1.6 & P1.7 =1，为正转
    P3->DIR |= 0xC0;    // make P3.6 & P3.7 out  右
    P3->OUT &= ~0xC0;   // low current sleep mode
    PWM_Init1(60000, 30000);//P2.4  Period of P2.4 is period*166.67ns, duty cycle is duty/period100hz 0.5
    int nan1=0,nan2=0;//状态判断0 = 未按下，1 = 按下
     int time_dalay1=0,time_dalay2=0,time_dalay3=0;//时间延迟函数
     int dbclick1=0,dbclick2=0;//
     int db_time1=0,db_time2=0;
     int protect_number;
     while(1){
         /*****双动开关关闭****/
         if(nan1==1 & nan2==1){
             time_dalay3+=1;
             Clock_Delay1ms(20);
             protect_number=time_dalay3;
         }
         /********左轮延时函数*******/
         if(nan1==1 & nan2==0  ){
             time_dalay1+=1;
             Clock_Delay1ms(20);
         }
         /*********控制左轮Motor_1的函数段**********/
         if (P1IN  &  0x02)     //   P1IN表示这一轮的情况，P1.1没按下，为高电平//.......按下生效......
         {
             if( time_dalay1>100)
             {//3秒长按松开生效函数
                 P1OUT |= 0x80;//恢复反转待定状态 1000 0000
                 P3OUT &= ~0x80;//仅关闭左轮
             }
             if(dbclick1==1)//双击开始计数
             {
                 db_time1+=1;
                 Clock_Delay1ms(20);
                 if(db_time1>25)//超过这段时间就双击失效
                 {
                     db_time1=0;
                     dbclick1=0;
                 }
             }
             else //已经双击结束了
             {
                 db_time1=0;
                 dbclick1=0;
             }
             time_dalay1=0;//长按验证失效
             nan1=0;
         }
         else//按下状态
         {
             if(nan1==0 ){//表示之前没按/******按下生效******/
                 if(dbclick1==1){//说明双击已经按下第一次了，那这一次就是第二次按下
                     P1OUT &=~0x80;//正转
                     P2->DIR |= 0x80;          // P2.7 output
                     P2->SEL0 |= 0x80;         //  P2.7 Timer0A functions
                     P2->SEL1 &= ~0x80;        //  P2.7 Timer0A functions
                     TIMER_A0->CCTL[0] = 0x0080;      // CCI0 toggle
                     TIMER_A0->CCR[0] = 3750;       // Period is 2*period*8*83.33ns is 1.333*period 200Hz 0.3
                     TIMER_A0->EX0 = 0x0000;          // divide by 1
                     TIMER_A0->CCTL[4] = 0x0040;      // CCR4 toggle/reset
                     TIMER_A0->CCR[4] = 1125;        // CCR4 duty cycle is duty4/period
                     TIMER_A0->CTL = 0x02F0;        // SMCLK=12MHz, divide by 8, up-down mode
                     P3OUT |= 0x80;
                      dbclick1=2;//双击复位
                      P2OUT =0x02;//亮绿灯
                  }
                 else{
                     P1OUT ^= 0x80;//翻转
                     P2->DIR |= 0x80;          // P2.7 output
                     P2->SEL0 |= 0x80;         //  P2.7 Timer0A functions
                     P2->SEL1 &= ~0x80;        //  P2.7 Timer0A functions
                     TIMER_A0->CCTL[0] = 0x0080;      // CCI0 toggle
                     TIMER_A0->CCR[0] = 7500;       // Period is 2*period*8*83.33ns is 1.333*period    100Hz 0.5
                     TIMER_A0->EX0 = 0x0000;          // divide by 1
                     TIMER_A0->CCTL[4] = 0x0040;      // CCR4 toggle/reset
                     TIMER_A0->CCR[4] = 3750;        // CCR4 duty cycle is duty4/period
                     TIMER_A0->CTL = 0x02F0;        // SMCLK=12MHz, divide by 8, up-down mode
                     P3OUT |= 0x80;
                     P2OUT =0x01;//亮红灯
                     dbclick1=1;//为双击第一次
                     db_time1=0;//双击开始
                 }
             }
             nan1=1;//为下一轮循环传递这一轮循环的开闭情况
         }
         /*******右轮*****///几乎同上
                        /********右轮延时函数*******///此时s1松开
                               if(nan2==1 & nan1==0){
                                   time_dalay2+=1;
                                   Clock_Delay1ms(20);
                               }
                               /****右轮控制****/
                if (P1IN  &  0x10) {    //   P1.4没按下，为高电平//.......按下生效......
                    if( time_dalay2>50){//3秒长按*****松开生效函数******
                        P1OUT |= 0x40;//恢复反转待定状态
                        P3OUT &= ~0x40;//仅关闭右轮0100 0000
                    }
                    if(dbclick2==1)//双击开始计数
                    {
                        db_time2+=1;
                        Clock_Delay1ms(20);
                        if(db_time2>30)//超过这段时间就双击失效
                        {
                            db_time2=0;
                            dbclick2=0;
                        }
                    }
                    else //已经双击结束了
                    {
                        db_time2=0;
                        dbclick2=0;
                    }
                    nan2=0;
                    time_dalay2=0;
                }
                else//按下状态
                {
                    if(nan2==0 ){//表示之前没按/******按下生效******/
                        if(dbclick2==1){//说明双击已经按下第一次了，那这一次就是第二次按下
                            P1OUT |= 0x40;//反转
                            P2->DIR |= 0x40;          // P2.6 output
                            P2->SEL0 |= 0x40;         // P2.6Timer0A functions
                            P2->SEL1 &= ~0x40;        // P2.6 Timer0A functions
                            TIMER_A0->CCTL[0] = 0x0080;      // CCI0 toggle  300Hz  0.7
                            TIMER_A0->CCR[0] = 2500;       // Period is 2*period*8*83.33ns is 1.333*period
                            TIMER_A0->EX0 = 0x0000;          // divide by 1
                            TIMER_A0->CCTL[3] = 0x0040;      // CCR3 toggle/reset
                            TIMER_A0->CCR[3] = 1750;        // CCR3 duty cycle is duty3/period
                            TIMER_A0->CTL = 0x02F0;        // SMCLK=12MHz, divide by 8, up-down mode
                            P3OUT |= 0x40;
                            dbclick2=2;//双击复位
                            P2OUT =0x02;//亮绿灯
                        }
                        else{
                            P1OUT ^= 0x40;
                            P2->DIR |= 0x40;          // P2.6 output
                            P2->SEL0 |= 0x40;         // P2.6Timer0A functions
                            P2->SEL1 &= ~0x40;        // P2.6 Timer0A functions
                            TIMER_A0->CCTL[0] = 0x0080;      // CCI0 toggle  300Hz  0.7
                            TIMER_A0->CCR[0] = 7500;       // Period is 2*period*8*83.33ns is 1.333*period
                            TIMER_A0->EX0 = 0x0000;          // divide by 1  100Hz  0.3
                            TIMER_A0->CCTL[3] = 0x0040;      // CCR3 toggle/reset
                            TIMER_A0->CCR[3] = 2250;        // CCR3 duty cycle is duty3/period
                            TIMER_A0->CTL = 0x02F0;        // SMCLK=12MHz, divide by 8, up-down mode
                            P3OUT |= 0x40;
                            dbclick2=1;//为双击第一次
                            db_time2=0;//双击开始
                        }
                    }
                    nan2=1;//为下一轮循环传递这一轮循环的开闭情况
                }
                /*******双关延迟******/
                if(nan1==0 &nan2==0)
                       {
                    if(protect_number>100)  {
                               P1OUT |= 0xc0;//恢复正转
                               P3OUT &= ~0xc0;//仅关闭左右轮
                               protect_number=0;
                           }
                       }
     }
}


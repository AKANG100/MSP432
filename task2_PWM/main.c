#include "msp.h"
#include "Clock.h"
#include <stdio.h>
#include <PWM.h>
/*****֪ʶ��*****
 * ����.6Ϊ�ұߣ�.7Ϊ���
 * P1.6��1.7��������ת��0=����1=����ǰ
 * P3.6��P3.7���ƿ��գ�0=ͣ��1=��
 */
void Port2_Init(void){
  P2->SEL0 = 0x00;
  P2->SEL1 = 0x00; //����P2.2-P2.0 as GPIO
  P2->DS = 0x07;   // make P2.2-P2.0 high drive strength
  P2->DIR = 0x07;  // make P2.2-P2.0 out
  P2->OUT = 0x00;  // all LEDs off
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    Clock_Init48MHz();
    Port2_Init();
    P1DIR &= 0xEC;//����P1.0����ƣ�����P1.0,P1.1��P1.4���밴ť1110  1100
     P1DIR |= 0x01;//��������ƣ�ȥ��P1.0������P1.1��P1.4���밴ť0000 0001
    P1REN = 0x12;//ʹ�ܣ�����P1.1��P1.4��ʼΪ�ߵ�λ0001  0010
     P1OUT = 0x13;//����ʱ������P1.0��P1.1��P1.4��ʼ����ߵ�λ0001  0011��Ϊ����״̬
    P2OUT = 0x00;//�ر�LED2��
    P1->DIR |= 0xC0;    // make P1.6 & P1.7 out  1100  0000��
    P1->OUT |= 0xC0;//ʹP1.6 & P1.7 =1��Ϊ��ת
    P3->DIR |= 0xC0;    // make P3.6 & P3.7 out  ��
    P3->OUT &= ~0xC0;   // low current sleep mode
    PWM_Init1(60000, 30000);//P2.4  Period of P2.4 is period*166.67ns, duty cycle is duty/period100hz 0.5
    int nan1=0,nan2=0;//״̬�ж�0 = δ���£�1 = ����
     int time_dalay1=0,time_dalay2=0,time_dalay3=0;//ʱ���ӳٺ���
     int dbclick1=0,dbclick2=0;//
     int db_time1=0,db_time2=0;
     int protect_number;
     while(1){
         /*****˫�����عر�****/
         if(nan1==1 & nan2==1){
             time_dalay3+=1;
             Clock_Delay1ms(20);
             protect_number=time_dalay3;
         }
         /********������ʱ����*******/
         if(nan1==1 & nan2==0  ){
             time_dalay1+=1;
             Clock_Delay1ms(20);
         }
         /*********��������Motor_1�ĺ�����**********/
         if (P1IN  &  0x02)     //   P1IN��ʾ��һ�ֵ������P1.1û���£�Ϊ�ߵ�ƽ//.......������Ч......
         {
             if( time_dalay1>100)
             {//3�볤���ɿ���Ч����
                 P1OUT |= 0x80;//�ָ���ת����״̬ 1000 0000
                 P3OUT &= ~0x80;//���ر�����
             }
             if(dbclick1==1)//˫����ʼ����
             {
                 db_time1+=1;
                 Clock_Delay1ms(20);
                 if(db_time1>25)//�������ʱ���˫��ʧЧ
                 {
                     db_time1=0;
                     dbclick1=0;
                 }
             }
             else //�Ѿ�˫��������
             {
                 db_time1=0;
                 dbclick1=0;
             }
             time_dalay1=0;//������֤ʧЧ
             nan1=0;
         }
         else//����״̬
         {
             if(nan1==0 ){//��ʾ֮ǰû��/******������Ч******/
                 if(dbclick1==1){//˵��˫���Ѿ����µ�һ���ˣ�����һ�ξ��ǵڶ��ΰ���
                     P1OUT &=~0x80;//��ת
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
                      dbclick1=2;//˫����λ
                      P2OUT =0x02;//���̵�
                  }
                 else{
                     P1OUT ^= 0x80;//��ת
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
                     P2OUT =0x01;//�����
                     dbclick1=1;//Ϊ˫����һ��
                     db_time1=0;//˫����ʼ
                 }
             }
             nan1=1;//Ϊ��һ��ѭ��������һ��ѭ���Ŀ������
         }
         /*******����*****///����ͬ��
                        /********������ʱ����*******///��ʱs1�ɿ�
                               if(nan2==1 & nan1==0){
                                   time_dalay2+=1;
                                   Clock_Delay1ms(20);
                               }
                               /****���ֿ���****/
                if (P1IN  &  0x10) {    //   P1.4û���£�Ϊ�ߵ�ƽ//.......������Ч......
                    if( time_dalay2>50){//3�볤��*****�ɿ���Ч����******
                        P1OUT |= 0x40;//�ָ���ת����״̬
                        P3OUT &= ~0x40;//���ر�����0100 0000
                    }
                    if(dbclick2==1)//˫����ʼ����
                    {
                        db_time2+=1;
                        Clock_Delay1ms(20);
                        if(db_time2>30)//�������ʱ���˫��ʧЧ
                        {
                            db_time2=0;
                            dbclick2=0;
                        }
                    }
                    else //�Ѿ�˫��������
                    {
                        db_time2=0;
                        dbclick2=0;
                    }
                    nan2=0;
                    time_dalay2=0;
                }
                else//����״̬
                {
                    if(nan2==0 ){//��ʾ֮ǰû��/******������Ч******/
                        if(dbclick2==1){//˵��˫���Ѿ����µ�һ���ˣ�����һ�ξ��ǵڶ��ΰ���
                            P1OUT |= 0x40;//��ת
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
                            dbclick2=2;//˫����λ
                            P2OUT =0x02;//���̵�
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
                            dbclick2=1;//Ϊ˫����һ��
                            db_time2=0;//˫����ʼ
                        }
                    }
                    nan2=1;//Ϊ��һ��ѭ��������һ��ѭ���Ŀ������
                }
                /*******˫���ӳ�******/
                if(nan1==0 &nan2==0)
                       {
                    if(protect_number>100)  {
                               P1OUT |= 0xc0;//�ָ���ת
                               P3OUT &= ~0xc0;//���ر�������
                               protect_number=0;
                           }
                       }
     }
}


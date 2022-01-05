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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timerֹͣ���Ź���ʱ��
    Clock_Init48MHz();
    Port23_Init();
    P1DIR &= 0xCC;//����P1.0����ƣ�����P1.0,P1.1��P1.4��P1.5���밴ť1110  1100
    P1DIR |= 0x01;//��������ƣ�ȥ��P1.0������P1.1��P1.4��P1.5���밴ť0000 0001
    P1REN = 0x32;//ʹ�ܣ�����P1.1��P1.4,P1.5��ʼΪ�ߵ�λ0001  0010
    P1OUT = 0x33;//����ʱ������P1.0��P1.1��P1.4,P1.5��ʼ����ߵ�λ0001  0011��Ϊ����״̬
    P2OUT |= 0x08;//sʹP2.3Ϊ���ƣ���������
    P2OUT |= 0x01 ;
    int i=1;//LED2���Ƶ�˳��
    int j=1,z=1;//��ʱ����
    int t;//j�ı���������ֹj�����㣬�����޷�ִ�����ó�ʼ����
    int s=1;//S3�İ��´���
    int time_delay=0;
    int nan1=0,nan2=0,nan3=0;//״̬�ж�0 = δ���£�1 = ����
    while(1){
        /*�ӳٺ�����һ��*/
        if (nan1==1 & nan2==1)
        {
            j+=1;
            Clock_Delay1ms(20);
            if(j>50) t=j;
        }
        /*����LED1�ĺ�����*/
        if (P1IN  &  0x02)     //   P1.1û���£�Ϊ�ߵ�ƽ//.......������Ч......
        {
           nan1=0;
           j=0;
        }
            else//����
            {
            if(nan1==0){//��ʾ֮ǰû��
              P1OUT ^= 0x01;  //�������0000  0001ʹP1.0��������ת����������
            }
               nan1=1;
            }

        /*����LED2�ĺ�����*/
        if (P1IN & 0x10)//��ʾû����P1.4,Ϊ�ߵ�ƽ//.......�ɿ���Ч.......
        {
            if (nan2==1){
            i+=1;
            if (i>6)   i=3;
            }
            nan2=0;
            j=0;
            }
        else//����ȥ��
        {
            nan2=1;
        }
        /*�ӳ����*/
       if(nan1==0 &nan2==0)//P1.1��P1.4ͬʱû����ȥ�ˣ����ʱ�����ִ�лָ����
        {
            if(t>50)  {
                i=1;
                P1OUT=0x13;
                t=0;
            }
        }
       /*�������*/
    if(i==1 | i==3)   {
        P2OUT &= ~0x07;//�ر�P2.0~P2.2
        P2OUT |= 0x01 ;
    }
    else if(i==2) {
        P2OUT &= ~0x07;//�ر�P2.0~P2.2
        P2OUT |= 0x02;
    }
    else if(i==4) {
        P2OUT &= ~0x07;//�ر�P2.0~P2.2
        P2OUT|= 0x04;
    }
    else if(i==5)  {
        P2OUT &= ~0x07;//�ر�P2.0~P2.2
        P2OUT |= 0x03;
    }
    else if(i==6){
        P2OUT &= ~0x07;//�ر�P2.0~P2.2
        P2OUT |= 0x07;
    }
    /******S3�Ŀ��Ƴ���****/
    if(P1IN & 0x20){//S3δ����
            if(nan3==1){//�ɿ���Ч,֮ǰ�ǰ��µ�
                P2OUT |= 0x08;
                s+=1;
            }
            /****LED3��˸����****/
            if(z>43*s){
                P2OUT ^= 0x08;
                z=0;
            }
            /***�����������**/
            if(time_delay>120){
                s=1;
                P2OUT |= 0x08;//sʹP2.3Ϊ���ƣ���������
            }
            time_delay=0;
            Clock_Delay1ms(20);
            nan3=0;
            z+=1;
    }
    else//����״̬
    {
        P2OUT &= ~0x08;//�رյ�
        time_delay+=1;
        Clock_Delay1ms(20);
        nan3=1;
        z=0;
    }
}//
}

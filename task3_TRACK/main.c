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
     WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timerֹͣ���Ź���ʱ��
     Clock_Init48MHz();
     P1DIR &= 0xEC;//����P1.0����ƣ�����P1.0,P1.1��P1.4���밴ť1110  1100
     P1DIR |= 0x01;//��������ƣ�ȥ��P1.0������P1.1��P1.4���밴ť0000 0001
     P1REN = 0x12;//ʹ�ܣ�����P1.1��P1.4��ʼΪ�ߵ�λ0001  0010
     P1OUT = 0x13;//����ʱ������P1.0��P1.1��P1.4��ʼ����ߵ�λ0001  0011��Ϊ����״̬
     Port2_Init();
     int nan1=0,nan_1=0;//״̬�ж�0 = δ���£�1 = ���¡�nan_1Ϊ���´���
     int nan2=0,nan_2=0;//nan_2Ϊ���´���0 = ��ֽ���� ��1 = ��ֽ���ߡ�
     int time_dalay_1;//��ʱ����
     while(1)
     {
        /*S1��ť����*/
        if (P1IN  &  0x02)     //   P1.1û���£�Ϊ�ߵ�ƽ//.......������Ч......
              {
            if(nan_1==0){//ѭ��û��
                Clock_Delay1ms(30);
                P3OUT &= ~0xc0;
                time_dalay_1+=1;
                nan_2=0;//S2״̬�ظ���ֽ����
                P2OUT=0x00;//�ر�LED2��
                //�������ͣת
            }
            if(time_dalay_1>30){
                P1OUT ^= 0x01;//��ת
                time_dalay_1=0;//��ʱ����
            }
            nan1=0;
              }
        else//����S1
        {
            if(nan1==0)
            {//֮ǰû��
                P1OUT &= ~ 0x01;//�ر�LED1
                nan_1  ^=1 ;//ѭ��ģ��Ŀ��������ת
                time_dalay_1=0;//��ʱ����
            }
            nan1=1;
        }
        /********s2�Ŀ���********/
         if (P1IN & 0x10)//��ʾû����P1.4,Ϊ�ߵ�ƽ//.......�ɿ���Ч.......
         {
             nan2=0;
         }
         else//���ش�
         {
             if(nan2==0 & nan_1==1)//ѭ����ʱ����Ч
             {
                 nan_2  ^=1 ;//��ֽ���ߵĿ��������ת
                 P1OUT ^=  0x01;//LED1�������ת
             }
             nan2=1;
         }


         if(nan_1==1)
         {
             //ѭ��ģ��
             uint8_t Data;
 //            uint8_t Data1;
             UART0_Initprintf();
             Clock_Init48MHz();
             Reflectance_Init();
             Motor_Init();
             PWM_Init34(10000, 5000, 5000);

 //                /*S1��ť����*/
 //                if (P1IN  &  0x02)     //   P1.1û���£�Ϊ�ߵ�ƽ//.......������Ч......
 //                {
 //                    nan1=0;
 //                }
 //                else//����
 //                {
 //                    if(nan1==0)
 //                {//֮ǰû��
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
                     /*��·��*/
                     case 0x00:
                         Port2_Output(WHITE);
                         break;

                     /*ֱ��*/
                     case 0x18:
                     case 0x3c:
                         Port2_Output(GREEN);
                         break;

                     /*ֱ���������*/
                     case 0x10: //00010000
                         Port2_Output(GREEN);
                     case 0x30: //00110000
                         Port2_Output(GREEN);
                     case 0x60: //01100000
                         Port2_Output(GREEN);

                      /*ֱ�����ҵ���*/
                     case 0x8: //00001000
                         Port2_Output(GREEN);
                     case 0xc: //00001100
                         Port2_Output(GREEN);
                     case 0x6: //00000110
                         Port2_Output(GREEN);
                         /*��ת�����ȣ�*/
                             case 0x80: //10000000
                             case 0xc0: //11000000
                             case 0xe0: //11100000
                             case 0xf0: //11110000
                             case 0xf8: //11111000
                             case 0xfc: //11111100
                             case 0xfe: //11111110
                                 Port2_Output(BLUE);
                                 break;

                             /*��ת*/
                             case 0x1:  //00000001
                             case 0x3:  //00000011
                             case 0x7:  //00000111
                             case 0xf:  //00001111
                             case 0x1f: //0001111
                             case 0x3f: //0011111
                             case 0x7f: //0111111
                                 Port2_Output(RED);
                                 break;

                             /*T��·��*/
                             case 0xff:
         ////                        Motor_Backward(3000, 3000);
         //                        Data1 = Reflectance_Read(500);
         //                        if(Data1==0x18 || Data1==0x10 || Data1==0x08)
         //                        {
         //                            Port2_Output(YELLOW);
         //                            Motor_Left(3000, 3000);/************************ȫ�ھ���ΪT��********************/
                                 Port2_Output(YELLOW);
                                     break;



                             /*�յ�*/
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

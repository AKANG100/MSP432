#include<stdio.h>
#include "msp.h"
#include"Reflectance.h"
#include"UART0.h"
#include"Clock.h"
#include"PWM.h"
#include"Motor.h"

#define DARK             0
#define RED            0x01 //��ת
#define GREEN          0x02 //ֱ��
#define YELLOW         0x03 //T����ת��ʮ����ת
#define BLUE           0x04 //��ת
#define PINK           0x05 //��ֱ->ֱ��
#define SKYBLUE        0x06 //��ֱ->ֱ��
#define WHITE          0x07 //��ͷ

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

/*
 * ʮ��·�� ������ת
 * ֱ�м���ת ����ֱ��
 * ֱ�м���ת ������ת
 * T�� ������ת
 *�ٶ� 5000
 *��ʱ 20ms
 */

void main(void)
{
    uint8_t Data;
    uint8_t Data1;

    /*��ʼ��*/
    UART0_Initprintf();
    Clock_Init48MHz();
    Reflectance_Init();
    Port2_Init();
    Motor_Init();
    PWM_Init34(10000, 5000, 5000);

    while(1)
    {
        Data = Reflectance_Read(500);
        printf("%5x",Data);
        Clock_Delay1ms(500);

        switch(Data)
        {
            /*��·��->��ͷ*/
            case 0x00: //00000000
                Port2_Output(WHITE);
                Motor_Backward(4000, 4000);
                break;

//            /*ֱ��->���м�*/
//            case 0x18: //00011000
//            case 0x3c: //00111100
//                Port2_Output(GREEN);
//                Motor_Forward(4000, 000);
//                break;
//
//            /*ֱ��->�������*/
//            case 0x10: //00010000
//                Port2_Output(GREEN);
//                Motor_Forward(3000, 2000);
//                Clock_Delay1ms(10);
//                break;
//            case 0x30: //00110000
//                Port2_Output(GREEN);
//                Motor_Forward(3000, 1000);
//                Clock_Delay1ms(10);
//                break;
//            case 0x60: //01100000
//                Port2_Output(GREEN);
//                Motor_Forward(3000, 500);
//                Clock_Delay1ms(10);
//                break;
//
//             /*ֱ��->���ҵ���*/
//            case 0x8: //00001000
//                Port2_Output(GREEN);
//                Motor_Forward(2000, 3000);
//                Clock_Delay1ms(10);
//                break;
//            case 0xc: //00001100
//                Port2_Output(GREEN);
//                Motor_Forward(1000, 3000);
//                Clock_Delay1ms(10);
//                break;
//            case 0x6: //00000110
//                Port2_Output(GREEN);
//                Motor_Forward(500,3000);
//                Clock_Delay1ms(10);
//                break;


            /*��ת*/
            /*ֱ�м���ת->������ת*/
            case 0x80: //10000000
            case 0xc0: //11000000
                Port2_Output(BLUE);
                Motor_Left(4000, 4000);
                Clock_Delay1ms(50);
                break;
            case 0xe0: //11100000
            case 0xf0: //11110000
            case 0xf8: //11111000
                Port2_Output(BLUE);
                Motor_Left(4000, 4000);
                Clock_Delay1ms(40);
                break;
            case 0xfc: //11111100
            case 0xfe: //11111110
                Port2_Output(BLUE);
                Motor_Left(4000,4000 );
                Clock_Delay1ms(20);
                break;



            /*��ת*/
            /*ֱ�м���ת->����ֱ��*/
            case 0x1:  //00000001
            case 0x3:  //00000011
            case 0x7:  //00000111
            case 0xf:  //00001111
            case 0x1f: //0001111
            case 0x3f: //0011111
            case 0x7f: //0111111
                Motor_Forward(3000,3000);
                Clock_Delay1ms(10);
                Data1 = Reflectance_Read(100);
                if(Data1==0x00) //��ת
                {
                    Motor_Backward(3000, 3000);
                    Clock_Delay1ms(10);
                    Port2_Output(RED);
                    Motor_Right(4000, 4000);
                    Clock_Delay1ms(30);
                }
                else            //ֱ��
                {
//                    Motor_Backward(3000, 3000);
//                    Clock_Delay1ms(10);
                    Port2_Output(SKYBLUE);
                    Motor_Forward(4000,4000);
                }
                Motor_Right(4000, 4000);
                Clock_Delay1ms(30);
                break;


            /*�յ�->ֹͣ*/
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
                Port2_Output(SKYBLUE);
                Motor_Stop();
                break;


            /*T ��·��->������ת*/
            /*ʮ��·��->������ת*/
            case 0xff:
                Port2_Output(YELLOW);
                Motor_Left(3000, 3000);
                Clock_Delay1ms(40);
                break;

//                Motor_Forward(3000,3000);
//                Clock_Delay1ms(20);
//
//                Data1 = Reflectance_Read(100);
//                if(Data1==0x00)
//                {
//                    Port2_Output(YELLOW);
//                    Motor_Backward(3000, 3000);
//                    Clock_Delay1ms(20);
//                    Motor_Left(4000,4000);
//                    Clock_Delay1ms(20);
//                    break;
//                }
//                else if(Data1=0x18)
//                {
//                    Port2_Output(PINK);
//                    Motor_Backward(3000, 3000);
//                    Clock_Delay1ms(20);
//                    Motor_Left(4000,4000);
//                    Clock_Delay1ms(20);
//                    break;
//                }

//            default:
//                Port2_Output(PINK);
//                Motor_Backward(3000, 3000);
//                break;

        }

    }
}

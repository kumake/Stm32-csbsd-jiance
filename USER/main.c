#include "led.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "control.h"
#include "ad9850.h"
#include "adc.h"
#include "sht10.h"
#include "24c02.h"
#include "key.h"
#include "usart2.h"


//ALIENTEKս��STM32������ʵ��1
//�����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
#define CMD_BUF_MAX  5
u8 CMD_BUF[CMD_BUF_MAX];
u8 CMD_POINT=0;
u8 ee_out_value,ee_mode_value;//�����ʽ������ģʽ

#define RET_BUF_MAX  5
u8 RET_BUF[RET_BUF_MAX];
u8 ret_flag=0;

u8 measure_flag=0;

 int main(void)
 {
 	u16 adcx;
	float adc_voltage;

	u8 show_buf[100];//��ʾ������

    //�¶�ת�����
    float TempResult;
    //ʪ��ת�����
    float HumResult;

	

 
 	delay_init();	    //��ʱ������ʼ��
 	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600

	usart2_init(9600);
 	
	
	CON_Init();	  
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();	 		//�����˿ڳ�ʼ��
	

	Adc_Init();		  		//ADC��ʼ��
	SHT10_GPIO_Init();      //SHT10��ʼ��

	//��ʼ��AD9850
	Ad9850_Init();
    AD9850_DATAOUT(0xaa);
	Ad9850_Reset();

	//��ʼ��AT24C02
	I2C_EE_Init();



	


	//����12V��Դ
	CON_DC12=1;

	//��дEEPROM
	ee_out_value='0';
	ee_mode_value='0';
	I2C_EE_BufferRead(&ee_out_value, EE_OUT_ADDRESS, 1);
	I2C_EE_BufferRead(&ee_mode_value, EE_MODE_ADDRESS, 1);
	if (ee_out_value=='m') //max232���
	{
		CON_OUT_232=1;//�̵�����max3232
		LED_OUT_TTL=0;
		LED_OUT_232=1;//��LED
	} else
	{
		CON_OUT_232=0;//�̵�����ttl
		LED_OUT_232=0;
		LED_OUT_TTL=1;//��LED

		if (ee_out_value!='t')
		{
			I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
		}
	}

	if (ee_mode_value=='c') //����ģʽ
	{
		LED_CONTINUE=1;
	} else					//����ģʽ
	{
		LED_CONTINUE=0;
		if (ee_out_value!='s')
		{
			I2C_EE_BufferWrite("s", EE_MODE_ADDRESS, 1);
			ee_mode_value='s';
		}
	}

	while(1)
	{

		if 	(ret_flag!=0)
		{
		 	ret_flag=0;
			printf("%s",RET_BUF);
			usart2_putstr(RET_BUF);
			RET_BUF[0]=0;
		}

		if (ee_mode_value=='c') //����ģʽ 
			measure_flag=1;

		if (measure_flag!=1)
		continue;

		measure_flag=0;

		//---------������ʼ------------------------------
	   	LED_MEASURE=1;

		//����400KHZ�����ź�
		Ad9850_Wr_Parrel(0x00,400000);

		//��ʱ1S
		delay_ms(1000);	 //��ʱ1000ms
		//��ʱ0.5S
		delay_ms(500);	 //��ʱ1000ms


		//����ʪ��
		SHT10_SoftReset();
		SHT10_Read( &HumResult, &TempResult);
		//sprintf((char *)show_buf, "T: %2.1fC    H: %2.1f%%\n",TempResult,HumResult );
		//printf("%s",show_buf);
		//usart2_putstr(show_buf);

		//���ѹ
		adcx=Get_Adc_Average(ADC_Channel_10,10);
		adc_voltage=(float)adcx*(3.0/4096);


		sprintf((char *)show_buf, ":V %2.1f %2.1f %.3f\n",TempResult,HumResult,adc_voltage);
		printf("%s",show_buf);
		usart2_putstr(show_buf);


		//ֹͣ400KHZ�����ź�
		Ad9850_Wr_Parrel(0x04,400000);
		LED_MEASURE=0;

		
		delay_ms(1000);	 //��ʱ1000ms
		delay_ms(1000);	 //��ʱ1000ms
	   //---------��������----------------------------------------------------

	   
	}
 }


 void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		
		
		CMD_BUF[CMD_POINT]=	Res;
	   	CMD_POINT++;

		if (CMD_BUF[0]!=':')
		{
			CMD_POINT=0;
		}

		if 	((CMD_POINT>=CMD_BUF_MAX)|| (Res=='\n'))
		{
			CMD_POINT=0;

			switch(CMD_BUF[1])
			{
				case 'M': //max3232���
				case 'm':
				 			I2C_EE_BufferWrite("m", EE_OUT_ADDRESS, 1);
							ee_out_value='m';
				 			CON_OUT_232=1;//�̵�����max3232
							LED_OUT_TTL=0;
							LED_OUT_232=1;//��LED
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'T': //TTL���
				case 't':
				 			I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
							ee_out_value='t';
				 			CON_OUT_232=0;//�̵�����ttl
							LED_OUT_232=0;
							LED_OUT_TTL=1;//��LED
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'C': //����ģʽ
				case 'c':
				 			I2C_EE_BufferWrite("c", EE_MODE_ADDRESS, 1);
							ee_mode_value='c';
				 			LED_CONTINUE=1;
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'S': //����ģʽ
				case 's':
				 			I2C_EE_BufferWrite("s", EE_MODE_ADDRESS, 1);
							ee_mode_value='s';
				 			LED_CONTINUE=0;
							sprintf((char *)RET_BUF, ":%c\n",CMD_BUF[1]);
							ret_flag=1;
							break;
				case 'X': //���β�������
				case 'x':
				 			measure_flag=1;
							break;
				case 'Z': //״̬��ѯ����
				case 'z':
							I2C_EE_BufferRead(&ee_out_value, EE_OUT_ADDRESS, 1);
							I2C_EE_BufferRead(&ee_mode_value, EE_MODE_ADDRESS, 1);
				 			sprintf((char *)RET_BUF, ":%c %c %c\n",CMD_BUF[1],ee_out_value,ee_mode_value);
							ret_flag=1;
							break;

				default:	
							break;
			}
		}

     }

} 

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	u8 tempbuf;
	delay_ms(10);//����
	if(KEY_SELECT==0)	 	 //����������
	{				 
		
			    I2C_EE_BufferRead(&tempbuf, EE_OUT_ADDRESS, 1);
				if (tempbuf=='t')	//ԭ����ttl���ڱ��max
				{
					CON_OUT_232=1;//�̵�����max3232
					LED_OUT_TTL=0;
					LED_OUT_232=1;//��LED
					I2C_EE_BufferWrite("m", EE_OUT_ADDRESS, 1);
					sprintf((char *)RET_BUF, ":m\n");
					ret_flag=1;	
				} else			  //ԭ����max���ڱ��ttl
				{
					CON_OUT_232=0;//�̵�����ttl
					LED_OUT_232=0;
					LED_OUT_TTL=1;//��LED
					I2C_EE_BufferWrite("t", EE_OUT_ADDRESS, 1);
					sprintf((char *)RET_BUF, ":t\n");
					ret_flag=1;	
				}
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}

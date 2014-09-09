/* Includes ------------------------------------------------------------------*/
#include "sht10.h"
#include "delay.h"
#include "usart.h"	
/* Private typedef -----------------------------------------------------------*/






/**********************************************************************************
 * �ļ���      ��sht10.c
 * ����        ��sht10��غ���         
 * ����        ��DJF
 * ��������    ��2013-6-28
 * �޸�����    ��2013-6-28 
 * �޸�˵��    ���״α�д
**********************************************************************************/

/* �Զ���ͬ��ؼ��� ---------------------------------------------------------- */

#define ACK 1
#define noACK 0

/* �Զ�������� -------------------------------------------------------------- */
/* �Զ��庯���� -------------------------------------------------------------- */

#define DATA_H() GPIO_SetBits( GPIO_DATA, GPIO_DATA_PIN )
#define DATA_L() GPIO_ResetBits( GPIO_DATA,GPIO_DATA_PIN )
#define SCK_H() GPIO_SetBits( GPIO_SCK, GPIO_SCK_PIN )
#define SCK_L() GPIO_ResetBits( GPIO_SCK, GPIO_SCK_PIN )

#define DATA_R() GPIO_ReadInputDataBit(GPIO_DATA,GPIO_DATA_PIN) //�� DATA ��������

/* �Զ������ ---------------------------------------------------------------- */

//u8 t_num[2] = {0,0};// ��Ŵ� SHT10 ����ȡ�����¶ȳ�ʼֵ 
//u8 h_num[2] = {0,0};// ��Ŵ� SHT10 ����ȡ����ʪ�ȳ�ʼֵ
u8 check_num[1] = {0};// ��Ŵ� SHT10 ����ȡ���� CRC У��ֵ 

/* �Զ��庯������ ------------------------------------------------------------ */

void SHT10_DATA_OUT(void);
void SHT10_DATA_IN(void);
void SHT10_Delay(void);

/********************************************************************************
 * ������    ��SHT10_Start
 * ����      ��SHT10�����������
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/

void SHT10_Start(void)
{
	
	SHT10_DATA_OUT();
	SCK_L();
	SHT10_Delay();
	SCK_H();
	SHT10_Delay();
	DATA_L();
	SHT10_Delay();
	SCK_L();
	SHT10_Delay();
	SCK_H();
	SHT10_Delay();
	DATA_H();
	SHT10_Delay();
	SCK_L();
	
}

/********************************************************************************
 * ������    ��SHT10_GPIO_Init
 * ����      ��SHT10 GPIO��ʼ���������� SHT10_Init() ��������
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/

void SHT10_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//ʹ�� SHT10 ���ݶ˿ڣ�ʱ�Ӷ˿� ����������ʱ��
	RCC_APB2PeriphClockCmd(RCC_GPIO_DATA ,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_GPIO_SCK ,ENABLE);

	//�������
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
	//�������
	GPIO_InitStructure.GPIO_Pin = GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_SCK, &GPIO_InitStructure);
}

/********************************************************************************
 * ������    ��SHT10_DATA_OUT
 * ����      ��SHT10 DATA���Ŷ�Ӧ�� stm32 ��������Ϊ���ģʽ
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/
void SHT10_DATA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//�������
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
}

/********************************************************************************
 * ������    ��SHT10_DATA_IN
 * ����      ��SHT10 DATA���Ŷ�Ӧ�� stm32 ��������Ϊ����ģʽ
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/
void SHT10_DATA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//�������룬ʹ���ⲿ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
}

/********************************************************************************
 * ������    ��SHT10_ReadByte
 * ����      ���� SHT10 ��ȡ8λ����,�ɲ��� Ack ��ֵ�ж��Ƿ����ͨ�š�
 *             ��ȡ CRC 8λУ��֮ǰ���� Ack=0 ���ʾ��ʹ�� CRC
 * �������  : u8 �͵� Ack ����Ϊ�ж��Ƿ����ͨ�ţ�Ack=0 ��ʾ����ͨ��
 * ������  ����
 * ����ֵ    ����������ȡ�� 8λ ����
 ********************************************************************************/

u8 SHT10_ReadByte( u8 Ack )
{
	u8 i, val=0;
	
	//���˸��ֽڵ����ݣ�SCK �ߵ�ƽʱ���� SHT10 �����ݣ������� SCK һ���͵�ƽ��֮�����һλ
	SHT10_DATA_IN();
	for( i=0x80; i>0; i/=2 )
	{
		SHT10_Delay();
		SCK_H();
		SHT10_Delay();
		if( DATA_R() )
			val = ( val | i );
		SCK_L();
	}
	
	//�ж��Ƿ����ͨ�ţ��������Ӧ��ƽ
	SHT10_DATA_OUT();
	if(Ack)
		DATA_L();
	else
		DATA_H();
	//��Ҫ���¸� SHT10 һ���͵�ƽ��ȷ�� DATA �����Ч
	SHT10_Delay();
	SCK_H();
	SHT10_Delay();
	SCK_L();
	SHT10_Delay();
	
	return val;
}

/********************************************************************************
 * ������    ��SHT10_WriteByte
 * ����      ���� SHT10 д��8λ����,���� value ��ʾҪд���ֵ
 *             д��ɹ��򷵻� 0 �����ɹ��򷵻� 1 ��
 * �������  : u8 �͵� value ��ʾҪд���ֵ
 * ������  ����
 * ����ֵ    ��u8�� д��ɹ��򷵻� 0 �����ɹ��򷵻� 1 ��
 ********************************************************************************/

u8 SHT10_WriteByte( u8 value )
{
	u8 i, error = 0;

	SHT10_DATA_OUT();
	
	for( i=0x80; i>0; i/=2 )
	{
		if( i & value )
			DATA_H();
		else
			DATA_L();
		
		SHT10_Delay();
		SCK_H();
		SHT10_Delay();
		SCK_L();
		SHT10_Delay();
	}
	
	SHT10_DATA_IN();
	
	SCK_H();
	SHT10_Delay();
	error = DATA_R();//�ڵ� 9 ��ʱ�������ض�ȡ SHT10 �� DATA �����ݣ�Ϊ�͵�ƽ��˵��������ȷ�Ľ��յ�ָ��
	SCK_L();
	SHT10_Delay();
	
	return error;
}

/********************************************************************************
 * ������    ��SHT10_ConnectionReset
 * ����      ��SHT10 ͨѶ�ж�ʱ��λ��״̬�Ĵ���������Ȼ����
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/

void SHT10_ConnectionReset(void)
{
	u8 i;
	
	SHT10_DATA_OUT();
	
	DATA_H();
	SCK_L();
	
	for( i=0; i<9; i++ )
	{
		SCK_H();
		SHT10_Delay();
		SCK_L();
		SHT10_Delay();
	}
	
	SHT10_Start();
}

/********************************************************************************
 * ������    ��SHT10_ConnectionReset
 * ����      ��SHT10 ��λ���򣬸�λ�ӿڣ����״̬�Ĵ���ΪĬ��ֵ
 *             ��һ����ǰ���ٵȴ�11ms
 * �������  : ��
 * ������  ����
 * ����ֵ    ����λ�ɹ��򷵻� 0 �����ɹ��򷵻�1
 ********************************************************************************/

u8 SHT10_SoftReset(void)
{
	u8 error = 0;
	SHT10_ConnectionReset();
	error = SHT10_WriteByte(0x1E);// 0x1E Ϊ SHT10 ��λ���� 11110 
	
	return error;
}

/********************************************************************************
 * ������    ��SHT10_Measure
 * ����      ��SHT10 �¶Ȼ�ʪ�ȳ�ʼֵ��ȡ             
 * �������  : u8* pValue ָ���¶Ȼ�ʪ�ȳ�ʼֵ
 *             u8* pCheckSum ָ�� CRC У��ֵ
 *             u8 mode ѡ����Ҫ��ȡ�¶�ֵ����ʪ��ֵ��TEMP��ʾ�¶ȣ�HUMI��ʾʪ��
 * ������  ����
 * ����ֵ    ����ȡ�ɹ��򷵻� 0 �����ɹ��򷵻�1
 ********************************************************************************/

u8 SHT10_Measure( u16* pValue, u8* pCheckSum, u8 mode )
{
	u8 i = 0;
	u8 error = 0;
	
	SHT10_Start();
	switch( mode )
	{
		case TEMP: 
			error += SHT10_WriteByte(0x03);
			break;
		case HUMI:
			error += SHT10_WriteByte(0x05);
			break;
		default:
			break;
	}
	
	//�ȴ�������������Լ��Ҫ 20/80/320ms �ֱ��Ӧ 8/12/14bit ����
	if( error == 0 )
	{
		SHT10_DATA_IN();
		for( i=0; i<16; i++ )
		{
			delay_ms(20);
			if( 0 == DATA_R() )
			{
				break;
			}
		}
		if ( i >= 16 )
			return 1;

		*pValue = SHT10_ReadByte( ACK ) << 8;
		*pValue += SHT10_ReadByte( ACK );
		*pCheckSum = SHT10_ReadByte( noACK );
	}
	
	return error;
}

/********************************************************************************
 * ������    ��SHT10_Calc
 * ����      ��SHT10 �¶Ⱥ�ʪ�ȵļ���
 *             ����������Ӧϵ����ͷ�ļ��пɰ�����ѡ��
 * �������  :  *pHumidity ָ�� SHT10 ʪ��ֵ
 *              *pTemperature ָ�� SHT10 �¶�ֵ
 *              u8* prh ָ����Ҫ�����ʪ�ȳ�ʼֵ  
 *              u8* pt ָ����Ҫ������¶ȳ�ʼֵ
 * ������  ����
 * ����ֵ    ������ʪ�ȵļ��������������������ָ��ַ
 ********************************************************************************/

void SHT10_Calc( u16* prh, u16* pt, float* pHumidity, float* pTemperature )
{
	//ʪ��ת��ϵ����Ӧ���� 12bit ʪ�ȼ���
const float C1 = -4.0;
const float C2 = 0.0405;
const float C3 = -0.0000028;

//ʪ�ȵ��¶Ȳ���ϵ����Ӧ���� 12bit ʪ�ȼ���
const float T1 = 0.01;
const float T2 = 0.00008;

//ʪ��ת��ϵ����Ӧ���� 8bit ʪ�ȼ���
//const float C1 = -4.0;
//const float C2 = 0.648;
//const float C3 = -0.00072;

//ʪ�ȵ��¶Ȳ���ϵ����Ӧ���� 8bit ʪ�ȼ���
//const float T1 = 0.01;
//const float T2 = 0.00128;


//�¶�ת��ϵ������Ӧ VDD = 5V
//const float D1 = -40;

//�¶�ת��ϵ������Ӧ VDD = 4V
//const float D1 = -39.8;

//�¶�ת��ϵ������Ӧ VDD = 3.5V
const float D1 = -39.7;

//�¶�ת��ϵ������Ӧ VDD = 3V
//const float D1 = -39.60;

//�¶�ת��ϵ������Ӧ 14bit �¶ȼ���
const float D2 = 0.01;

//�¶�ת��ϵ������Ӧ 12bit �¶ȼ���
//const float D2 = 0.04;
	
	u16 rh = * prh;
	u16 t = * pt;
	float rh_linear = 0;     //���ʪ��
	float humidity = 0;     //�����¶Ȳ����������ʪ��
	float temperature = 0;  //�¶ȼ�����ֵ
	
	temperature = D1+D2*t;
	
	rh_linear = C1 + C2*rh + C3 * rh*rh;
	humidity = ( temperature - 25 ) * ( T1 + T2*rh )+ rh_linear;
	
	if( temperature > 120 ) 
		temperature = 120;
	if( temperature < -40 ) 
		temperature = -40;
	
	if( humidity > 100 )
		humidity = 100;
	if( humidity <0.1 )
		humidity = 0.1;
	
	*pHumidity = humidity;
	*pTemperature = temperature;
}

/********************************************************************************
 * ������    ��SHT10_Calc
 * ����      ��SHT10 �¶Ⱥ�ʪ�ȵļ���
 *             ����������Ӧϵ����ͷ�ļ��пɰ�����ѡ��
 * �������  :  float* pHumidity ָ�� SHT10 ʪ��ֵ
 *              float* pTemperature ָ��SHT10 �¶�ֵ
 * ������  ����
 * ����ֵ    ������ʪ�ȵļ��������������������ָ��ַ
 ********************************************************************************/

u8 SHT10_Read( float* pHumidity, float* pTemperature )
{
	u8 error = 0;
	u16 t_num = 0;
	u16 h_num = 0;
	
	if ( SHT10_Measure( &h_num, check_num, HUMI ) )
	{
		if ( SHT10_Measure( &h_num, check_num, HUMI ) ) 
			error += 1;
	}
		
	if ( SHT10_Measure( &t_num, check_num, TEMP ) ) 
		error += 1;
		
	SHT10_Calc( &h_num, &t_num, pHumidity, pTemperature );
	
	return error;
}

/********************************************************************************
 * ������    ��SHT10_Delay
 * ����      ��SHT10 SCK ��ʱ���� ���� SCK_TIME ��ֵ��ȷ����ʱʱ��
 * �������  : ��
 * ������  ����
 * ����ֵ    ����
 ********************************************************************************/
void SHT10_Delay(void)
{
	delay_us(SCK_TIME);
}

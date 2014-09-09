/* Includes ------------------------------------------------------------------*/
#include "sht10.h"
#include "delay.h"
#include "usart.h"	
/* Private typedef -----------------------------------------------------------*/






/**********************************************************************************
 * 文件名      ：sht10.c
 * 描述        ：sht10相关函数         
 * 作者        ：DJF
 * 创作日期    ：2013-6-28
 * 修改日期    ：2013-6-28 
 * 修改说明    ：首次编写
**********************************************************************************/

/* 自定义同义关键字 ---------------------------------------------------------- */

#define ACK 1
#define noACK 0

/* 自定义参数宏 -------------------------------------------------------------- */
/* 自定义函数宏 -------------------------------------------------------------- */

#define DATA_H() GPIO_SetBits( GPIO_DATA, GPIO_DATA_PIN )
#define DATA_L() GPIO_ResetBits( GPIO_DATA,GPIO_DATA_PIN )
#define SCK_H() GPIO_SetBits( GPIO_SCK, GPIO_SCK_PIN )
#define SCK_L() GPIO_ResetBits( GPIO_SCK, GPIO_SCK_PIN )

#define DATA_R() GPIO_ReadInputDataBit(GPIO_DATA,GPIO_DATA_PIN) //读 DATA 引脚数据

/* 自定义变量 ---------------------------------------------------------------- */

//u8 t_num[2] = {0,0};// 存放从 SHT10 所读取到的温度初始值 
//u8 h_num[2] = {0,0};// 存放从 SHT10 所读取到的湿度初始值
u8 check_num[1] = {0};// 存放从 SHT10 所读取到的 CRC 校验值 

/* 自定义函数声明 ------------------------------------------------------------ */

void SHT10_DATA_OUT(void);
void SHT10_DATA_IN(void);
void SHT10_Delay(void);

/********************************************************************************
 * 函数名    ：SHT10_Start
 * 描述      ：SHT10启东传输程序
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
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
 * 函数名    ：SHT10_GPIO_Init
 * 描述      ：SHT10 GPIO初始化函数，由 SHT10_Init() 函数调用
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
 ********************************************************************************/

void SHT10_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//使能 SHT10 数据端口，时钟端口 引脚所在组时钟
	RCC_APB2PeriphClockCmd(RCC_GPIO_DATA ,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_GPIO_SCK ,ENABLE);

	//推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
	//推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_SCK, &GPIO_InitStructure);
}

/********************************************************************************
 * 函数名    ：SHT10_DATA_OUT
 * 描述      ：SHT10 DATA引脚对应的 stm32 引脚设置为输出模式
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
 ********************************************************************************/
void SHT10_DATA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
}

/********************************************************************************
 * 函数名    ：SHT10_DATA_IN
 * 描述      ：SHT10 DATA引脚对应的 stm32 引脚设置为输入模式
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
 ********************************************************************************/
void SHT10_DATA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
	GPIO_InitStructure.GPIO_Pin = GPIO_DATA_PIN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入，使用外部上拉电阻
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上啦输入
	GPIO_Init(GPIO_DATA, &GPIO_InitStructure);
}

/********************************************************************************
 * 函数名    ：SHT10_ReadByte
 * 描述      ：从 SHT10 读取8位数据,由参数 Ack 的值判断是否结束通信。
 *             读取 CRC 8位校验之前输入 Ack=0 则表示不使用 CRC
 * 输入参数  : u8 型的 Ack ，作为判断是否结束通信，Ack=0 表示结束通信
 * 输出结果  ：无
 * 返回值    ：返回所读取的 8位 数据
 ********************************************************************************/

u8 SHT10_ReadByte( u8 Ack )
{
	u8 i, val=0;
	
	//读八个字节的数据，SCK 高电平时读出 SHT10 的数据，读完后给 SCK 一个低电平，之后读下一位
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
	
	//判断是否结束通信，并输出相应电平
	SHT10_DATA_OUT();
	if(Ack)
		DATA_L();
	else
		DATA_H();
	//需要重新给 SHT10 一个低电平以确保 DATA 输出有效
	SHT10_Delay();
	SCK_H();
	SHT10_Delay();
	SCK_L();
	SHT10_Delay();
	
	return val;
}

/********************************************************************************
 * 函数名    ：SHT10_WriteByte
 * 描述      ：向 SHT10 写入8位数据,参数 value 表示要写入的值
 *             写入成功则返回 0 ，不成功则返回 1 。
 * 输入参数  : u8 型的 value 表示要写入的值
 * 输出结果  ：无
 * 返回值    ：u8型 写入成功则返回 0 ，不成功则返回 1 。
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
	error = DATA_R();//在第 9 个时钟上升沿读取 SHT10 的 DATA 的数据，为低电平则说明：已正确的接收到指令
	SCK_L();
	SHT10_Delay();
	
	return error;
}

/********************************************************************************
 * 函数名    ：SHT10_ConnectionReset
 * 描述      ：SHT10 通讯中断时复位，状态寄存器内容任然保留
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
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
 * 函数名    ：SHT10_ConnectionReset
 * 描述      ：SHT10 软复位程序，复位接口，清空状态寄存器为默认值
 *             下一命令前至少等待11ms
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：复位成功则返回 0 ，不成功则返回1
 ********************************************************************************/

u8 SHT10_SoftReset(void)
{
	u8 error = 0;
	SHT10_ConnectionReset();
	error = SHT10_WriteByte(0x1E);// 0x1E 为 SHT10 软复位命令 11110 
	
	return error;
}

/********************************************************************************
 * 函数名    ：SHT10_Measure
 * 描述      ：SHT10 温度或湿度初始值读取             
 * 输入参数  : u8* pValue 指向温度或湿度初始值
 *             u8* pCheckSum 指向 CRC 校验值
 *             u8 mode 选择需要读取温度值还是湿度值，TEMP表示温度，HUMI表示湿度
 * 输出结果  ：无
 * 返回值    ：读取成功则返回 0 ，不成功则返回1
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
	
	//等待测量结束。大约需要 20/80/320ms 分别对应 8/12/14bit 测量
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
 * 函数名    ：SHT10_Calc
 * 描述      ：SHT10 温度和湿度的计算
 *             计算所需相应系数在头文件中可按需求选择
 * 输入参数  :  *pHumidity 指向 SHT10 湿度值
 *              *pTemperature 指向 SHT10 温度值
 *              u8* prh 指向需要计算的湿度初始值  
 *              u8* pt 指向需要计算的温度初始值
 * 输出结果  ：无
 * 返回值    ：将温湿度的计算结果放入输入参数中所指地址
 ********************************************************************************/

void SHT10_Calc( u16* prh, u16* pt, float* pHumidity, float* pTemperature )
{
	//湿度转换系数，应用于 12bit 湿度计算
const float C1 = -4.0;
const float C2 = 0.0405;
const float C3 = -0.0000028;

//湿度的温度补偿系数，应用于 12bit 湿度计算
const float T1 = 0.01;
const float T2 = 0.00008;

//湿度转换系数，应用于 8bit 湿度计算
//const float C1 = -4.0;
//const float C2 = 0.648;
//const float C3 = -0.00072;

//湿度的温度补偿系数，应用于 8bit 湿度计算
//const float T1 = 0.01;
//const float T2 = 0.00128;


//温度转换系数，对应 VDD = 5V
//const float D1 = -40;

//温度转换系数，对应 VDD = 4V
//const float D1 = -39.8;

//温度转换系数，对应 VDD = 3.5V
const float D1 = -39.7;

//温度转换系数，对应 VDD = 3V
//const float D1 = -39.60;

//温度转换系数，对应 14bit 温度计算
const float D2 = 0.01;

//温度转换系数，对应 12bit 温度计算
//const float D2 = 0.04;
	
	u16 rh = * prh;
	u16 t = * pt;
	float rh_linear = 0;     //相对湿度
	float humidity = 0;     //经过温度补偿修正后的湿度
	float temperature = 0;  //温度计算后的值
	
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
 * 函数名    ：SHT10_Calc
 * 描述      ：SHT10 温度和湿度的计算
 *             计算所需相应系数在头文件中可按需求选择
 * 输入参数  :  float* pHumidity 指向 SHT10 湿度值
 *              float* pTemperature 指向SHT10 温度值
 * 输出结果  ：无
 * 返回值    ：将温湿度的计算结果放入输入参数中所指地址
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
 * 函数名    ：SHT10_Delay
 * 描述      ：SHT10 SCK 延时函数 根据 SCK_TIME 的值，确定延时时间
 * 输入参数  : 无
 * 输出结果  ：无
 * 返回值    ：无
 ********************************************************************************/
void SHT10_Delay(void)
{
	delay_us(SCK_TIME);
}

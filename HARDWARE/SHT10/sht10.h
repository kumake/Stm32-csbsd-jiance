/**********************************************************************************
 * 文件名      ：sht10.h
 * 描述        ：sht10.c 的头文件，使用 sht10.c 相关函数时包含此头文件即可
 * 作者        ：DJF
 * 创作日期    ：2013-6-28
 * 修改日期    ：2013-6-28 
 * 修改说明    ：首次编写
**********************************************************************************/

#ifndef __SHT10_H
#define __SHT10_H

/* 头文件 --------------------------------------------------------------------- */
#include "sys.h"


/* 供外部调用函数声明 ------------------------------------------------------------ */

void SHT10_GPIO_Init(void);//使用 SHT10 之前 需要先调用此函数对相关引脚进行初始化
u8 SHT10_Read( float* pHumidity, float* pTemperature );//SHT10 温湿度读取函数，读取成功返回0，否则返回1；
                                                     //温湿度值通过 输入参数 的指针传递
u8 SHT10_SoftReset(void);

/* 自定义同义关键字 ---------------------------------------------------------- */

#define GPIO_DATA       GPIOB                   //  SHT10数据端口引脚所在组
#define GPIO_DATA_PIN   GPIO_Pin_11             //  SHT10数据端口引脚号
#define RCC_GPIO_DATA   RCC_APB2Periph_GPIOB  //  SHT10数据引脚时钟组

#define GPIO_SCK       GPIOB                   //  SHT10时钟端口引脚所在组
#define GPIO_SCK_PIN   GPIO_Pin_10             //  SHT10时钟端口引脚号
#define RCC_GPIO_SCK   RCC_APB2Periph_GPIOB  //  SHT10时钟引脚时钟组

#define SCK_TIME 10 //确定 SHT10 SCK 的频率，频率值为 ( 0.1/SCK_TIME )MHz 
        // SHT10 的频率取值范围是 0~10MHz（0~1MHz）对应电压 VDD>4.5V(VDD<4.5)



/* 自定义变量 ---------------------------------------------------------------- */

enum {TEMP,HUMI};

#endif

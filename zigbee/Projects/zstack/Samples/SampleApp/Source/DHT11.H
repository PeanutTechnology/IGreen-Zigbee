#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_us(void); //1 us  延时
extern void Delay_10us(void); //10 us  延时
extern void Delay_ms(unsigned int xms);	//延时函数
extern void COM(void);         // 温湿写入
extern void DHT11(void);   //温湿传感启动

extern uchar temp[2]; 
extern uchar temp1[5];
extern uchar humidity[2];
extern uchar humidity1[9];
extern uchar humi_decade;  //湿度十位
extern uchar humi_unit;    //湿度个位
extern uchar temp_decade;  //温度十位
extern uchar temp_unit;//温度个位

#endif
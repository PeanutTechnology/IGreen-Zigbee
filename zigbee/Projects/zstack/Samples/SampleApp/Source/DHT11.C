#include <ioCC2530.h>
#include "OnBoard.h"

/**
*  @CopyRight(版权): 华韵科技有限公司 2016~2026
*  @作者： 主要负责人：姜陈俊 辅助：王辰浩
*  @文件名： DHT11.C
*  @日期： 2016/4/6
*  @描述： 温湿度传感器DHT11驱动代码
*  @主要函数列表：
*       1.
*       2.
*  @注意点：
*       1. delay函数内部需要使用协议栈自带的延时函数
*       2. 要包含“OnBoard.h”
*/

#define uint unsigned int
#define uchar unsigned char

#define temp_humi P0_6

/*******函数声明*********/
void Delay_us(void); //1 us延时
void Delay_10us(void); //10 us延时
void Delay_ms(uint Time);//n ms延时
void COM(void);	// 温湿写入
void DHT11(void) ;  //温湿传感启动


//温湿度定义
uchar ucharFLAG,uchartemp;
uchar humi_decade;  //湿度十位
uchar humi_unit;    //湿度个位
uchar temp_decade;  //温度十位
uchar temp_unit = 4;//温度个位
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;

uchar temp[2]={0,0}; 
uchar temp1[5]="temp=";
uchar humidity[2]={0,0};
uchar humidity1[9]="humidity=";

/****************************
        延时函数
*****************************/
void Delay_us() //1 us延时
{
   MicroWait(1);   
}

void Delay_10us() //10 us延时
{
  MicroWait(10);
}

void Delay_ms(uint Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}


/***********************
   温湿度传感
***********************/
void COM(void)	// 温湿写入
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
     ucharFLAG=2; 
     while((!temp_humi)&&ucharFLAG++);
     Delay_10us();
     Delay_10us();
     Delay_10us();
     uchartemp=0;
     if(temp_humi)uchartemp=1;
     ucharFLAG=2;
     while((temp_humi)&&ucharFLAG++);   
     if(ucharFLAG==1)break;    
     ucharcomdata<<=1;
     ucharcomdata|=uchartemp; 
     }    
}

void DHT11(void)   //温湿传感启动
{
    temp_humi=0;
    Delay_ms(19);  //>18MS
    temp_humi=1; 
    P0DIR &= ~0x40; //重新配置IO口方向
    Delay_10us();
    Delay_10us();						
    Delay_10us();
    Delay_10us();  
     if(!temp_humi) 
     {
      ucharFLAG=2; 
      while((!temp_humi)&&ucharFLAG++);
      ucharFLAG=2;
      while((temp_humi)&&ucharFLAG++); 
      COM();
      ucharRH_data_H_temp=ucharcomdata;
      COM();
      ucharRH_data_L_temp=ucharcomdata;
      COM();
      ucharT_data_H_temp=ucharcomdata;
      COM();
      ucharT_data_L_temp=ucharcomdata;
      COM();
      ucharcheckdata_temp=ucharcomdata;
      temp_humi=1; 
      uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
       if(uchartemp==ucharcheckdata_temp)
      {
          ucharRH_data_H=ucharRH_data_H_temp;
          ucharRH_data_L=ucharRH_data_L_temp;
          ucharT_data_H=ucharT_data_H_temp;
          ucharT_data_L=ucharT_data_L_temp;
          ucharcheckdata=ucharcheckdata_temp;
       }
         temp_decade=ucharT_data_H/10; 
         temp_unit=ucharT_data_H%10;
	 
         humi_decade =ucharRH_data_H/10; 
         humi_unit =ucharRH_data_H%10;        
    } 
    else //没用成功读取，返回0
    {
         temp_decade=0; 
         temp_unit=0;
	 
         humi_decade=0; 
         humi_unit=0;  
    } 
   P0DIR |= 0x40; //IO口需要重新配置 
}

#include <stdio.h>
#include <string.h>
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"
#include "SampleApp.h"
#include "SampleAppHw.h"
#include "OnBoard.h"
/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "ds18b20.h" //温度传感器头文件
#include  "MT_UART.h" //此处用于串口
//#include "DHT11.H" //温湿度传感器头文件      
#include "instructions.h" //存放Igreen通信协议

/**
 *  @CopyRight(版权): 华韵科技有限公司 2016~2026
 *  @作者： 主要负责人：姜陈俊 辅助：王辰浩
 *  @日期： 2016/4/5
 *  @描述： 本文件是应用的主要文件
 *  @主要函数列表： 
 *     1. void SampleApp_Init( uint8 task_id )
 *                                                   //初始化串口
 *     2. void SampleApp_SendPeriodicMessage(void)   
 *                                                   //子节点周期性发送数据给主节点
 *     3. void SampleApp_MessageMSGCB(afIncomingMSGPacket_t pkt)
 *                                                    //主节点将接收的数据通过串口给终端
 */


#define LED P1_0
//#define HUMI_PIN P2_0 //定义土壤湿度的检测引脚
void sendmsg_temp_humi(void);  //发送温度和湿度
void sendmsg_temp(void);   //18b20温度

void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );

// 该列表需要填放应用程序特定的Cluster IDs(暂译为簇ID)
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;


uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/**----------------------------------------------------------------*
 * @函数    SampleApp_SendPeriodicMessage
 * @简介   发送周期性信息(给主节点)
 * @参数    none
 * @返回值  none
 *----------------------------------------------------------------*/
void SampleApp_SendPeriodicMessage( void )
{
 // sendmsg_temp_humi(); //发送温度和湿度
    sendmsg_temp();

}

/**----------------------------------------------------------------*
 * @函数    void sendmsg_temp_humi(void)
 * @简介    发送温度和湿度
 * @参数    none
 * @返回值  none
 *----------------------------------------------------------------*/
void sendmsg_temp(void)
{
     uint8 buffer[32];
  /*----------------------------------- 
   *  温度湿度检测将其放入缓冲区
   *----------------------------------*/
  Temp_test();    //温度检测
  buffer[0] = RES_TEMP;   //温度 
  buffer[1] = 0; buffer[2] = 0; buffer[3] = 0; buffer[4] = 1; //4byte=32bit 唯一ID
  buffer[5] = CMD_SEP; 
  buffer[6] = temp/10 + '0';
  buffer[7] = temp%10 + '0';
  buffer[8]= CMD_END; 
  //将温度发送给节点
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID, //和主节点中SampleApp_MessageMSGCB的SAMPLEAPP_PERIODIC_CLUSTERID一致。
                       9,//字节数
                       buffer,//存放温度缓冲区
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
      HalUARTWrite(0, "temp success\n", strlen("temp success\n")); //显示温度发送成功
  } 
  else{// Error occurred in request to send.
  }
}
/**----------------------------------------------------------------*
 * @函数    void sendmsg_temp_humi(void)
 * @简介    发送温度和湿度
 * @参数    none
 * @返回值  none
 *----------------------------------------------------------------*/
//void sendmsg_temp_humi(void)
//{
//  uint8 buffer[32];
//  /*----------------------------------- 
//   *  温度湿度检测将其放入缓冲区
//   *----------------------------------*/
//  DHT11();    //温度检测
//  buffer[0] = RES_TEMP;   //温度 
//  buffer[1] = 0; buffer[2] = 0; buffer[3] = 0; buffer[4] = 1; //4byte=32bit 唯一ID
//  buffer[5] = CMD_SEP; 
//  buffer[6] = temp_decade + '0';
//  buffer[7] = temp_unit   + '0';
//  buffer[8]= CMD_END; 
//  //将温度发送给节点
//  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
//                       SAMPLEAPP_PERIODIC_CLUSTERID, //和主节点中SampleApp_MessageMSGCB的SAMPLEAPP_PERIODIC_CLUSTERID一致。
//                       9,//字节数
//                       buffer,//存放温度缓冲区
//                       &SampleApp_TransID,
//                       AF_DISCV_ROUTE,
//                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
//  {
//      HalUARTWrite(0, "temp success\n", strlen("temp success\n")); //显示温度发送成功
//  }
//  else{// Error occurred in request to send.
//  }
//
//  buffer[0] = RES_HUMI;   //湿度 
//  buffer[1] = 0; buffer[2] = 0; buffer[3] = 0; buffer[4] = 1;//4byte=32bit 唯一ID(1)
//  buffer[5] = CMD_SEP; 
//  buffer[6] = humi_decade + '0';
//  buffer[7] = humi_unit   + '0';
//  buffer[8] = CMD_END; 
//  //将湿度发送给节点
//  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,SAMPLEAPP_PERIODIC_CLUSTERID, 9,buffer,&SampleApp_TransID,AF_DISCV_ROUTE,AF_DEFAULT_RADIUS ) 
//                             == afStatus_SUCCESS )
//  {
//      HalUARTWrite(0, "humi success\n", strlen("humi success\n")); //显示温度发送成功
//  }
//  else{// Error occurred in request to send.
//  }
//}

/**-----------------------------------------------------------------------------
 * @fn      SampleApp_MessageMSGCB
 * @brief   数据信息处理器的回调函数。该函数处理任何进来的数据（可能来自与其他设备）
 *          因此，依赖于cluster ID(簇ID)，进行相应的操作
 * @param   none
 * @return  none
 *---------------------------------------------------------------------------*/
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  int i;
  uint16 flashTime;
  uint8 buffer[64];

  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_PERIODIC_CLUSTERID: //接到SAMPLEAPP_PERIODIC_CLUSTERID的数据
      {
          for (i = 0; i < 32; i++) {
              if (pkt->cmd.Data[i] == CMD_END)  //找到一条指令，则将该指令(0~i+1之间的字符串)发送出去
              {
                  HalUARTWrite(0, &pkt->cmd.Data[0], i+1); //打印收到数据(数量为end-start+1)
                  break;
              }
          }
      }
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
      break;
  }
}

/**-----------------------------------------------------------------------------------------------
 * @fn      SampleApp_Init
 * @brief   通用APP任务的初始化函数。该函数在初始化程序和任何需要包含应用相关的初始化程序期间被调用。
 *         （例如：硬件初始化程序，table初始化，power up（上电）通知等）
 * @param   task_id - OSAL分配的ID。该ID需要被用于发送信息和设置定时器。
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  MT_UartInit();//串口初始化
  MT_UartRegisterTaskID(task_id);//登记任务号
  HalUARTWrite(0,"Hello IGreen\n",12); //（串口0，'字符'，字符个数。）
  
  /*------------------------------------------ 
   *        引脚初始化
   *------------------------------------------*/
//  P1DIR |= 0x010 ; //P1_0  定义为输出   湿度传感器
//  P1INP |= 0X01;   // 打开下拉
  
//  P2SEL &= ~0X01;  // 设置 0 P20  为普通 O IO  口
//  P2DIR &= ~0X01;  // 在 在 0 P20  口，设置为输入模式
//  P2INP &= ~0x01;  // 打开 0 P20  上拉电阻, , 不影响
  
  P0SEL &= 0xbf;   //DS18B20/DHT11 IO口初始化
  
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;//数据包发送到网络上所有设备

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;

  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
   //   case CMD_SERIAL_MSG:   //串口收到数据后由MT_UART层传递过来的数据
          
    //      SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);
      //    break;
        // Received when a key is pressed
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (SampleApp_NwkState == DEV_ZB_COORD)
              || (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
    SampleApp_SendPeriodicMessage();//周期性发送函数

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
void SampleApp_SendFlashMessage( uint16 flashTime )
{
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}
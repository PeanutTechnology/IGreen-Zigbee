#ifndef _H_INSTRUCTIONS
#define _H_INSTRUCTIONS

#define ACCOUNT_MAX 32

/*指令头(head),指令尾(end)，指令分隔符(seperator)*/
#define CMD_HEAD   'H'
#define CMD_END    'E'
#define CMD_SEP    '/'

/*大类指令：1.心跳指令 2.管理类 3.控制类 4.结果类*/
#define CMD_HEART  '0'
#define CMD_MAN    'M'
#define CMD_CTRL   'C'
#define CMD_RES    'R'

/*心跳类：1.心跳指令*/
#define HEART_BEAT    'B'

/*管理类：1.登录指令*/
#define MAN_LOGIN     'L'

/*控制类：1.灯具*/
#define CTRL_LAMP     'A'

/*结果类-：1.登录结果 2.温度 3.湿度 4.温湿度 5.终端离线 6.设备离线*/
#define RES_LOGIN     'L'
#define RES_TEMP      'P'
#define RES_HUMI      'U'
#define RES_TOFFL     'O'
#define RES_DOFFL     'D'

/*登陆成功/失败*/
#define LOGIN_SUCCESS '1'
#define LOGIN_FAILED  '0'

/*灯开关*/
#define LAMP_ON       '1'
#define LAMP_OFF      '0'

#endif

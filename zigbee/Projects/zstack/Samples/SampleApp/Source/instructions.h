#ifndef _H_INSTRUCTIONS
#define _H_INSTRUCTIONS

#define ACCOUNT_MAX 32

/*ָ��ͷ(head),ָ��β(end)��ָ��ָ���(seperator)*/
#define CMD_HEAD   'H'
#define CMD_END    'E'
#define CMD_SEP    '/'

/*����ָ�1.����ָ�� 2.������ 3.������ 4.�����*/
#define CMD_HEART  '0'
#define CMD_MAN    'M'
#define CMD_CTRL   'C'
#define CMD_RES    'R'

/*�����ࣺ1.����ָ��*/
#define HEART_BEAT    'B'

/*�����ࣺ1.��¼ָ��*/
#define MAN_LOGIN     'L'

/*�����ࣺ1.�ƾ�*/
#define CTRL_LAMP     'A'

/*�����-��1.��¼��� 2.�¶� 3.ʪ�� 4.��ʪ�� 5.�ն����� 6.�豸����*/
#define RES_LOGIN     'L'
#define RES_TEMP      'P'
#define RES_HUMI      'U'
#define RES_TOFFL     'O'
#define RES_DOFFL     'D'

/*��½�ɹ�/ʧ��*/
#define LOGIN_SUCCESS '1'
#define LOGIN_FAILED  '0'

/*�ƿ���*/
#define LAMP_ON       '1'
#define LAMP_OFF      '0'

#endif

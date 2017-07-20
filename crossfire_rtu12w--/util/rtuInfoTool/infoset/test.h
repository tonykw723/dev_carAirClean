
typedef char xt_s8;
typedef unsigned char xt_u8;
typedef short xt_s16;
typedef unsigned short xt_u16;
typedef int xt_s32;
typedef unsigned int xt_u32;
typedef long long xt_s64;
typedef char * xt_string;
typedef unsigned char xt_bool;
typedef void xt_void;
typedef float xt_float;
typedef double xt_double;
typedef xt_u32 xt_time_t;

#define  ODD 1;
#define EVEN 2;
#define MARK 3;
#define SPACE 4;

#define START_FLAG 0x68
#define TAIL_FLAG 0x16
#define num_ADDR  6
#define READ 0x03
#define INSETCMD 0X19


typedef enum
{
	NONE_LAMP,SODIUM_LAMP=01,LED_LAMP=04,SOLAR_LAMP=10,
} LAMP_enum_t;

typedef enum
{
	PWM_A=1,PWM_N=2
} PWM_enum_t;

typedef enum
{
	NO=0,	YES, ERR,  OTHER
} comfirm_enum_t;

typedef struct
{
	xt_s8 COM[256];
	xt_u8 Stopbits;
	xt_u8 Parity;
	xt_u8 BaudRate;
}port;

//-------LAMP ��Ŀ���ֵ----------
#define  LAMP_NUM_MAX   2

#define  LAMP1_INDEX  0
#define  LAMP2_INDEX  1
#define  LAMP3_INDEX  2     
#define  LAMP4_INDEX  3

typedef struct 
{
	xt_u16 have_writed;
	//---------��Ŀ�Ͱ汾-----------
	xt_u8 project_name[6];//��Ŀ��
	xt_u8 rtu_sn[4];            //ϵ�к�
	xt_u8 hardware_vertion[4];//Ӳ���汾
	xt_u8 software_vertion[4];//����汾
	//--------��������--------------
	LAMP_enum_t lamp_type[LAMP_NUM_MAX];//��ͷ�ƾ�����
	xt_u8 lamp_num;//��ͷ��
	PWM_enum_t pwm_type;//PWM���ͣ�����/����
	xt_u8 pwm_num; //�����߸���
	xt_u16 pwm_frequence;//PWMƵ��
	comfirm_enum_t pwm_decay_opened;//��˥����
	xt_u8 pwm_decay_value;//��˥ֵ
	//-------PLC��ز���-----------
	xt_u8 sever_uid[6];               //zigbee���鲥�͹㲥���õ�
	xt_u8 rtuOrPlc_uid_own[6];  //uid��ַ
	xt_u8 rtuOrPlc_domain[2];   //���
	xt_u8 rtuOrPlc_group[3];     //���
	xt_u8 rn_LBAaddr[2];            //��������ʱ�����õ�
	xt_u8 flag_have_joined;       //��ʾ�ϴ���������־
	//xt_u8 dev_module_id;

	//--------���Ӷ������-------
	xt_u8      lamp_swap;
	xt_u16    polling_sloar_period;
	xt_u16    storeQueue_inNum;
	xt_u16    storeQueue_OutNum;
} rtu_cfg_para_t;



typedef  struct 
{
	unsigned char	cmd;
	unsigned char   project_name[6];
	unsigned char   rtu_sn[4];
	unsigned char   hardware_vertion[4];
	unsigned char   software_version[4];
	unsigned char      lamp_type[LAMP_NUM_MAX];
	unsigned char      lamp_num;
	unsigned char      rtu_addr[6];
	unsigned char      rtu_group[3];
	unsigned char      rtu_domain[2];
	unsigned char      pwm_type;
	unsigned short    pwm_frequence;
	unsigned char      pwm_decay_opened;
	unsigned char      pwm_decay_value;
	unsigned char      lamp_swap;
}PACK_SET_2 ;

extern PACK_SET_2 set_pack;
extern port portset;
extern bool RtuInfoGet(_TCHAR *p);
extern bool	RtuInfoSet();
extern rtu_cfg_para_t rtu_cfg_para_get;
extern bool TwoASCtoHEX(unsigned char* psetbuf,unsigned char *pgetbuf,int length);
extern bool openport(_TCHAR *portname);
extern void CutZero(unsigned char* psrc,unsigned char* ptargt,int length);
extern void HEX2Asc(unsigned char* psetbuf,unsigned char *pgetbuf,int length);

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

//-------LAMP 数目最大值----------
#define  LAMP_NUM_MAX   2

#define  LAMP1_INDEX  0
#define  LAMP2_INDEX  1
#define  LAMP3_INDEX  2     
#define  LAMP4_INDEX  3

typedef struct 
{
	xt_u16 have_writed;
	//---------项目和版本-----------
	xt_u8 project_name[6];//项目名
	xt_u8 rtu_sn[4];            //系列号
	xt_u8 hardware_vertion[4];//硬件版本
	xt_u8 software_vertion[4];//软件版本
	//--------基本参数--------------
	LAMP_enum_t lamp_type[LAMP_NUM_MAX];//灯头灯具类型
	xt_u8 lamp_num;//灯头数
	PWM_enum_t pwm_type;//PWM类型，正向/反向
	xt_u8 pwm_num; //调光线个数
	xt_u16 pwm_frequence;//PWM频率
	comfirm_enum_t pwm_decay_opened;//光衰开关
	xt_u8 pwm_decay_value;//光衰值
	//-------PLC相关参数-----------
	xt_u8 sever_uid[6];               //zigbee，组播和广播有用到
	xt_u8 rtuOrPlc_uid_own[6];  //uid地址
	xt_u8 rtuOrPlc_domain[2];   //域号
	xt_u8 rtuOrPlc_group[3];     //组号
	xt_u8 rn_LBAaddr[2];            //瑞萨组网时，有用到
	xt_u8 flag_have_joined;       //表示上次组网过标志
	//xt_u8 dev_module_id;

	//--------增加额外参数-------
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
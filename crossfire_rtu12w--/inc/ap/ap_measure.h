/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __AP_MEASURE_H_
#define __AP_MEASURE_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//----------校准系数--------------
#define  V_CK        	1		   //电压校准系数
//#define  I_CK[4]        0.9990
//----------
#ifdef REFERENCE_5V
#define  INTO_CURRENT(x) (0.005086*(x))   //(*5/4096)*1000/20/12---rtu09
#define  INTO_VOLTAGE(x) (0.366945*(x))	  //(*5/4096)*150000/10/49.9*(x)---rtu09 
#endif
#ifdef REFERENCE_4_096V
#define  INTO_CURRENT(x) (0.004167*(x))   //(*4.096/4096)*1000/20/12---rtu06c
#define  INTO_VOLTAGE(x) (0.300601*(x))	  //(*4.096/4096)*150000/10/49.9*(x)---RTU06C
#endif
#ifdef REFERENCE_3V3
#define  INTO_CURRENT(x) (0.003357*(x))   //(*3.3/4096)*1000/20/12---rtu09
#define  INTO_VOLTAGE(x) (0.242183*(x))	  //(*3.3/4096)*150000/10/49.9*(x)---rtu09 
#endif

#ifdef MEASURE_DC
#define  INTO_DC_CURRENT(x)   (0.001299*(x))   //(*3.3/4096)/31/0.02(x)
#define  INTO_DC_VOLTAGE(x) (0.010474*(x))	  //(*3.3/4096)*13(x)
#endif

#define   AVR_DIVISOR  20

#define	 ABS(a)		(((a) > 0) ? (a) : (a) * -1)
#define	 SQUARE(a)	((a) * (a))
#define	 SQRT(a)		(sqrt(a))

typedef struct
{
	//store runtime config pointer for ap_measure
	ap_t	instance;
} ap_measure_priv_data_t;

typedef enum
{
	//ap_measure command for measure process
	AP_MEASURE_CMD_TEST = 0x300,

	AP_MEASURE_CMD_END,
} ap_measure_cmd_n;

//get runtime config pointer for ap_measure
ap_t* ApMeasureGetInstance(void);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __AP_MEASURE_H_


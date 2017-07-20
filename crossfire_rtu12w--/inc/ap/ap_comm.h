/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __AP_COMM_H_
#define __AP_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct
{
	//store runtime config pointer for ap_comm
	ap_t	instance;
} ap_comm_priv_data_t;

typedef enum
{
	//ap_comm command for send out data 
	AP_COMM_CMD_SEND = 0x100,

	AP_COMM_CMD_END,
} ap_comm_cmd_n;

//get runtime config pointer for ap_comm
ap_t* ApCommGetInstance(void);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __AP_COMM_H_


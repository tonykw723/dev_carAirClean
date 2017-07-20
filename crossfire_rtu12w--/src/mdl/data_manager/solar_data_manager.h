/**********************************************************************/
/* Copyright (c) 2012 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __SOLAR_DATA_MANAGER_H_
#define __SOLAR_DATA_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus
#define MAX_SOLAR_DATA_STORE_SIZE 5

#define SDM_EEPROM_START_ADDR 4096
#define SDM_EACH_PAKAGE_SIZE    128
#define SDM_MAX_PAKAGES_SIZE    480//(4*24*5)
#define SDM_MAX_BYTES_SIZE         61440//(4*24*5*128)

typedef struct
{
	xt_u16		         qin;
	xt_u16		         qout;
	xt_u8 *                   buffaddr;
	xt_u16		         buffsize;	
}PACKED solar_data_store_queue;


void SDM_test(void);
xt_u32 SloarDataStoreWrtie(xt_u8 *data,xt_u8 len);
xt_u8*  SloarDataStoreRead(xt_u8 len);
xt_void SloarDataStoreDel(void);
xt_void SloarDataStoreReset(void);



#endif // End for __MDL_DM_H_
 


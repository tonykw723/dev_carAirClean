/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_timer.h"

#include "xt_bsp_rtu09c.h"

#include "config.h"
#include "global_para.h"

extern int TestMain(int argc, char* argv[]);
rtu_para_t glbRtuPara;
int main(int argc, char* argv[])
{
	//XtBspWdgInit();
	
	XtBspInit();
  
	//TestMain(argc,argv);
	ApConfig();

	return 0;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus

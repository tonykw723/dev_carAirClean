#ifndef DO_TASK_H
#define DO_TASK_H

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus
typedef struct 
{
	xt_u8      whichDay;
	xt_u8      hour;
	xt_u8      minute;
	xt_u16    howmanyMinutes;
	xt_u8      action;

}PACKED task_type;


void do_sensorTask(void);

#ifdef __cplusplus
}
#endif // End for __cplusplus
void do_timeSensorTask(void);

#endif // End 

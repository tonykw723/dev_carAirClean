#ifndef ODB_CTRL_H
#define ODB_CTRL_H

xt_s32 odb_readDatas(xt_u8* pData,xt_s32* pDataLen);
xt_s32 odb_chkDatas(xt_u8* pData,xt_s32* pDataLen);
void odb_doTask(xt_u8* pData,xt_s32* pDataLen);
#endif



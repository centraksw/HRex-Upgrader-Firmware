#ifndef _DFU_H
#define _DFU_H

void DFU_Task(void);
void Flash_Erase_Part();
void Flash_Write_INTVEC(BYTE* intvec_data, WORD len);
void Flash_Write_Segment(BYTE seg_no, BYTE* seg_data, WORD len);
void Read_INTVEC();
#endif //_DFU_H

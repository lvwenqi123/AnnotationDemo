/*******************************/
/******  Fb For iBoard     *****/
/*******************************/
/****** Version : v1.0       ***/
/******    Data : 2021.11.24 ***/
/******  Author : yance      ***/
/*******************************/

#ifndef LibFb_kjd_311d2_H
#define LibFb_kjd_311d2_H

#ifdef __cplusplus
extern "C" {
#endif
int kjd311d2_fbdraw_init(int width,int height);
void* kjd311d2_fbdraw_addr();
void kjd311d2_fbRefresh();
void kjd311d2_fbEnable(int flag);
int  kjd311d2_fbdraw_release();
#ifdef __cplusplus
}
#endif
#endif

/*******************************/
/******  Fb For iBoard     *****/
/*******************************/
/****** Version : v1.0       ***/
/******    Data : 2021.11.24 ***/
/******  Author : yance      ***/
/*******************************/

#ifndef LIBFB_H
#define LIBFB_H

/* Init the fb buffer,layer default is 0,width*height default is 1920*1080 */
/* return: 0:Success,Others:Failed       */
int   fbdraw_init(int layer,int width,int height);

/* get the fb addr,layer default is 0 */
/* return: !null:Succcess , null:Failed */
void* fbdraw_addr(int layer);

/* release the fb addr,layer default is 0 */
/* return: 0:Success , others:Failed */
int   fbdraw_release(int layer);

#endif // LIBFB_H

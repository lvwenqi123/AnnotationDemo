#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <hifb.h>
#include <sys/ioctl.h>
#include <cutils/properties.h>
#include <string.h>
#include <malloc.h>

#define DEFAULT_FB_PROPERTY "ro.hisi.hwcursor.fb"

/*****************************************************************************/
static int fd;
static struct fb_fix_screeninfo gst_finfo;
static struct fb_var_screeninfo gst_vinfo;

static HIFB_SIZE_S g_ScreenSize = {0, 0};
static HIFB_SIZE_S ScreenSize = {0, 0};
static HI_U32 u32DisplayWidth = 0, u32DisplayHeight = 0;

/*****************************************************************************/

int main()
{
	unsigned char *fbp = NULL, *ptr;
	int x, y, x0, y0;
	int i,j;
    int fd0 = open("/dev/graphics/fb0", O_RDWR, 0);
    if (fd0 < 0) {
         printf("open /dev/graphics/fb1 failed! fd:%d\n", fd0);
         return HI_FALSE;
    }

    if (ioctl(fd0, FBIOGET_SCREENSIZE, &ScreenSize) < 0) {
        printf("get FBIOGET_SCREENSIZE failed!\n");
    }
	else
	{
		printf("get FBIOGET_SCREENSIZE u32Width: %d ,u32Height: %d \n",ScreenSize.u32Width,ScreenSize.u32Height);
	}

 //   if (fd0 >=0 ){
 //       int ret = close(fd0);
 //       printf("close fb! ret:%d\n", ret);
 //   }

   // char fb_dev[PROPERTY_VALUE_MAX];
   // int ret = property_get(DEFAULT_FB_PROPERTY, fb_dev, "/dev/graphics/fb2");

   // if (ret <= 0 || strlen(fb_dev) <= 0) {
   //     printf("get hwcursor fb failed! ret:%d", ret);
   //     return HI_FALSE;
   // }

    /* open Framebuffer device fb3 overlay */

   // fd = open("/dev/graphics/fb1", O_RDWR, 0);
   // printf("open %s ing! fd:%d ,ret:%d", fb_dev, fd, ret);
//	 printf("opening! fd:%d \n",fd);

  //  if (fd < 0) {
       // printf("open %s  failed! fd:%d ,ret:%d", fb_dev, fd, ret);
	//    printf("fail open! fd:%d ",fd);
  //     return HI_FALSE;
   // }

    memset(&gst_vinfo,0,sizeof(gst_vinfo));

    if (ioctl(fd0, FBIOGET_VSCREENINFO, &gst_vinfo) < 0) {
        printf("get v screenInfo failed!\n");
        return HI_FALSE;
    }
	else {
		printf("FBIOGET_VSCREENINFO xres:%d\n",gst_vinfo.xres);
		printf("FBIOGET_VSCREENINFO xres_virtual:%d\n",gst_vinfo.xres_virtual);
		printf("FBIOGET_VSCREENINFO yres:%d\n",gst_vinfo.yres);
		printf("FBIOGET_VSCREENINFO yres_virtual:%d\n",gst_vinfo.yres_virtual);
		printf("FBIOGET_VSCREENINFO xres:%d\n",gst_vinfo.xres);
		printf("FBIOGET_VSCREENINFO bits_per_pixel:%d\n",gst_vinfo.bits_per_pixel);
		printf("FBIOGET_VSCREENINFO activate:%d\n",gst_vinfo.activate);
		printf("FBIOGET_VSCREENINFO height:%d\n",gst_vinfo.height);
		printf("FBIOGET_VSCREENINFO width:%d\n",gst_vinfo.width);
		printf("FBIOGET_VSCREENINFO 1:%d\n",gst_vinfo.bits_per_pixel);
		printf("FBIOGET_VSCREENINFO 2:%d\n",gst_vinfo.red.offset);
		printf("FBIOGET_VSCREENINFO 3:%d\n",gst_vinfo.red.length);
		printf("FBIOGET_VSCREENINFO 4:%d\n",gst_vinfo.green.offset);
		printf("FBIOGET_VSCREENINFO 5:%d\n",gst_vinfo.green.length);		
		printf("FBIOGET_VSCREENINFO 6:%d\n",gst_vinfo.blue.offset);
		printf("FBIOGET_VSCREENINFO 7:%d\n",gst_vinfo.blue.length);		
		printf("FBIOGET_VSCREENINFO 8:%d\n",gst_vinfo.transp.offset);
		printf("FBIOGET_VSCREENINFO 9:%d\n",gst_vinfo.transp.length);
	}
    printf("opening! fd:%d \n",fd);
	
    /*gst_vinfo.reserved[0] = 0;
    gst_vinfo.reserved[1] = 0;
    gst_vinfo.reserved[2] = 0;
    gst_vinfo.xoffset = 0;
    gst_vinfo.yoffset = 0;
    gst_vinfo.activate = FB_ACTIVATE_FORCE;

    gst_vinfo.bits_per_pixel = 32;
    gst_vinfo.red.offset     = 0;
    gst_vinfo.red.length     = 8;
    gst_vinfo.green.offset   = 8;
    gst_vinfo.green.length   = 8;
    gst_vinfo.blue.offset    = 16;
    gst_vinfo.blue.length    = 8;
    gst_vinfo.transp.offset  = 24;
    gst_vinfo.transp.length  = 8;


    gst_vinfo.xres = ScreenSize.u32Width;
    gst_vinfo.xres_virtual = ScreenSize.u32Width;
    gst_vinfo.yres = ScreenSize.u32Height;
    gst_vinfo.yres_virtual = ScreenSize.u32Height;*/

    //ALOGD("widtd:%d, height:%d", iconWidth, iconHeight);

    if (ioctl(fd0, FBIOPUT_VSCREENINFO, &gst_vinfo) < 0) {
        printf("set variable screeninfo failed!\n");
        return HI_FALSE;
    }

    if (ioctl(fd0, FBIOGET_FSCREENINFO, &gst_finfo) < 0) {
        printf("Get fix screenInfo failed!\n");
        return HI_FALSE;
    }
	else
	{
		printf("FBIOGET_VSCREENINFO smem_len:%d\n",gst_finfo.smem_len);
		printf("FBIOGET_VSCREENINFO line_length:%d\n",gst_finfo.line_length);
	}
    printf("gst_finfo.smem_len : %d \n",gst_finfo.smem_len);
    fbp = mmap(NULL, gst_finfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd0, 0);
    if (fbp == NULL) {
        printf("hal Fb mmap failed!");
        return HI_FALSE;
    }else{
        printf("hal Fb mmap success!\n");
        //return HI_TRUE;
    }
	 x0 = 500;
	 y0 = 200;
	 ptr = fbp + y0 * gst_finfo.line_length + x0 * gst_vinfo.bits_per_pixel / 8;
	 
	 for(j=0;j<200;j++)
	 {
	 	//printf("j===%d\n",j);
		ptr = fbp + (y0+j) * gst_finfo.line_length + x0 * gst_vinfo.bits_per_pixel / 8;
		char* tmp_ptr = ptr;
		 for(i=0;i<200;i++)
		 {
		 	*tmp_ptr++ = 0;//B
			 *tmp_ptr++ = 255;  //G
			 *tmp_ptr++ = 0;    //R
			 *tmp_ptr++ = 255;    //A
			 //printf("i===%d\n",i);
		 }
		 //printf("add line");
		 
	 }
	 //*ptr=0x00ff0000;
	 /*for( i = 0; i < 100; i++){
		 char* tmp_ptr = ptr;
		 for(j = 0; j < 200; j++){
			 *tmp_ptr++ = 255;
			 *tmp_ptr++ = 255;  //
			 *tmp_ptr++ = 255;    //
			 *tmp_ptr++ = 0;    //
		}
		ptr += 800;
	 }*/
	 while(1);
	 munmap(fbp,gst_finfo.smem_len);
	 close(fd);
	 return 0;
}
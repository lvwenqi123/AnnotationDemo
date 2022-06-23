#include <SkCanvas.h>
#include <SkBitmap.h>
#include <memory>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/fb.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "SkiaLog.h"
#include "FbAbstract.h"
#include "common.h"

FbAbstract::FbAbstract()
{
    fd_ = -1;
    fbOn_ = false;
    fbAddr_ = nullptr;
    fbSize_ = 0;
    //buf_ = new ;
}

FbAbstract::~FbAbstract() {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        delete *it;
        ++it;
    }
    cl_.clear();
    bmp0_.setPixels(nullptr);
    bmp1_.setPixels(nullptr);
    bmp2_.setPixels(nullptr);
#if SYS_6 && FRAME_BUF
    if (fd_ >0) {
        fbdev_munmap(fbAddr_, fbSize_);
        fbdev_close(fd_);
    }
#elif FRAME_BUF
    if (fd_ >0) {
        munmap(fbAddr_, fbSize_);
        close(fd_);
    }
#endif
    fbAddr_ = nullptr;
    fbSize_ = 0;
    fd_ = -1;
    if (buf_) {
        delete []buf_;
        buf_ = nullptr;
    }
}

void FbAbstract::SetImageInfo(SkImageInfo &ii) {
    ii_ = ii;
//    if (!buf_) {
//        int bufSize = ii_.width() * ii_.height();
//        buf_ = new uint8_t[bufSize];
//        memset(buf_, 0, bufSize);
//    }
}

void FbAbstract::DrawPath(SkPath &path, SkPaint &paint) {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawPath(path, paint);
        ++it;
    }
}

void FbAbstract::DrawBitmapRectToRect(SkBitmap &bmp, SkRect &src, SkRect &dst) {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        CanvasAdapt::DrawBitmapRect((*it), bmp, src, dst);
        ++it;
    }
}

void FbAbstract::DrawBitmap(SkBitmap &bmp, float x, float y) {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawBitmap(bmp, x, y);
        ++it;
    }
}

void FbAbstract::DrawCircle(float x, float y, SkScalar radius, const SkPaint &paint) {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawCircle(x, y, radius, paint);
        ++it;
    }
}
#if SYS_6 && FRAME_BUF
bool Fb648::InitFb()
{
    fbOn_ = false;
    DBG("Will Open Fb");
    fd_ = fbdev_open("/dev/graphics/fb0", O_RDWR); //step 1: open and init fbdev
    if (fd_ < 0) {
        ERR("cannot open fb0 fd_ = %d", fd_);
        return false;
    }
    struct fb_fix_screeninfo finfo;
    if (fbdev_ioctl(fd_, FBIOGET_FSCREENINFO, &finfo) == -1) {
        ERR("get fb_fix_screeninfo fail ...");
        return false;
    }
    DBG("Get Fb Info..");
    struct fb_var_screeninfo fb_vinfo;
    if (fbdev_ioctl(fd_, FBIOGET_VSCREENINFO, &fb_vinfo) == -1) {
        ERR("get fb_var_screeninfo fail ...\n");
        return false;
    }

    DBG("fb_vinfo: %d, %d, %d, %d", fb_vinfo.xres, fb_vinfo.yres,
        fb_vinfo.xres_virtual, fb_vinfo.yres_virtual);
    DBG("finfo.smem_len=%ld\n", (long) finfo.smem_len);

    fbSize_ = round_up_to_page_size(finfo.smem_len);

    fbAddr_ = (uint8_t *) fbdev_mmap(0, fbSize_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_,
                                     0); //step 2: get framebuffer addr
    //fbAddr_ = (int8_t*)(0x0000007fffffffff & (long int)fbAddr_);
    DBG("fbAddr_ = %p fbSize=%ld\n", fbAddr_, fbSize_);
    if (fbAddr_) {
        /*fb操作相关位图初始化*/
        bmp0_.setInfo(ii_);
        bmp1_.setInfo(ii_);
        bmp2_.setInfo(ii_);

        bmp0_.setPixels(fbAddr_);
        bmp1_.setPixels(fbAddr_ + fbSize_);
        bmp2_.setPixels(fbAddr_ + fbSize_ * 2);

        c0_ = new SkCanvas(bmp0_);
        c1_ = new SkCanvas(bmp1_);
        c2_ = new SkCanvas(bmp2_);
        cl_.push_back(c0_);
        cl_.push_back(c1_);
        cl_.push_back(c2_);
        fbOn_ = true;
    }
    return fbOn_;
}
#endif


bool Fb811::InitFb()
{
    fbOn_ = false;
    DBG("Will Open Fb");
    fd_ = open("/dev/graphics/fb1", O_RDWR, 0); //step 1: open and init fbdev
    if (fd_ < 0) {
        ERR("cannot open fb0 fd_ = %d", fd_);
        return false;
    }

    DBG("Get Fb Info..");
    HIFB_SIZE_S ScreenSize = {0, 0};
    if (ioctl(fd_, FBIOGET_SCREENSIZE, &ScreenSize) < 0) {
        ERR("get FBIOGET_SCREENSIZE failed!");
        return false;
    }
    ERR("==== w = %d h = %d", ScreenSize.u32Width, ScreenSize.u32Height);
    struct fb_var_screeninfo gst_vinfo;
    memset(&gst_vinfo, 0, sizeof(gst_vinfo));
    if (ioctl(fd_, FBIOGET_VSCREENINFO, &gst_vinfo) < 0) {
        ERR("get v screenInfo failed!");
        return false;
    }
    ERR("gst_vinfo.bits_per_pixel = %d r = %d g = %d b = %d a = %d",
        gst_vinfo.bits_per_pixel,
        gst_vinfo.red.offset,
            //gst_vinfo.red.length,
        gst_vinfo.green.offset,
            //gst_vinfo.green.length,
        gst_vinfo.blue.offset,
            //gst_vinfo.blue.length,
        gst_vinfo.transp.offset);
    //gst_vinfo.transp.length);
    ERR("gst_vinfo.activate = %d", gst_vinfo.activate);
    gst_vinfo.reserved[0] = 0;
    gst_vinfo.reserved[1] = 0;
    gst_vinfo.reserved[2] = 0;
    gst_vinfo.xoffset = 0;
    gst_vinfo.yoffset = 0;
    gst_vinfo.activate = FB_ACTIVATE_FORCE;

    gst_vinfo.bits_per_pixel = 32;

    gst_vinfo.red.offset = 0;
    gst_vinfo.red.length = 8;
    gst_vinfo.green.offset = 8;
    gst_vinfo.green.length = 8;
    gst_vinfo.blue.offset = 16;
    gst_vinfo.blue.length = 8;
    gst_vinfo.transp.offset = 24;
    gst_vinfo.transp.length = 8;


    gst_vinfo.xres = ScreenSize.u32Width;
    gst_vinfo.xres_virtual = ScreenSize.u32Width;
    gst_vinfo.yres = ScreenSize.u32Height;
    gst_vinfo.yres_virtual = ScreenSize.u32Height;
    if (ioctl(fd_, FBIOPUT_VSCREENINFO, &gst_vinfo) < 0) {
        ERR("set variable screeninfo failed!");
        return false;
    }
    DBG("fb_vinfo: %d, %d, %d, %d", gst_vinfo.xres, gst_vinfo.yres,
        gst_vinfo.xres_virtual, gst_vinfo.yres_virtual);

    struct fb_fix_screeninfo gst_finfo;
    if (ioctl(fd_, FBIOGET_FSCREENINFO, &gst_finfo) == -1) {
        ERR("get fb_fix_screeninfo fail ...");
        return false;
    }

    DBG("finfo.smem_len=%ld", (long) gst_finfo.smem_len);
    fbAddr_ = (uint8_t *) mmap(NULL, gst_finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                               fd_, 0); //step 2: get framebuffer addr
    fbSize_ = gst_finfo.smem_len;
    DBG("fbAddr_ = %p fbSize=%ld", fbAddr_, (long) fbSize_);
    if (fbAddr_) {
        /*fb操作相关位图初始化*/
        ii_ = ii_.makeWH(ScreenSize.u32Width, ScreenSize.u32Height);
        bmp0_.setInfo(ii_);
        bmp0_.setPixels(fbAddr_);
        c0_ = new SkCanvas(bmp0_);
        cl_.push_back(c0_);
        fbOn_ = true;
    }
    return fbOn_;
}

void Fb811::ClearFb() {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawColor(SK_ColorTRANSPARENT, SkBlendMode::kClear);
        ++it;
    }
}

void Fb811::ClearFbByRect(SkRect &bound)
{
    int w = bound.width();
    int h = bound.height();
    SkImageInfo ii = SkImageInfo::Make(w, h, kRGBA_8888_SkColorType, kOpaque_SkAlphaType);
    int offset = bound.fLeft * 4;
    SkBitmap bmp;
    bmp.setInfo(ii, bmp0_.rowBytes());
    bmp.setPixels((char*)bmp0_.getPixels() + offset);
    SkCanvas c(bmp);
    c.drawColor(SK_ColorTRANSPARENT, SkBlendMode::kClear);
}



bool Fb560::InitFb() {
    DBG("Will Open Fb");
    fd_ = open("/dev/graphics/fb0", O_RDWR, 0); //step 1: open and init fbdev
    if (fd_ < 0) {
        ERR("cannot open fb0 fd_ = %d", fd_);
        return false;
    }

    DBG("Get Fb Info..");

//    HIFB_SIZE_S ScreenSize = {0, 0};
//    if (ioctl(fd_, FBIOGET_SCREENSIZE, &ScreenSize) < 0) {
//        ERR("get FBIOGET_SCREENSIZE failed!\n");
//    }
//    ERR("==== w = %d h = %d", ScreenSize.u32Width, ScreenSize.u32Height);

//    struct fb_var_screeninfo gst_vinfo;
//    memset(&gst_vinfo, 0, sizeof(gst_vinfo));
//    if (ioctl(fd_, FBIOGET_VSCREENINFO, &gst_vinfo) < 0) {
//        ERR("get v screenInfo failed!");
//        return false;
//    }
//    if (ioctl(fd_, FBIOPUT_VSCREENINFO, &gst_vinfo) < 0) {
//        ERR("set variable screeninfo failed!\n");
//        return false;;
//    }
//    DBG("fb_vinfo: %d, %d, %d, %d", gst_vinfo.xres, gst_vinfo.yres,
//        gst_vinfo.xres_virtual, gst_vinfo.yres_virtual);

    INFO("Get fb_fix_screeninfo");
    struct fb_fix_screeninfo gst_finfo;
    if (ioctl(fd_, FBIOGET_FSCREENINFO, &gst_finfo) == -1) {
        ERR("get fb_fix_screeninfo fail ...");
        return false;
    }

    DBG("finfo.smem_len=%ld", (long) gst_finfo.smem_len);
    fbAddr_ = (uint8_t *) mmap(NULL, gst_finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                               fd_, 0); //step 2: get framebuffer addr
    fbSize_ = gst_finfo.smem_len;
    DBG("fbAddr_ = %p fbSize=%ld", fbAddr_, (long) fbSize_);
    fbOn_ = true;
    return fbOn_;
}

void Fb560::SetSysVersion(int sysVersion) {
    sysVersion_ = sysVersion;
    if (fbAddr_ && cl_.empty()) {
        /*fb操作相关位图初始化*/
        SkImageInfo ii = ii_;
        if (sysVersion_ == SysName_Hi3751V560) {
            ii = ii_.makeColorType(kBGRA_8888_SkColorType);
        }
        bmp0_.setInfo(ii);
        bmp1_.setInfo(ii);
        bmp2_.setInfo(ii);

        bmp0_.setPixels(fbAddr_);
        int fbsize = ii.width() * ii.height() * 4;
        bmp1_.setPixels(fbAddr_ + fbsize);
        bmp2_.setPixels(fbAddr_ + fbsize * 2);

        c0_ = new SkCanvas(bmp0_);
        c1_ = new SkCanvas(bmp1_);
        c2_ = new SkCanvas(bmp2_);
        cl_.push_back(c0_);
        cl_.push_back(c1_);
        cl_.push_back(c2_);
        fbOn_ = true;
    }
}

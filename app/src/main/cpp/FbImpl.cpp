#include "FbAbstract.h"
#include "LibFb.h" //睿屏的fb
#include "LibFb_kjd_311d2.h"
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "SkiaLog.h"

uint16_t red = 0xf800;
uint16_t green = 0x07e0;
uint16_t blue = 0x001f;
uint16_t white = 0xffff;
uint16_t black = 0x0;

uint32_t red32 = 0xffff0000;
uint32_t green32 = 0xff00ff00;
uint32_t blue32 = 0xff0000ff;
uint32_t white32 = 0xffffffff;
uint32_t black32 = 0xff000000;

FbAbstract::FbAbstract()
{
    fd_ = -1;
    fbOn_ = false;
    fbAddr_ = nullptr;
    fbSize_ = 0;
}

FbAbstract::~FbAbstract() {
    if (fd_ >0) {
        munmap(fbAddr_, fbSize_);
        close(fd_);
    }
    fbAddr_ = nullptr;
    fbSize_ = 0;
    fd_ = -1;
}
/*睿频的fb*/
#if m7332 && FRAME_BUF
FbRuiPing::~FbRuiPing()
{
    if (fbAddr_ != nullptr) {
#if 1
        fbdraw_release(0);
#else
        fb_.fbdraw_release(0);
#endif
    }
}

bool FbRuiPing::InitFb() {
#if 1
    int h = 0;
    if (ii_.height() < 1080) {
        h = ii_.height();
        ii_ = ii_.makeWH(ii_.width(), 1080);
    }
    int res = fbdraw_init(0, ii_.width(), ii_.height());
    if (res != 0) {
        INFO("RuiPing Fb Init failed! res = %d", res);
        return fbOn_;
    }
    fbAddr_ = (uint8_t*)fbdraw_addr(0);
#else
    fb_.handle = dlopen("system/lib/libFbAddr.so", RTLD_NOW);
    if (fb_.handle != nullptr) {
        fb_.fbdraw_init = (fun) dlsym(fb_.handle, "fbdraw_init");
        fb_.fbdraw_addr = (fun1) dlsym(fb_.handle, "fbdraw_addr");
        fb_.fbdraw_release = (fun2) dlsym(fb_.handle, "fbdraw_release");
    } else {
        ERR("Cannot open library: %s", dlerror());
    }
    if (fb_.fbdraw_init != nullptr) {
        int res = fb_.fbdraw_init(0, ii_.width(), ii_.height());
        if (res != 0) {
            INFO("RuiPing Fb Init failed! res = %d", res);
            return fbOn_;
        }

        fbAddr_ = (uint8_t*)fb_.fbdraw_addr(0);
    }
#endif

    if (fbAddr_) {
        if (h != 0) {
            SkImageInfo ii = ii_.makeWH(ii_.width(), h);
            SetImageInfo(ii);
        } else {
            /*fb操作相关位图初始化*/
            bmp0_.setInfo(ii_);
            bmp0_.setPixels(fbAddr_);
            c0_ = new SkCanvas(bmp0_);
            cl_.push_back(c0_);
        }
        fbOn_ = true;
    }
    return fbOn_;
}

void FbRuiPing::ClearFb()
{
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawColor(SK_ColorTRANSPARENT, SkBlendMode::kClear);
        ++it;
    }
}

void FbRuiPing::SetImageInfo(SkImageInfo &ii) {
    if (fbAddr_ != nullptr) {
        auto it = cl_.begin();
        while (it != cl_.end()) {
            delete *it;
            ++it;
        }
        cl_.clear();
        void *ptr = fbAddr_;
        if (ii.height() < ii_.height()) {
            int offset = ii_.width() * (ii_.height() - ii.height()) * 4;
            ptr = fbAddr_ + offset;
        }
        bmp0_.setInfo(ii);
        bmp0_.setPixels(ptr);
        c0_ = new SkCanvas(bmp0_);
        cl_.push_back(c0_);
    }
    ii_ = ii;
}
#endif


#if rp982 && FRAME_BUF
FbRuiPing982::~FbRuiPing982() {
    if (fbAddr_ != nullptr) {
        fbdraw_release(0);
    }
}
bool FbRuiPing982::InitFb() {
    int res = fbdraw_init(0, ii_.width(), ii_.height());
    if (res != 0) {
        INFO("RuiPing982 Fb Init failed! res = %d", res);
        return fbOn_;
    }
    INFO("rp 982 fb");
   fbAddr_ = (uint8_t*)fbdraw_addr(0);

    if (fbAddr_) {
        /*fb操作相关位图初始化*/
        bmp0_.setInfo(ii_);
        bmp0_.setPixels(fbAddr_);
        c0_ = new SkCanvas(bmp0_);
        cl_.push_back(c0_);
        fbOn_ = true;
    }
    return fbOn_;
}
void FbRuiPing982::ClearFb() {
    auto it = cl_.begin();
    while (it != cl_.end()) {
        (*it)->drawColor(SK_ColorTRANSPARENT, SkBlendMode::kClear);
        ++it;
    }
}
#endif

bool Fb560::InitFb(int w, int h) {
    DBG("Will Open Fb");
    fd_ = open("/dev/graphics/fb0", O_RDWR, 0); //step 1: open and init fbdev
    if (fd_ < 0) {
        ERR("cannot open fb0 fd_ = %d", fd_);
        return false;
    }
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
    memset(fbAddr_, 0, fbSize_);
    return fbOn_;
}
void Fb560::RefreshAccelerateDraw(int left, int top, int w, int h, uint8_t *src) {
    if (src) {
        INFO("FLUSH left = %d top = %d w = %d h = %d src = %p", left, top, w, h, src);
        int x = 0, y = 0;
        int bottom = top + h;
        int right = left + w;
        int tvalue = 0;
        int toffset = 0;
        int rect_width = w;
        int fb_width = 1920;
        unsigned int value = 0;
        int offset = 0;
        int32_t *dst = (int32_t*)src;
        int32_t *fb0=(int32_t*)fbAddr_;
        int32_t *fb1=(int32_t*)fbAddr_ + 1920 * 1080;
        int32_t *fb2=(int32_t*)fbAddr_ + 1920 * 1080 * 2;
        for (y = top; y <= bottom; y++) {
            tvalue = (y - top) * rect_width - left;
            toffset = y * fb_width;
            for (x = left; x < right; x++) {
                offset = toffset + x;
                value = *(int32_t *) (dst + offset) & 0xffffffff;
                unsigned char a = (value >> 24) & 0xFF;
                unsigned char b = (value >> 16) & 0xFF;
                unsigned char g = (value >> 8) & 0xFF;
                unsigned char r = (value >> 0) & 0xFF;
                value = 0;
                value |= a;
                value <<= 8;
                value |= r;
                value <<= 8;
                value |= g;
                value <<= 8;
                value |= b;
                {
                    *(int32_t *) (fb0 + offset) = value;
                    *(int32_t *) (fb1 + offset) = value;
                    *(int32_t *) (fb2 + offset) = value;
                }
            }
        }
    }
}


//#if KJD_311D2 && FRAME_BUF
Fbkjd311d2::Fbkjd311d2() {}
Fbkjd311d2::~Fbkjd311d2() {
    INFO("Fbkjd311d2 fbAddr_ = %p", fbAddr_);
    if (fbAddr_) {
        kjd311d2_fbdraw_release();
        fbAddr_ = nullptr;
    }
}
bool Fbkjd311d2::InitFb(int w, int h) {
    DBG("Will Open Fb");
    kjd311d2_fbdraw_init(w, h);
    fbAddr_ = (uint8_t*)kjd311d2_fbdraw_addr();
    if (fbAddr_) {
        fbOn_ = true;
        return true;
    }
    return false;
}

void Fbkjd311d2::EnableFb(bool flag) {
    if (flag) {
        if (!fbEnable_) {
            kjd311d2_fbEnable(1);
            ClearBuffer();
            FbRefresh();
        }
        fbEnable_ = true;
    } else {
        if (fbEnable_) {
            kjd311d2_fbEnable(0);
        }
        fbEnable_ = false;
    }
}


void Fbkjd311d2::ClearFb() {
    ClearBuffer();
}

void Fbkjd311d2::FbRefresh() {
    kjd311d2_fbRefresh();
}

void Fbkjd311d2::ClearBuffer() {
    uint8_t *fb0 = fbAddr_;
    uint8_t *fb1 = fbAddr_ + 3840 * 2160 * 4;
    ClearBuffer(0, 0 , 3840, 2160, fb0);
    ClearBuffer(0, 0 , 3840, 2160, fb1);
}

void Fbkjd311d2::ClearBuffer(int x, int y, int w, int h, uint8_t *buf) {
    uint32_t *loc32 = (uint32_t*)buf;
    int i = 0 ,j = 0;
    int stride = 32;
    for (j = y; j < (y+h); j++)
        for (i = x; i<(x+w); i++)
            loc32[i + j*(stride)] = black32;
}

void Fbkjd311d2::RefreshAccelerateDraw(int left, int top, int w, int h, uint8_t *src) {
    if (src) {
        int x = 0, y = 0;
        int bottom = top + h;
        int right = left + w;
        int tvalue = 0;
        int toffset = 0;
        int rect_width = w;
        int fb_width = 3840;
        int value = 0;
        int offset = 0;
        uint8_t *fb0 = fbAddr_;
        uint8_t *fb1 = fbAddr_ + 3840 * 2160 * 4;
        for (y = top; y <= bottom; y++) {
            tvalue = (y - top) * rect_width - left;
            toffset = y * fb_width;
            for (x = left; x <= right; x++) {
                value = *(src + tvalue + x) & 0xffffffff;
                offset = toffset + x;
                {
                    *(int32_t *) (fb0 + offset) = value;
                    *(int32_t *) (fb1 + offset) = value;
                }
            }
        }
    }
}





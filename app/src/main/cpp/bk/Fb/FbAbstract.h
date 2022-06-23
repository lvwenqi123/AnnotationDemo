//
// Created by TEST on 2020/8/25.
//

#ifndef INC_FbAbstract_H
#define INC_FbAbstract_H
#include <SkCanvas.h>
#include <SkBitmap.h>
#include <list>
#include <memory>
#include "common.h"


class FbAbstract{
public:
    FbAbstract();
    virtual ~FbAbstract();

    virtual bool InitFb() = 0;
    bool IsFbOn() const {return fbOn_;}
    void SetImageInfo(SkImageInfo &ii);
    void DrawPath(SkPath &path, SkPaint &paint);
    void DrawBitmapRectToRect(SkBitmap &bmp, SkRect &src, SkRect &dst);
    void DrawBitmap(SkBitmap &bmp, float x, float y);
    void DrawCircle(float x, float y, SkScalar radius, const SkPaint &paint);
    virtual void ClearFb() {}
    virtual void ClearFbByRect(SkRect &bound) {}
    uint8_t *GetFbBuf() {return buf_;}
    virtual void SetSysVersion(int sysVersion) {sysVersion_ = sysVersion;}
    int GetSysVersion() {return sysVersion_;}
protected:
    SkBitmap bmp0_,bmp1_,bmp2_;
    SkCanvas *c0_ = nullptr, *c1_ = nullptr, *c2_ = nullptr;
    std::list<SkCanvas*> cl_;
    int fd_ = -1;
    bool fbOn_ = false;
    uint8_t* fbAddr_ = nullptr;
    size_t fbSize_ = 0;
    SkImageInfo ii_;
    uint8_t *buf_;
    int sysVersion_;
};
typedef std::shared_ptr<FbAbstract>FbAbstractPtr;




#if SYS_6 && FRAME_BUF
class Fb648 : public FbAbstract{
public:
    virtual bool InitFb() override;
private:
    inline size_t round_up_to_page_size(size_t x) {
        return (x + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
    }
};
#endif

class Fb811 : public FbAbstract{
public:
    virtual bool InitFb()override;
    virtual void ClearFb() override;
    virtual void ClearFbByRect(SkRect &bound)override;
};

class Fb560 : public FbAbstract{
public:
    virtual bool InitFb() override ;
    virtual void SetSysVersion(int sysVersion) override;
};

class FbFactory{
public:
    static FbAbstractPtr CreateFb(int sys)
    {
        FbAbstractPtr fb = nullptr;
#if SYS_6
        fb = std::make_shared<Fb648>();
#endif
#if FRAME_BUF
        switch (sys) {
            case SysName_Hi3751V560:
                fb = std::make_shared<Fb560>();
                break;
            case SysName_Hi811:
                fb = std::make_shared<Fb811>();
                break;
            default:
                break;
        }
#endif
        return fb;
    }
};
#endif

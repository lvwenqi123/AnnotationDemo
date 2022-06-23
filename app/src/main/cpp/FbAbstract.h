//
// Created by TEST on 2020/8/25.
//

#ifndef INC_FbAbstract_H
#define INC_FbAbstract_H
#include <list>
#include <memory>
#include <dlfcn.h>
#include <linux/fb.h>
#include "Define.h"

class FbAbstract{
public:
    FbAbstract();
    virtual ~FbAbstract();

    virtual bool InitFb(int w, int h) = 0;
    virtual void EnableFb(bool flag) {fbEnable_ = flag;}
    bool IsFbOn() const {return fbOn_;}
    virtual void ClearFb() {}
    virtual void SetSysVersion(int sysVersion) {sysVersion_ = sysVersion;}
    int GetSysVersion() {return sysVersion_;}
    virtual void RefreshAccelerateDraw(int x, int y, int w, int h, uint8_t *pixel) {}
protected:
    int fd_ = -1;
    bool fbOn_ = false;
    uint8_t* fbAddr_ = nullptr;
    size_t fbSize_ = 0;
    int sysVersion_;
    bool fbEnable_ = false;
};
typedef std::shared_ptr<FbAbstract>FbAbstractPtr;

//睿频fb
#if m7332 && FRAME_BUF
class FbRuiPing : public FbAbstract {
public:
    typedef int (*fun) (int layer,int width,int height);
    typedef void* (*fun1) (int layer);
    typedef int (*fun2)(int layer);
    typedef struct Fbfun{
        void *handle;
        fun fbdraw_init;
        fun1 fbdraw_addr;
        fun2 fbdraw_release;
        Fbfun():handle(nullptr) { }
        ~Fbfun(){
            if (handle) { dlclose(handle); }
        }
    }Fbfun;
public:
    virtual ~FbRuiPing();
    virtual bool InitFb() override ;
    virtual void ClearFb() override;
    virtual void SetImageInfo(SkImageInfo &ii)override;
protected:
    Fbfun fb_;
};
#endif

#if rp982 && FRAME_BUF
class FbRuiPing982 : public FbAbstract {
public:
    virtual ~FbRuiPing982();
    virtual bool InitFb() override;
    virtual void ClearFb() override;
};
#endif

class Fb560 : public FbAbstract{
public:
    virtual bool InitFb(int w, int h) override;
    virtual void RefreshAccelerateDraw(int left, int top, int w, int h, uint8_t *pixel)override;
};
#endif

//#if KJD_311D2 && FRAME_BUF
class Fbkjd311d2 : public FbAbstract{
    public:
    Fbkjd311d2();
    virtual ~Fbkjd311d2();
    virtual bool InitFb(int w, int h) override;
    virtual void EnableFb(bool flag) override;
    virtual void ClearFb() override;
    virtual void RefreshAccelerateDraw(int x, int y, int w, int h, uint8_t *pixel)override;
protected:
    void ClearBuffer();
    void FbRefresh();
    void ClearBuffer(int x, int y, int w, int h, uint8_t *buf);
};
//#endif

class FbFactory{
public:
    static FbAbstractPtr CreateFb(int sys)
    {
        FbAbstractPtr fb = nullptr;
#if FRAME_BUF
        switch (sys) {
            case SysName_Hi3751V560:
                fb = std::make_shared<Fb560>();
                break;
            case SysName_RuiPingM7332:
#if m7332
                fb = std::make_shared<FbRuiPing>();
#endif
                break;
            case SysName_RP982:
#if rp982
                fb = std::make_shared<FbRuiPing982>();
#endif
                break;
            case SysName_KJD311d2:
#if KJD_311D2
                fb = std::make_shared<Fbkjd311d2>();
#endif
                break;
            default:
                break;
        }
#endif
        return fb;
    }
};

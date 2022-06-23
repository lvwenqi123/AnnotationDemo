/*
 * NativeWindowInterface.h
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */

#ifndef NATIVEWINDOWINTERFACE_H_
#define NATIVEWINDOWINTERFACE_H_

#include <android/native_window.h>
#include <SkCanvas.h>
#include <android/rect.h>
#include <iostream>
#include <SkImageEncoder.h>
#include <list>
#include "common.h"
#include "FbAbstract.h"


//typedef std::shared_ptr<SkCanvas>SkCanvasPtr;


class NativeWindowInterface{
public:
	static NativeWindowInterface *GetInstance();
	static void DestroyInstance();

	void Init(ANativeWindow *mANativeWindow, int format);
	void UnInit();

	void SetErasePic(SkBitmap *bmp);
	void SetBg(SkBitmap *bg0 ,SkBitmap *bg1);
	void UpdateNativeWindow(ANativeWindow *mANativeWindow, int format);
	void DestroyNativeWindow(ANativeWindow *mANativeWindow);

    int FlushWindow(ARect *rect);
    int FlushWindow(SkCanvas *c, ARect *rect);
	int FlushFramebuf(ARect *rect);

	SkCanvas* LockCanvas(ARect *rect); //成对使用
	int unlockCanvasAndPost(SkCanvas *c);
	void DrawPath(SkPath &path, SkPaint &paint, bool save=true);
	void DrawPathByFb(SkPath &path, SkPaint &paint, bool save=true);
	void DrawPath(SkPath &path, SkPath &path1, SkPaint &paint, bool save=true);
	void DrawPoint(SkPoint &point, SkPaint &paint);
	void DrawBitmapRectToRect(SkBitmap &bmp, SkRect &src, SkRect &dst);
	void DrawBitmap(SkBitmap &bmp, float x, float y, SkRect &src);
	void DrawRoundRect(const SkRect& rect, SkScalar rx, SkScalar ry,
	                       const SkPaint& paint);
	void DrawCircle(float x, float y, SkScalar radius, const SkPaint &paint, bool save);
	void DrawCircleByFb(float x, float y, SkScalar radius, const SkPaint &paint, bool save);
	void DrawText(const void* text, size_t byteLength, SkScalar x, SkScalar y, const SkPaint& paint);
	int Refresh(ARect *rect);
	int Refresh(SkCanvas *c, ARect *rect1, ARect *rect2);
	bool FbOn() {return fbOn_;}

	SkCanvas *GetSkCanvas() {return skCanv_;};
	SkBitmap& GetBmp() {return cacheBmp_;};
	SkBitmap* GetEraseBmp() {return eraseBmp_;}

	void DrawJavaCacheBmp(SkRect rect);

	int8_t* GetPixelBuf() {return buf_;};
	int GetPixelFormat() {return format_;}
	const SkImageInfo& Info() const { return ii_; }
	void ClearFb();
	void ClearFb(SkRect r);
	void AddUserCvs(SkCanvas *c) {skCanv1_ = c;}; //有save标志的接口 会同步数据到该cvs
	SkRect GetRectScreen() {return rectScreen_;};
	int Width() const {return ii_.width();}
	int Height() const {return ii_.height();}
	const ScreenMode GetScreenMode();	void SetBusy(bool busy) {busy_ = busy;}
	bool IsBusy() {return busy_;}
	/*刷新失败次数*/
	void SetFailuresNum(int num) {failuresNum_ = num;}
    void clearBuffer();
	void SetSysVersion(int sysVersion);
	void SetJavaCacheBitmap(JNIEnv *env, jobject jbitmap) {
		jcacheBitmap_ = jbitmap;
		env_ = env;
	}
	jobject GetJavaCacheBitmap() {return jcacheBitmap_;}
	int GetSys() const {return sys_;}
	bool ResetFb(int w, int h);
	bool ResetFb(int x, int y, int w, int h);
	void EnableFb(bool flag);

private:
	NativeWindowInterface();
	~NativeWindowInterface();

	int RefreshWindow(SkCanvas *c, ARect *r1, ARect *r2);
	int RefreshFramebuf(SkCanvas *c, ARect *r1, ARect *r2);
	void InitFrameBuf(int sys);
	void ReadPixel2Buf(SkCanvas *c, const ARect *r, int32_t *dstBuf);
	void ReadPixel2FrameBuf(SkCanvas *c, const ARect *r);
	bool InitVirtualInput();
	int ShowMouseWhere(int dx,int dy);
	void CloseVirtualInput();

protected:
	ANativeWindow *nativeWindow_ = NULL;
	SkCanvas *skCanv_ = NULL;
	SkCanvas *skCanv1_ = nullptr;
	int w_;
	int h_;
	int format_;
	int bufSize_;
	int8_t* buf_ = NULL;
	SkBitmap cacheBmp_;
	SkBitmap *eraseBmp_;
	SkBitmap *fbBgBmp_ = NULL, *bgBmp_ = NULL;
	SkImageInfo ii_;
	int fd_ = -1;
	size_t fbSize_ = 0;
	SkRect rectScreen_;
	bool fbOn_;
    FbAbstractPtr fbdev_ = nullptr;
	int uinp_fd_ = -1;
	bool bVirtualInputInit_ = false;
	static NativeWindowInterface *ins_;
	bool busy_;
    std::atomic_int failuresNum_; //失败次数
	jobject jcacheBitmap_;
	JNIEnv *env_;
	int sys_;
};


#endif /* NATIVEWINDOWINTERFACE_H_ */

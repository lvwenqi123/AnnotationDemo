/*
 * NativeWindowInterface.cpp
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */

#include "NativeWindowInterface.h"

#include "SkCanvas.h"
#include <stdio.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/fb.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <android/bitmap.h>

#define Custom_PointerIcon_Property			"persist.sys.custom.pointer"

const int _erasePicW = 200;
const int _erasePicH = 200;

inline size_t round_up_to_page_size(size_t x)
{
    return (x + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
}

NativeWindowInterface *NativeWindowInterface::ins_ = NULL;
NativeWindowInterface* NativeWindowInterface::GetInstance()
{
	if (!ins_) {
		ins_ = new NativeWindowInterface();
	}
	return ins_;
}

void NativeWindowInterface::DestroyInstance()
{
	if (ins_) {
		delete ins_;
		ins_ = NULL;
	}

}

NativeWindowInterface::NativeWindowInterface()
{
	skCanv_ = 0;
	nativeWindow_ = 0;
	w_ = 0;
	h_ = 0;
	buf_ = 0;
	bufSize_ = 0;
	fbOn_ = false;
	format_ = 0;
	eraseBmp_ = NULL;
	SetBusy(false);
    SetFailuresNum(0);
}
NativeWindowInterface::~NativeWindowInterface()
{
	UnInit();
}

void NativeWindowInterface::Init(ANativeWindow *mANativeWindow, int format)
{
	ENTER_F
	nativeWindow_ = mANativeWindow;
	format_ = format;
	ANativeWindow_acquire(nativeWindow_);

	w_  = ANativeWindow_getWidth(nativeWindow_);
	h_  = ANativeWindow_getHeight(nativeWindow_);

	ANativeWindow_setBuffersGeometry(nativeWindow_, 0, 0, format);
	SkImageInfo ii = SkImageInfo::Make(w_, h_, Tools::convertPixelFormat(format_), kOpaque_SkAlphaType);
	ii_ = ii;
	cacheBmp_.allocPixels(ii);

	skCanv_ = new SkCanvas(cacheBmp_);
	//InitFrameBuf();
	//InitVirtualInput();
	rectScreen_ = SkRect::MakeLTRB(0, 0, w_, h_);
	INFO("Native init w = %d h = %d", w_, h_);
	EXIT_F
}

void NativeWindowInterface::SetErasePic(SkBitmap *bmp)
{
	eraseBmp_ = bmp;
}

void NativeWindowInterface::SetBg(SkBitmap *bg0 ,SkBitmap *bg1)
{
	fbBgBmp_ = bg0;
	bgBmp_ = bg1;
}

void NativeWindowInterface::UpdateNativeWindow(ANativeWindow *mANativeWindow, int format)
{
	if (mANativeWindow) {
		int w  = ANativeWindow_getWidth(mANativeWindow);
		int h  = ANativeWindow_getHeight(mANativeWindow);
		INFO("UpdateNativeWindow w = %d h = %d", w, h);

		if (!nativeWindow_) {
			nativeWindow_ = mANativeWindow;
			ANativeWindow_acquire(nativeWindow_);
			ANativeWindow_setBuffersGeometry(nativeWindow_, 0, 0, format_);
		}
/*处理横竖屏*/
		if (w != w_ || h != h_) {
			w_ = w;
			h_ = h;
			SkImageInfo ii = SkImageInfo::Make(w_, h_,
											   Tools::convertPixelFormat(format_), kOpaque_SkAlphaType);
			ii_ = ii;
			cacheBmp_.reset();
			cacheBmp_.allocPixels(ii_);
			delete skCanv_;
			skCanv_ = new SkCanvas(cacheBmp_);
            rectScreen_ = {0,0, (float)w_, (float)h_};
			//ReInitFrameBuf();
		}
	}
}

void NativeWindowInterface::DestroyNativeWindow(ANativeWindow *mANativeWindow)
{
	DBG("DestroyNativeWindow mANativeWindow = %p nativeWindow_ = %p", mANativeWindow, nativeWindow_);
	if (nativeWindow_) {
		ANativeWindow_release(nativeWindow_);
		nativeWindow_ = NULL;
	}
}

/*fb初始化*/

void NativeWindowInterface::InitFrameBuf(int sys)
{
	if (fbdev_ == nullptr) {
		fbdev_ = FbFactory::CreateFb(sys);
		if (fbdev_) {
			fbdev_->SetImageInfo(ii_);
			fbOn_ = fbdev_->InitFb();
		}
	}
}

void NativeWindowInterface::ClearFb()
{
	if (fbdev_) fbdev_->ClearFb();
}

void NativeWindowInterface::ClearFb(SkRect rect)
{
	const char *fbAddr_ = nullptr;
	int left = (int)rect.left();
	int right = (int)rect.right();
	int top = (int)rect.top();
	int bottom = (int)rect.bottom();
	int32_t *fb0 = (int32_t*) fbAddr_;
	if (fbAddr_) {
		int value = 0;
		int32_t toffset;
		int32_t offset;
		if (bottom >= _FB_HEIGHT)
			bottom = _FB_HEIGHT - 1;
		if (right >= _FB_WIDTH)
			right = _FB_WIDTH - 1;
		for (int y = top; y <= bottom; y++) {
			toffset = y * _FB_WIDTH;
			for (int x = left; x <= right; x++) {
				offset = toffset + x;
				*(fb0 + offset) = value;
			}
		}
	}
}

void NativeWindowInterface::UnInit()
{
	DBG("Enter NativeWindowInterface::UnInit");
	if (nativeWindow_) {
		ANativeWindow_release(nativeWindow_); /*释放surface对象*/
		nativeWindow_ = NULL;
	}
	if (buf_) {
		delete []buf_;
		buf_ = NULL;
	}
	cacheBmp_.reset();
	if (skCanv_) {
		delete skCanv_;
		skCanv_ = nullptr;
	}
	fbOn_ = false;
	fbdev_ = nullptr;
    CloseVirtualInput();
	DBG("Exit NativeWindowInterface::UnInit");
}

void NativeWindowInterface::DrawPath(SkPath &path, SkPaint &paint, bool save)
{
	if (save) {
		skCanv_->drawPath(path, paint);
		skCanv1_->drawPath(path, paint);
	}
}

void NativeWindowInterface::DrawPathByFb(SkPath &path, SkPaint &paint, bool save)
{
	if (fbOn_) {
		if (fbdev_) fbdev_->DrawPath(path, paint);
	}
}

void NativeWindowInterface::DrawPath(SkPath &path, SkPath &path1, SkPaint &paint, bool save)
{
	if (fbOn_) {
		if (fbdev_) fbdev_->DrawPath(path, paint);
	}
    if (save) {
        skCanv_->drawPath(path1, paint);
        skCanv1_->drawPath(path1, paint);
    }
}

void NativeWindowInterface::DrawPoint(SkPoint &point, SkPaint &paint)
{
	//skCanv_->drawPoint(point.fX, point.fY, paint);
}

void NativeWindowInterface::DrawBitmapRectToRect(SkBitmap &bmp, SkRect &src, SkRect &dst)
{
	//if (fbdev_) fbdev_->DrawBitmapRectToRect(*fbBgBmp_, src, dst);
	CanvasAdapt::DrawBitmapRect(skCanv_, *fbBgBmp_, src, dst);
}

void NativeWindowInterface::DrawBitmap(SkBitmap &bmp, float x, float y, SkRect &src)
{
	if (fbdev_) fbdev_->DrawBitmap(bmp, x, y);
	skCanv_->drawBitmap(bmp, x, y);
}

void NativeWindowInterface::DrawRoundRect(const SkRect& rect, SkScalar rx, SkScalar ry,
	                       const SkPaint& paint)
{
}

void NativeWindowInterface::DrawCircle(float x, float y, SkScalar radius, const SkPaint &paint, bool save)
{
	if (save) {
		skCanv_->drawCircle(x, y, radius, paint);
		skCanv1_->drawCircle(x, y, radius, paint);
	}
}

void NativeWindowInterface::DrawCircleByFb(float x, float y, SkScalar radius, const SkPaint &paint, bool save)
{
	if (fbdev_) fbdev_->DrawCircle(x, y, radius, paint);
}

void NativeWindowInterface::DrawText(const void* text, size_t byteLength, SkScalar x, SkScalar y, const SkPaint& paint)
{
	//skCanv_->drawText(text, byteLength, x ,y, paint);
}
int NativeWindowInterface::Refresh(ARect *rect)
{
	if (fbOn_) {
		FlushFramebuf(rect);
	} else {
		FlushWindow(rect);
	}
	return 0;
}

int NativeWindowInterface::Refresh(SkCanvas *c, ARect *rect1, ARect *rect2)
{
	if (fbOn_) {
		RefreshFramebuf(c, rect1, rect2);
	} else {
		RefreshWindow(c, rect1, rect2);
	}
	return 0;
}

int NativeWindowInterface::FlushWindow(ARect *rect)
{
	if (!nativeWindow_) return -1;
	int ret = 0;
	ANativeWindow_Buffer nwBuffer;

	ret = ANativeWindow_lock(nativeWindow_, &nwBuffer, rect);
	if (0 != ret) {
		ERR("FlushWindow ANativeWindow_lock error %d", ret);
		return ret;
	}

	if (!rect) {
		ssize_t bpr = nwBuffer.stride *  Tools::bytesPerPixel(nwBuffer.format);
		//DBG("Update Android Window size = %ld bit = %p w = %d h = %d",
		//		bpr, nwBuffer.bits, (int)nwBuffer.width, (int)nwBuffer.height);
		//DBG("FW Format = %d", nwBuffer.format);
		SkImageInfo imgInfo = SkImageInfo::Make(nwBuffer.width, nwBuffer.height,
				Tools::convertPixelFormat(nwBuffer.format),
				nwBuffer.format == android::PIXEL_FORMAT_RGBX_8888
				? kOpaque_SkAlphaType : kPremul_SkAlphaType);
		skCanv_->readPixels(imgInfo, nwBuffer.bits, bpr, 0, 0);
	} else {
		ReadPixel2Buf(skCanv_, rect, (int32_t*)nwBuffer.bits);
	}
	if (0 != ANativeWindow_unlockAndPost(nativeWindow_)) {
		ERR("ANativeWindow_unlockAndPost Failed ErrCode = %d", -1);
	}
	return ret;
}

int NativeWindowInterface::FlushWindow(SkCanvas *c, ARect *rect)
{
	if (IsBusy()) return -1;
	SetBusy(true);
	int ret = 0;
	if (!c) return -1;
	if (!nativeWindow_) {
	    ERR("NativeWindowInterface::nativeWindow_ is NULL");
        SetBusy(false);
        return -1;
	}
	ANativeWindow_Buffer nwBuffer;
	ret = ANativeWindow_lock(nativeWindow_, &nwBuffer, rect);
	if (0 != ret) {
		ERR("ANativeWindow_lock error %d", ret);
        SetBusy(false);
		return ret;
	}
	if (!rect) {
		size_t bpr = nwBuffer.stride * Tools::bytesPerPixel(nwBuffer.format);
		//DBG("Update Android Window size = %ld bit = %p w = %d h = %d",
		//		bpr, nwBuffer.bits, (int)nwBuffer.width, (int)nwBuffer.height);
        SkImageInfo imgInfo = SkImageInfo::Make(nwBuffer.width, nwBuffer.height,
                Tools::convertPixelFormat(nwBuffer.format),
                nwBuffer.format == android::PIXEL_FORMAT_RGBX_8888
                ? kOpaque_SkAlphaType : kPremul_SkAlphaType);
		c->readPixels(imgInfo, nwBuffer.bits, bpr, 0, 0);
	} else {
		ReadPixel2Buf(c, rect, (int32_t*)nwBuffer.bits);
	}
	if (0 != ANativeWindow_unlockAndPost(nativeWindow_)) {
		ret = -1;
		ERR("ANativeWindow_unlockAndPost Failed ErrCode = %d", -1);
	}
	//loseNum_ = 3;
	SetBusy(false);
	return ret;
}

int32_t *fb0 = NULL;
int32_t *fb1 = NULL;
int32_t *fb2 = NULL;

int NativeWindowInterface::FlushFramebuf(ARect *r)
{
#if 0
	const ARect *rect = r;
	int left = rect->left;
	int right = rect->right;
	int top = rect->top;
	int bottom = rect->bottom;
	fb0 = (int32_t*) fbAddr_;
	fb1 = (int32_t*) (fbAddr_ + _FB_BUF_SIZE);
	fb2 = (int32_t*) (fbAddr_ + _FB_BUF_SIZE * 2);

	if (fbAddr_) {
		int w = rect->right - rect->left + 1;
		int h = rect->bottom - rect->top + 1;
		//if (w >_FB_WIDTH || h > _FB_HEIGHT)return -1;
		SkImageInfo imgInfo = SkImageInfo::Make(w, h, kN32_SkColorType,
				kPremul_SkAlphaType);
		bool r = skCanv_->readPixels(imgInfo, buf_,
				imgInfo.width() * imgInfo.bytesPerPixel(), left, top);
		int32_t *src = (int32_t*) buf_;
		int32_t offset_size = 0;
		int value = 0;
		int32_t tvalue;
		int32_t toffset;
		int32_t offset;
		int32_t rect_width = imgInfo.width();
		int32_t alp;
		if (bottom >= _FB_HEIGHT)
			bottom = _FB_HEIGHT - 1;
		if (right >= _FB_WIDTH)
			right = _FB_WIDTH - 1;
		for (int y = top; y <= bottom; y++) {
			tvalue = (y - top) * rect_width - left;
			toffset = y * _FB_WIDTH;
			for (int x = left; x <= right; x++) {
				value = *(src + tvalue + x) & 0xffffffff;
				offset = toffset + x;
				if (value != 0) {
					*(int32_t*) (fb0 + offset) = *(int32_t*) (fb1 + offset) =
							*(int32_t*) (fb2 + offset) = value;
				}
			}
		}
	}
#endif
	return 0;
}

int NativeWindowInterface::RefreshWindow(SkCanvas *c, ARect *r1, ARect *r2)
{
	int ret = 0;
	ANativeWindow_Buffer nwBuffer;
	ret = ANativeWindow_lock(nativeWindow_, &nwBuffer, r1);
	if (0 != ret) {
		ERR("ANativeWindow_lock error %d", ret);
		return ret;
	}

	if (!r1) {
		ssize_t bpr = nwBuffer.stride * Tools::bytesPerPixel(nwBuffer.format);
		//DBG("Update Android Window size = %ld bit = %p w = %d h = %d",
		//		bpr, nwBuffer.bits, (int)nwBuffer.width, (int)nwBuffer.height);
        SkImageInfo imgInfo = SkImageInfo::Make(nwBuffer.width, nwBuffer.height,
                Tools::convertPixelFormat(nwBuffer.format),
                nwBuffer.format == android::PIXEL_FORMAT_RGBX_8888
                ? kOpaque_SkAlphaType : kPremul_SkAlphaType);
		c->readPixels(imgInfo, nwBuffer.bits, bpr, 0, 0);
	} else {
		if (c) {
			if (r1) {
				ReadPixel2Buf(c, r1, (int32_t*)nwBuffer.bits);
			}
			if (r2) {
				ReadPixel2Buf(c, r2, (int32_t*)nwBuffer.bits);
			}
		}
	}
	if (0 != ANativeWindow_unlockAndPost(nativeWindow_)) {
		ERR("ANativeWindow_unlockAndPost Failed ErrCode = %d", -1);
	}
	return ret;
}

int NativeWindowInterface::RefreshFramebuf(SkCanvas *c, ARect *r1, ARect *r2)
{
	if (r1) {
		ReadPixel2FrameBuf(c, r1);
	}
	if (r2) {
		ReadPixel2FrameBuf(c, r2);
	}
	return 0;
}

void NativeWindowInterface::ReadPixel2Buf(SkCanvas *c, const ARect *rect, int32_t *dstBuf)
{
	int w = rect->right - rect->left + 1;
	int h = rect->bottom - rect->top + 1;
	//if (w > _BMP_W)
		//w = _BMP_W;
	//if (h > _BMP_H)
		//h = _BMP_H;
	//ERR("l = %d r = %d t = %d b = %d", rect->left, rect->right, rect->top, rect->bottom);

	SkImageInfo imgInfo = SkImageInfo::Make(w, h,
			Tools::convertPixelFormat(format_), kPremul_SkAlphaType);
	c->readPixels(imgInfo, buf_, imgInfo.width() * imgInfo.bytesPerPixel(),
			rect->left, rect->top);
	//ERR("w = %d h = %d=======", w, h);
	int32_t *buf = (int32_t*) dstBuf;
	int32_t *src = (int32_t*) buf_;
	unsigned int srcYoffset = 0, dstYoffset = 0;
	for (int y = rect->top; y < rect->bottom; ++y) {
		srcYoffset = (y - rect->top) * imgInfo.width();
		dstYoffset = y * w_;
		for (int x = rect->left; x < rect->right; ++x) {
			*(buf + (x + dstYoffset)) = *(src + (x - rect->left + srcYoffset));
		}
	}
}

void NativeWindowInterface::ReadPixel2FrameBuf(SkCanvas *c, const ARect *r)
{
	const char *fbAddr_ = nullptr;
	const ARect *rect = r;
	int left = rect->left;
	int right = rect->right;
	int top = rect->top;
	int bottom = rect->bottom;

	fb0 = (int32_t*) fbAddr_;
	fb1 = (int32_t*) (fbAddr_ + _FB_BUF_SIZE);
	fb2 = (int32_t*) (fbAddr_ + _FB_BUF_SIZE * 2);

	if (fbAddr_) {
		int w = rect->right - rect->left + 1;
		int h = rect->bottom - rect->top + 1;
		//if (w >_FB_WIDTH || h > _FB_HEIGHT)return -1;
		SkImageInfo imgInfo = SkImageInfo::Make(w, h, kN32_SkColorType,
				kPremul_SkAlphaType);
		bool r = c->readPixels(imgInfo, buf_,
				imgInfo.width() * imgInfo.bytesPerPixel(), left, top);
		int32_t *src = (int32_t*) buf_;
		int32_t offset_size = 0;
		int value = 0;
		int32_t tvalue = 0;
		int32_t toffset = 0;
		int32_t offset = 0;
		int32_t rect_width = imgInfo.width();
		int32_t alp = 0;
		if (bottom >= _FB_HEIGHT)
			bottom = _FB_HEIGHT - 1;
		if (right >= _FB_WIDTH)
			right = _FB_WIDTH - 1;
		for (int y = top; y < bottom; y++) {
			tvalue = (y - top) * rect_width - left;
			toffset = y * _FB_WIDTH;
			for (int x = left; x < right; x++) {
				value = *(src + tvalue + x) & 0xffffffff;
				offset = toffset + x;
				if (value != 0) {
					*(int32_t*) (fb0 + offset) = *(int32_t*) (fb1 + offset) =
							*(int32_t*) (fb2 + offset) = value;
				}
			}
		}
	}
}

#include "RunTimeTest.h"

RunTimeTest tm2;

SkCanvas* NativeWindowInterface::LockCanvas(ARect *rect)
{
    if (failuresNum_ >0) {
        failuresNum_--;
		INFO("may be wait some times return null");
		return nullptr;
	}
	if (IsBusy()) {
	    INFO("ANativeWindow_lock Is Busy return null");
        return nullptr;
	}
	SetBusy(true);
	int ret = 0;
	ANativeWindow_Buffer nwBuffer;
	memset(&nwBuffer, 0, sizeof(nwBuffer));
	ret = ANativeWindow_lock(nativeWindow_, &nwBuffer, rect);
	if (0 != ret) {
		ERR("ANativeWindow_lock error %d", ret);
        SetBusy(false);
		return nullptr;
	}
    SkImageInfo info = SkImageInfo::Make(nwBuffer.width, nwBuffer.height,
            Tools::convertPixelFormat(nwBuffer.format),
            nwBuffer.format == android::PIXEL_FORMAT_RGBX_8888
            ? kOpaque_SkAlphaType : kPremul_SkAlphaType);

	SkBitmap bitmap;
	ssize_t bpr = nwBuffer.stride * Tools::bytesPerPixel(nwBuffer.format);
	bitmap.setInfo(info, bpr);
	if (nwBuffer.width > 0 && nwBuffer.height > 0) {
		bitmap.setPixels(nwBuffer.bits);
	} else {
		// be safe with an empty bitmap.
		bitmap.setPixels(NULL);
		INFO("empty bitmap");
	}
	SkCanvas *tmp = new SkCanvas(bitmap);
	if (rect) {
		SkRect r;
		Tools::ARect2SkRect(rect, &r);
		tmp->clipRect(r);
	}
	return tmp;
}
int NativeWindowInterface::unlockCanvasAndPost(SkCanvas *c)
{
	if (0 != ANativeWindow_unlockAndPost(nativeWindow_)) {
		ERR("ANativeWindow_unlockAndPost Failed ErrCode = %d", -1);
	}
	if (c) {
		delete c;
		c = nullptr;
	}
	SetBusy(false);
	return 0;
}

const ScreenMode NativeWindowInterface::GetScreenMode()
{
	return ii_.width() < ii_.height() ? ScreenMode_Portrait : ScreenMode_Landscape;
}

bool NativeWindowInterface::InitVirtualInput()
{
	bVirtualInputInit_ = false;
#if SYS_9
	int i;
    struct uinput_user_dev uinp;
	uinp_fd_ = open("/dev/uinput", O_WRONLY | O_NDELAY);
    if (uinp_fd_ == 0) {
        return bVirtualInputInit_;
    }
    memset(&uinp, 0x00, sizeof(uinp));
    strncpy(uinp.name, "wb_key", sizeof(uinp.name)-1);
    uinp.id.vendor = 0x046d;
    uinp.id.product = 0x000f;
    uinp.id.bustype = BUS_VIRTUAL;
    ioctl(uinp_fd_, UI_SET_EVBIT, EV_KEY);

    //for (i = 0; i < 0xFF; i++)
    {
        ioctl(uinp_fd_, UI_SET_KEYBIT, BTN_LEFT);
        ioctl(uinp_fd_, UI_SET_KEYBIT, BTN_RIGHT);
    }
    ioctl(uinp_fd_, UI_SET_EVBIT, EV_REL);
    ioctl(uinp_fd_, UI_SET_RELBIT, REL_X);
    ioctl(uinp_fd_, UI_SET_RELBIT, REL_Y);

    if (write(uinp_fd_, &uinp, sizeof(uinp)) != sizeof(uinp)) {
        INFO("First write returned fail.");
        return bVirtualInputInit_;
    }
    if (ioctl(uinp_fd_, UI_DEV_CREATE)) {
		INFO("ioctl UI_DEV_CREATE returned fail.");
        return bVirtualInputInit_;
    }
    bVirtualInputInit_ = true;
#endif
    return bVirtualInputInit_;
}


int NativeWindowInterface::ShowMouseWhere(int dx,int dy)
{
	if (uinp_fd_ >0) {
		struct input_event event;
		memset(&event, 0x00, sizeof(event));
		//gettimeofday(&event.time, NULL);
		event.type = EV_REL;
		event.code = REL_X;
		event.value = dx;
		write(uinp_fd_, &event, sizeof(event));

		memset(&event, 0x00, sizeof(event));
		//gettimeofday(&event.time, NULL);
		event.type = EV_REL;
		event.code = REL_Y;
		event.value = dy;
		write(uinp_fd_, &event, sizeof(event));

		/*memset(&event, 0x00, sizeof(event));
        //gettimeofday(&event.time, NULL);
        event.type = EV_KEY;
        event.code = BTN_LEFT;
        event.value = 1;
        write(uinp_fd, &event, sizeof(event));*/


		memset(&event, 0x00, sizeof(event));
		//gettimeofday(&event.time, NULL);
		event.type = EV_SYN;
		event.code = 0;
		event.value = 0;
		write(uinp_fd_, &event, sizeof(event));
		INFO("ShowMouseWhere");
	}
	return 0;
}

void NativeWindowInterface::CloseVirtualInput()
{
#if SYS_9
	if (uinp_fd_ >0) {
		close(uinp_fd_);
		uinp_fd_ = -1;
	}
#endif
}

void NativeWindowInterface::clearBuffer()
{
#if SYS_9
//    property_set(Custom_PointerIcon_Property,"true");
//    for (int i=0;i<1;i++) {
//        ShowMouseWhere(-1,-1);
//    }
#endif
}

void NativeWindowInterface::SetSysVersion(int sysVersion)
{
	sys_ = sysVersion;
	if (sys_ == SysName_LangGuo311d2) {
		fbOn_ = true;
		return;
	}
	if (fbdev_ == nullptr) {
		INFO("SetSysVersion = sysVersion = %d", sysVersion);
	    InitFrameBuf(sysVersion);
	}
	if (fbdev_) {
		fbdev_->SetSysVersion(sysVersion);
	}
}

void NativeWindowInterface::DrawJavaCacheBmp(SkRect rect) {
	if (sys_ == SysName_LangGuo311d2) {
		AndroidBitmapInfo info;
		int *pixel;
		if (AndroidBitmap_getInfo(env_, jcacheBitmap_,&info) == 0) {
			INFO("AndroidBitmap_getInfo format = %d", info.format);
			if (AndroidBitmap_lockPixels(env_, jcacheBitmap_,(void**)&pixel) == 0) {
				int width = info.width;
				int height = info.height;
				SkBitmap bm;
				SkImageInfo image_info = SkImageInfo::MakeS32(width,height,SkAlphaType::kPremul_SkAlphaType);
				bm.setInfo(image_info,image_info.minRowBytes());
				bm.setPixels(pixel);
				SkCanvas c(bm);
				c.drawBitmapRect(cacheBmp_, rect, rect, nullptr);
				AndroidBitmap_unlockPixels(env_, jcacheBitmap_);
			} else {
				ERR("AndroidBitmap_lockPixels Error");
			}
		} else {
			ERR("AndroidBitmap_getInfo Error");
		}
	}
}

bool NativeWindowInterface::ResetFb(int w, int h) {
    if (GetSys() == SysName_RuiPingM7332 && fbdev_) {
        SkImageInfo ii = fbdev_->GetImageInfo();
        if (w == ii.width() && h != ii.height()) {
            ii = ii.makeWH(w, h);
            fbdev_->SetImageInfo(ii);
            return true;
        }
    }
    return false;
}

bool NativeWindowInterface::ResetFb(int x, int y, int w, int h) {
	if (fbdev_) {
		fbdev_->ResetFb(x ,y, w, h);
	}
	return true;
}

void NativeWindowInterface::EnableFb(bool flag) {
	if (fbdev_) {
		fbdev_->EnableFb(flag);
	}
}
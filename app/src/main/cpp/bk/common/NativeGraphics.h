/*
 * NativeGraphics.h
 *
 *  Created on: 2018年9月19日
 *      Author: chengang
 */

#ifndef NATIVEGRAPHICS_H_
#define NATIVEGRAPHICS_H_
#include "SkiaLog.h"
#include <android/bitmap.h>


class NativeGraphics{
public:
	/*使用该接口时内存需要自己释放，否则会造成内存泄露*/
    static SkBitmap *GetNativeBitmap(JNIEnv *env, jobject bitmap) {
        if (!bitmap) {
            ERR("ERROR, bitmap = %p", bitmap);
            return nullptr;
        }
        SkASSERT(env);
        SkASSERT(bitmap);
        SkBitmap *outBitmap = nullptr;
        AndroidBitmapInfo info;
        int *pixel;
        if (AndroidBitmap_getInfo(env, bitmap, &info) == 0) {
            INFO("AndroidBitmap_getInfo format = %d", info.format);
            if (AndroidBitmap_lockPixels(env, bitmap, (void **) &pixel) == 0) {
                int width = info.width;
                int height = info.height;
                SkBitmap bm;
                SkImageInfo image_info = SkImageInfo::MakeS32(width, height,
                                                              SkAlphaType::kPremul_SkAlphaType);
                bm.setInfo(image_info, image_info.minRowBytes());
                bm.setPixels(pixel);
                INFO("pixel = %p", pixel);
                outBitmap = new SkBitmap();
                outBitmap->allocPixels(image_info);
                SkCanvas c(*outBitmap);
                c.drawColor(0, SkBlendMode::kClear);
                c.drawBitmap(bm, 0, 0);
                AndroidBitmap_unlockPixels(env, bitmap);
            } else {
                ERR("AndroidBitmap_lockPixels Error");
            }
        } else {
            ERR("AndroidBitmap_getInfo Error");
        }
        return outBitmap;
    }
};




#endif /* NATIVEGRAPHICS_H_ */

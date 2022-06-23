#include <jni.h>
#include <string>
#include "AccelarateDraw.h"
#include <android/bitmap.h>
#include "SkiaLog.h"

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_com_example_view_AccelerateApi_accelerateInit(JNIEnv *env, jobject obj, jint w, jint h, jint sys) {
    AcclarateDraw::GetInstance()->Init(w, h, sys);
}


JNIEXPORT void JNICALL Java_com_example_view_AccelerateApi_accelerateDeInit(JNIEnv *env, jobject obj) {
    AcclarateDraw::DestroyInstance();
}

JNIEXPORT void JNICALL Java_com_example_view_AccelerateApi_startAccelerateDraw(JNIEnv *env, jobject obj) {
    AcclarateDraw::GetInstance()->StartAccelerateDraw();
}

JNIEXPORT void JNICALL Java_com_example_view_AccelerateApi_refreshAccelerateDraw(JNIEnv *env, jobject obj, jint x, jint y, jint w, jint h, jobject bitmap) {
    assert(env);
    assert(bitmap);
    AndroidBitmapInfo info;
    int *pixel = nullptr;
    if (AndroidBitmap_getInfo(env, bitmap, &info) == 0) {
        INFO("AndroidBitmap_getInfo format = %d w = %d h = %d", info.format, info.width, info.height);
        if (AndroidBitmap_lockPixels(env, bitmap, (void **) &pixel) == 0) {
            AcclarateDraw::GetInstance()->RefreshAccelerateDraw(x, y, w, h, (uint8_t *)pixel);
            AndroidBitmap_unlockPixels(env, bitmap);
        } else {
            ERR("AndroidBitmap_lockPixels Error");
        }
    }
}

JNIEXPORT void JNICALL Java_com_example_view_AccelerateApi_stopAccelerateDraw(JNIEnv *env, jobject obj)
{
    AcclarateDraw::GetInstance()->StopAccelerateDraw();
}
#ifdef __cplusplus
}
#endif

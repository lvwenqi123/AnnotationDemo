package com.example.view;

import android.graphics.Bitmap;

public class AccelerateApi {
    static {
        System.loadLibrary("native-lib");
    }

    public native void accelerateInit(int w, int h, int sys);

    public native void accelerateDeInit();

    public native void startAccelerateDraw();

    public native void refreshAccelerateDraw(int x, int y, int w, int h, Bitmap bitmap);

    public native void stopAccelerateDraw();
}

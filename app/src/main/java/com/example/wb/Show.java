package com.example.wb;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.view.Surface;

public class Show {
    public Surface sur;
    public Canvas c;
    public Bitmap bmp;
    Show(Canvas c, Bitmap bmp) {
        this.c = c;
        this.bmp = bmp;
    }
    void UpdateSurface(Surface sur) {this.sur = sur;}
}

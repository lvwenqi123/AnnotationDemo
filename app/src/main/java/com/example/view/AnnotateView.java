package com.example.view;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import android.view.View;

import com.example.annotateimages.MainActivity;
import com.example.log.LogUtil;
import com.example.wb.WhiteBoard;

public class AnnotateView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {
    private boolean init = false;
    private WhiteBoard wb;
    private MainActivity main = null;
    public AnnotateView(Context context) {
        super(context);
    }

    public AnnotateView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().addCallback(this);
        main = (MainActivity) context;
        setZOrderOnTop(true);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
    }

    public AnnotateView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        setZOrderOnTop(true);
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        getHolder().addCallback(this);
        main = (MainActivity) context;
    }

    public AnnotateView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (!init) {
            Init(getWidth(), getHeight(), holder.getSurface());
        } else {
            wb.UpdateSurface(holder.getSurface());
        }
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        return false;
    }

    @Override
    public boolean onTouchEvent(final  MotionEvent event) {
        if (wb != null) {
            return wb.onTouchEvent(event);
        }
        return true;
    }

    void Init(int w, int h, Surface surface) {
        if (!init) {
            wb = new WhiteBoard(w, h, surface, main);
            wb.SetColor(Color.WHITE);
            wb.SetStrokeWidth(3);
            wb.Preview();
            init = true;
        }
    }

    public WhiteBoard GetWB() {return wb;}
}

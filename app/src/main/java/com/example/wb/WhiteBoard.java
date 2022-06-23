package com.example.wb;

import android.graphics.Bitmap;
import android.graphics.BlendMode;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.util.Log;
import android.util.SparseArray;
import android.view.CollapsibleActionView;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.example.annotateimages.MainActivity;
import com.example.log.LogUtil;
import com.example.view.AccelerateApi;

import java.util.ArrayList;

public class WhiteBoard {
    private int w, h;
    private Surface surface;
    private int color = Color.RED;
    private float strokeWidth = 20;
    private int penType = 0;
    private String tag = "cg";
    public int id = 0;

    private SparseArray<IDrawer> pencilList    = new SparseArray<>();
    private AEraser fingerEraser;
    private AEraser gestureEraser;
    private Paint paint;
    private int preEraserPointId = -1;

    private Bitmap bgBitmap     = null;
    private Bitmap drawBitmap   = null;

    private Canvas drawCanvas   = null;//书写画布

    private ArrayList<Object> undoList = new ArrayList<>();
    private ArrayList<Object> redoList = new ArrayList<>();
    private AccelerateApi ac = new AccelerateApi();
    private Rect screenRect     = new Rect();
    private MainActivity main;
    private int mode;

    public WhiteBoard(int w, int h, Surface surface, MainActivity main) {
        this.w = w;
        this.h = h;
        this.main = main;
        this.surface = surface;
        bgBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888, true);
        //bgBitmap.eraseColor(Color.parseColor(Configure.colorStr2));
        bgBitmap.eraseColor(Color.TRANSPARENT);
        drawBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888, true);
        drawBitmap.eraseColor(Color.TRANSPARENT);
        drawCanvas = new Canvas(drawBitmap);
        //drawCanvas.drawColor(Color.parseColor(Configure.colorStr2));
        LoadBg();
        toCreatePaint();
        int sys = 1;
        ac.accelerateInit(w, h, sys);
        screenRect.set(0, 0, w, h);
    }

    private void clearAllDrawer(){
        fingerEraser  = null;
        gestureEraser = null;
        pencilList.clear();
    }

    private IDrawer getDrawer(int pointId){
        if(Util.getMode() == Util.ERASER) {
            return fingerEraser;
        } else if(Util.getMode() == Util.GESTURE) {
            return gestureEraser;
        } else {
            return pencilList.get(pointId);
        }
    }

    private void toCreatePaint() {
        paint = new Paint();
        int color  = Color.BLUE;
        float size = 3.0f;
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeJoin(Paint.Join.ROUND);
        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setColor(color);
        paint.setAntiAlias(true);
        paint.setStrokeWidth(size);
    }

    Canvas getDrawCanvas() {return drawCanvas;}
    Bitmap getBgBitmap() {return bgBitmap;}

    //刷新书写内容
    public void refreshCache(Rect rect){
//        Log.d(TAG,"refreshCache");
        if(rect != null) {
            /*针对多窗添加区域判断*/
            int width=rect.width();
            if( rect.right > Util.OVERRIDE_SCREEN_WIDTH ) width=Util.OVERRIDE_SCREEN_WIDTH-rect.left;

            //绘制之前，将背景和书写图层的区域依次叠加到刷新图层
            //mCacheCanvas.drawBitmap(mBgBitmap, rect, rect, null);
            //mCacheCanvas.drawBitmap(mDrawBitmap, rect, rect, null);
            //mAcd.refreshAccelerateDraw(rect.left, rect.top, width, rect.height(), mCacheBitmap);
//            LogUtil.i("l = " + rect.left + " t = " + rect.top + " width = " + width + " h = " + rect.height());
//            Canvas c = surface.lockCanvas(rect);
//            c.drawBitmap (drawBitmap, rect, rect, null);
//            surface.unlockCanvasAndPost(c);
            //LogUtil.e("fb flush");
            ac.refreshAccelerateDraw(rect.left, rect.top, width, rect.height(), drawBitmap);
        }
    }

    void DrawBg(String colorStr, Canvas c) {
        c.drawColor(Color.parseColor(colorStr));
    }

    public boolean onTouchEvent(final  MotionEvent event) {
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                MotionEventTouchDown(event);
                break;
            case MotionEvent.ACTION_MOVE:
                MotionEventTouchMove(event);
                break;
            case MotionEvent.ACTION_POINTER_UP:
                MotionEventTouchUp(event, false);
                break;
            case MotionEvent.ACTION_UP:
                MotionEventTouchUp(event, true);
                break;
        }
        return true;
    }

    public void Preview() {
        ReDraw();
    }
    public void UpdateSurface(Surface surface) {this.surface = surface;}
    public void ClearScreen() {
        redoList.clear();
        undoList.clear();
        SetMode(Util.PEN);
        LoadBg();
        Preview();
    }


    private void MotionEventTouchDown(MotionEvent event) {
        int curPointerIndex   = event.getActionIndex();
        int curPointerId      = event.getPointerId(curPointerIndex);
        float x               = event.getX(curPointerIndex);
        float y               = event.getY(curPointerIndex);
        float width = 0;

        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            width = event.getTouchMajor();
            ac.startAccelerateDraw();
        }
        /*
        //TODO: PLAN A, 隐藏悬浮窗
        if(drawSkipBoundsRect != null && !drawSkipBoundsRect.isEmpty()){
            Log.d(TAG,"touchDown refreshCache by drawSkipBoundsRect");
            refreshCache(drawSkipBoundsRect);
        }*/
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN || Util.getMode() == Util.PEN) {
            touchDown(curPointerId, x, y, width);
        }
    }

    //开始加速
    private void touchDown(int curPointerId, float x, float y, float w) {
        IDrawer drawer = toCreateDrawer(curPointerId, w);
        drawer.touchDown(x, y);
    }

    private IDrawer toCreateDrawer(int pointId, float width){
        IDrawer drawer = null;
        if(Util.getMode() == Util.ERASER) {
            if(fingerEraser == null){
                fingerEraser = new FingerEraser(paint);
                fingerEraser.setEraserWidthHeight(150, 150);
            }
            drawer = fingerEraser;
        } else if(width >= Util.GestureEraseThound) {
            if(gestureEraser == null){
                gestureEraser = new GestureEraser(paint);
                gestureEraser.setEraserWidthHeight((int)width * 3, (int)width * 3);
                preEraserPointId = pointId;
                Util.setMode(Util.GESTURE);
            }
            drawer = gestureEraser;
        } else {
            drawer = new Pencil(paint);
            pencilList.append(pointId,drawer);
            Util.setMode(Util.PEN);
        }
        return drawer;
    }

    private void MotionEventTouchMove(MotionEvent event) {
        int pointerCount = event.getPointerCount();
        int curPointerIndex = 0;
        for (int i = 0; i < pointerCount; i++) {
            if(Util.getMode() == Util.PEN || preEraserPointId == event.getPointerId(i)){
                curPointerIndex = i;
            } else {
                curPointerIndex = 0;
            }
            paintDraw(event.getPointerId(curPointerIndex), event.getX(curPointerIndex), event.getY(curPointerIndex));
        }
    }

    private void touchMove(int curPointerId, float x, float y) {
        paintDraw(curPointerId, x, y);
    }
    private void paintDraw(int curPointerId, float x, float y) {
        IDrawer drawer = getDrawer(curPointerId);
        if (drawer != null && drawer.touchMove(x, y)) {
            drawer.draw(this, true);
        }
    }

    private void MotionEventTouchUp(MotionEvent event, boolean releaseAll) {
        int curPointerIndex = 0;
        if(Util.getMode() == Util.PEN || preEraserPointId == event.getPointerId(event.getActionIndex())){
            curPointerIndex = event.getActionIndex();
        }else{
            curPointerIndex = 0;
        }
        touchUp(event.getPointerId(curPointerIndex), event.getX(curPointerIndex), event.getY(curPointerIndex), releaseAll);
    }

    private void touchUp(int curPointerId, float x, float y, boolean releaseAll) {
        if (Util.getMode() == Util.PEN) {
            paintUp(curPointerId, x, y);
        } else {
            IDrawer drawer = null;
            if (Util.getMode() == Util.GESTURE) {
                drawer = gestureEraser;
            } else if (Util.getMode() == Util.ERASER) {
                drawer = fingerEraser;
            }
            if (drawer != null) {
                drawer.draw(this, false);
            }
        }

            //刷新总个界面
            if (releaseAll) {
                preEraserPointId = -1;
                //requestCacheDraw();
                clearAllDrawer();
                main.ChangeButtonAlpha();
                //mAcd.stopAccelerateDraw();
                requestCacheDraw();
                requestCacheDraw();
            }
    }

    private void paintUp(int curPointerId, float x, float y) {
        IDrawer drawer = getDrawer(curPointerId);
        if (drawer != null) {
            if(drawer.touchUp(x, y)) {
                drawer.draw(this, true);
            }
            if(Util.getMode() == Util.PEN) {
                pencilList.remove(curPointerId);
            }
        }
    }

    public void Undo() {
        if (undoList.size() >0) {
            int len = undoList.size() - 1;
            Object obj = undoList.get(len);
            undoList.remove(len);
            redoList.add(obj);
            Preview();
        }
    }

    public void Redo() {
        if (redoList.size() >0) {
            int idx = redoList.size() - 1;
            Object obj = redoList.get(idx);
            redoList.remove(idx);
            undoList.add(obj);
            Preview();
        }
    }

    //刷新总个屏幕
    public void requestCacheDraw() {
            Canvas canvas = surface.lockCanvas(null);
            canvas.drawBitmap(drawBitmap, 0, 0, null);
            surface.unlockCanvasAndPost(canvas);
    }

    public void SetColor(int color) {
        this.color = color;
    }
    public void SetStrokeWidth(float strokeWidth) {this.strokeWidth = strokeWidth;}
    public void SetPenType(int penType) {this.penType = penType;}

    void ReDraw() {
        Canvas canvas = surface.lockCanvas(null);
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        surface.unlockCanvasAndPost(canvas);
    }

    public void SetMode(int mode) {
        Util.setMode(mode);
        Preview();
    }

    void LoadBg() {
        drawCanvas.drawBitmap(bgBitmap, 0, 0, null);
    }
}

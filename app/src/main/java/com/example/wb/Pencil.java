package com.example.wb;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;

import com.example.log.LogUtil;
import com.example.wb.IDrawer;
import com.example.wb.WhiteBoard;

public class Pencil implements IDrawer {
    public static final String TAG = Pencil.class.getSimpleName();

    protected Path mCurrPath ;//the current path of this touch
    protected Path mPath;//the total path of this shape
    protected Paint mPaint;

    protected float mX = 0, mY = 0, mCX = 0, mCY = 0;

    Pencil(Paint paint) {
        mPaint = new Paint(paint);
        mPath  = new Path();
        mCurrPath = new Path();
    }

    @Override
    public void draw(WhiteBoard wb, boolean show) {
        if (wb == null) {
            return;
        }

        Canvas drawCanvas   = wb.getDrawCanvas();
        if(drawCanvas == null) {
            return;
        }

        drawCanvas.drawPath(mCurrPath, mPaint);
        wb.refreshCache(getCurrRect());
    }

    @Override
    public void touchDown(float x, float y) {
        mPath.moveTo(x, y);
        mX  = x;
        mY  = y;
        mCX = x;
        mCY = y;
    }
    @Override
    public boolean touchMove(float x, float y) {
        mCurrPath.reset();
        mCurrPath.moveTo(mCX, mCY);
        mCX = (x + mX) / 2;
        mCY = (y + mY) / 2;
        mCurrPath.quadTo(mX, mY, mCX, mCY);
        //ͬʱ��������path
        mPath.quadTo(mX, mY, mCX, mCY);
        mX = x;
        mY = y;
        return true;
    }
    @Override
    public boolean touchUp(float x, float y) {
        return false;
    }
    @Override
    public Rect getCurrRect() {
        float padding = mPaint.getStrokeWidth();
        RectF rectf = new RectF();
        mCurrPath.computeBounds(rectf, true);
        //У������Ƿ񳬳��߽�
        int left = (int) (rectf.left - padding);
        if (left < 0) {
            left = 0;
        }
        int top = (int) (rectf.top - padding);
        if (top < 0) {
            top = 0;
        }
        int right = (int) (rectf.right + padding);
        if (right > Util.SCREEN_WIDTH) {
            right = Util.SCREEN_WIDTH;
        }
        int bottom = (int) (rectf.bottom + padding);
        if (bottom > Util.SCREEN_HEIGHT) {
            bottom = Util.SCREEN_HEIGHT;
        }
        return new Rect(left, top, right, bottom);
    }
}

package com.example.wb;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.Rect;
import android.util.Log;
import android.view.MotionEvent;

public class Painter {
    protected Path path;
    protected Rect r;
    protected PointF start = new PointF();
    protected PointF last = new PointF();
    protected PointF ctrl = new PointF();
    protected float strokeWidth = 3;
    protected int color;
    Paint paint = new Paint();
    Painter() {
        path = new Path();
        path.reset();
        paint.reset();
        paint.setStyle(Paint.Style.FILL);
        paint.setDither(true);
        paint.setAntiAlias(true);
        paint.setStrokeJoin(Paint.Join.ROUND);
        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setStyle(Paint.Style.STROKE);
        r = new Rect();
    }

    void Point2Path(int event, float x, float y, int width, Path record) {
            if (event == MotionEvent.ACTION_DOWN) {
                start.set(x, y);
                last.set(x+1, y+1);
                path.reset();
                path.moveTo(start.x, start.y);
                path.lineTo(last.x, last.y);
                r.set(Tools.Point2Rect(start, last, width));
                record.moveTo(start.x, start.y);
            } else  {
                float midX = (x + last.x) / 2;
                float midY = (y + last.y) / 2;
                path.rewind(); /*只清数据不清内存*/
                path.moveTo(start.x, start.y);
                path.quadTo(last.x, last.y, midX, midY);
                record.quadTo(last.x, last.y, midX, midY);
                r.set(Tools.Point2Rect(start, last, new PointF(midX, midY), width));
                last.set(x, y);
                start.set(midX, midY);
            }
    }

    void Point2Path(int event, float x, float y){}
    void SetPaint(Paint src) {
        this.paint.set(src);
    }

    Path GetPath() {return path;}
    Rect GetRect() {return r;}
}

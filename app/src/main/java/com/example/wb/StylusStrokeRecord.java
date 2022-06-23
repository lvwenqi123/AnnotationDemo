package com.example.wb;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;

import java.util.ArrayList;

public class StylusStrokeRecord{
    protected Path path;
    boolean isBuildPath = false;
    int color;
    float width;
    ArrayList<StylusMeta> stylusMetaVec = new ArrayList<>();
    StylusStrokeRecord(int color, float width) {
        path = new Path();
        path.reset();
    }

    void AddStroke(StylusMeta sm) { stylusMetaVec.add(sm); }

    void InnerStroke(ArrayList<StylusMeta> stylusMetaVec)
    {
        path.setFillType(Path.FillType.WINDING);
        StylusMeta sm0, sm1, sm2, sm3;
        float d01, d12, d23;
        d01 = d12 = d23 = 0;
        if (stylusMetaVec.size() > 0) {
            sm2 = stylusMetaVec.get(0);
            StylusStrokeBuilder.AddCircle(path, sm2);
            if (stylusMetaVec.size() > 1) {
                sm1 = sm2;
                sm2 = stylusMetaVec.get(1);
                d12 = StylusStrokeBuilder.Distance(sm1, sm2);
                if (stylusMetaVec.size() > 2) {
                    sm3 = stylusMetaVec.get(2);
                    d23 = StylusStrokeBuilder.Distance(sm2, sm3);
                    StylusStrokeBuilder.Quad1(path, sm1, sm2, sm3, d12, d23);
                    int size = stylusMetaVec.size();
                    for (int i = 3; i < size; i++) {
                        sm0 = sm1;
                        sm1 = sm2;
                        sm2 = sm3;
                        sm3 = stylusMetaVec.get(i);
                        d01 = d12;
                        d12 = d23;
                        d23 = StylusStrokeBuilder.Distance(sm2, sm3);
                        StylusStrokeBuilder.Cubic(path, sm0, sm1, sm2, sm3, d01, d12, d23);
                    }
                    StylusStrokeBuilder.Quad0(path, sm1, sm2, sm3, d12, d23);
                } else {
                    StylusStrokeBuilder.Line(path, sm1, sm2);
                }
            }
        }
        isBuildPath = true;
    }

    boolean IsBuildPath() {return isBuildPath;}

    Path BuildPath() {
        if (!isBuildPath) {
            InnerStroke(stylusMetaVec);
        }
        return path;
    }

    void Draw(Canvas c, Paint paint)
    {
        if (!isBuildPath) { //防止有时候没有产生up消息
            InnerStroke(stylusMetaVec);
        }
        paint.setStrokeWidth(0);
        paint.setColor(color);
        paint.setStyle(Paint.Style.FILL);
        c.drawPath(path, paint);
    }
};
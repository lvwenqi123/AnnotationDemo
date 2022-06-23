package com.example.wb;

import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;

public class Tools {
    static RectF Point2RectF(PointF p1, PointF p2, float w)
    {
        RectF ret = new RectF();
        ret.left = Math.min(p1.x, p2.x) - w;
        ret.top = Math.min(p1.y, p2.y) - w;
        ret.right = Math.max(p1.x, p2.x) + w;
        ret.bottom = Math.max(p1.y, p2.y) + w;
        return ret;
    }

    static Rect Point2Rect(PointF p1, PointF p2, int w)
    {
        Rect ret = new Rect();
        ret.left = Math.min((int)p1.x, (int)p2.x) - w;
        ret.top = Math.min((int)p1.y, (int)p2.y) - w;
        ret.right = Math.max((int)p1.x, (int)p2.x) + w;
        ret.bottom = Math.max((int)p1.y, (int)p2.y) + w;
        return ret;
    }

    static Rect Point2Rect(PointF p0, PointF p1, PointF p2, int w)
    {
        Rect r = new Rect();
        r.left = (int)Math.min(p0.x, p1.x);
        r.left = (int)Math.min(r.left, p2.x) - w;

        r.right = (int)Math.max(p0.x, p1.x);
        r.right = (int)Math.max(r.right, p2.x) + w;

        r.top = (int)Math.min(p0.y, p1.y);
        r.top = (int)Math.min(r.top, p2.y) - w;

        r.bottom = (int)Math.max(p0.y, p1.y);
        r.bottom = (int)Math.max(r.bottom, p2.y) + w;
        return r;
    }

    static double LineAngle2(PointF p1, PointF p2) {
        //求角度
        double tmp = (p2.y - p1.y) / (p2.x - p1.x);
        double deg = Math.atan(tmp);
        return (deg * (double) 180 / Math.PI);
    }

    public static double GetDistance1(float x0, float y0, float x1, float y1) {
        return  Math.hypot((x1 - x0), (y1 - y0));
    }

    public static double GetDistance2(PointF p1, PointF p2) {
        return  GetDistance1(p1.x, p1.y, p2.x, p2.y);
    }

    /*检查该点是否在该圆形内*/
    public static boolean IsInCircle(float x, float y, float cx, float cy, float radius) {
        float newRadius  = (float)Tools.GetDistance1(x, y, cx, cy);
        if (newRadius < radius) {
            return true;
        }
        return false;
    }

    public static  boolean IsContains(RectF r, float x, float y) {
        return !(x < r.left || y < r.top || x > r.right || y > r.bottom);
    }

    public static  boolean IsContains(Rect r, float x, float y) {
        return !(x < r.left || y < r.top || x > r.right || y > r.bottom);
    }

    public static Rect ConverRect(Rect src) {
        Rect res = new Rect();
        res.left = Math.min(src.left, src.right);
        res.right = Math.max(src.left, src.right);
        res.top = Math.min(src.top, src.bottom);
        res.bottom = Math.max(src.top, src.bottom);
        return res;
    }

    public static PointF GetAverage(PointF p1, PointF p2) {
        return new PointF((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f);
    }

    public static void SinglePoint2Rect(Rect r, int x, int y, int w, int h) {
        r.left = x - w / 2;
        r.top = y - h / 2;
        r.right = x + w / 2;
        r.bottom = y + h / 2;
    }

    static void MergeRect(Rect dst, Rect src, int w, int h) //�ϲ�����
    {
        dst.left = Math.min(dst.left, src.left);
        dst.right = Math.max(dst.right, src.right);
        dst.top = Math.min(dst.top, src.top);
        dst.bottom = Math.max(dst.bottom, src.bottom);
        dst.left -= w;
        dst.right += w;
        dst.top -= h;
        dst.bottom += h;
    }
}

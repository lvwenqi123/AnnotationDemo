package com.example.wb;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.RectF;
import android.util.Log;
import android.view.MotionEvent;

//笔划渐变
public class GradientPainter extends Painter{
    PointF pv[] = new PointF[6];
    float lastAnglesAverage[] = new float[2];
    PointF lastPoints[] = new PointF[2];
    float lastLen;
    float realStrokeWidth;
    final static float m_realK = 0.2f;
    final static float m_realVC = 0.2f;
    Canvas cvs;

    GradientPainter(Canvas c, int color , float strokeWidth) {
        for (int i = 0 ; i < pv.length; ++i) {
            pv[i] = new PointF(0,0);
        }

        for (int i = 0; i < lastPoints.length ; ++i) {
            lastPoints[i] = new PointF(0,0);
        }
        lastLen = 0;
        realStrokeWidth = 0;
        cvs = c;
        this.color = color;
        this.strokeWidth = strokeWidth;
        Log.i("cg", "sw = " + strokeWidth);
        paint.setColor(this.color);
    }

    void Point2Path(int event, float x, float y) {
        if (event == MotionEvent.ACTION_DOWN) {
            InitPt(x ,y);
            cvs.drawPoint(x, y, paint);
            PointF p1 = new PointF(x, y);
            PointF p2 = new PointF(x+1, y+1);
            r.set(Tools.Point2Rect(p1, p2, (int)strokeWidth));
        } else if (event == MotionEvent.ACTION_UP) {
            float realStrokeWidth1 = realStrokeWidth / 5.0f;//2021.07.30
            if (realStrokeWidth1 > 1.1)  realStrokeWidth1 = 1.1f;
            PointF cur = new PointF(x ,y);
            PointF endPoint = GetFilterPt(x ,y);
            paint.setStrokeWidth(realStrokeWidth - realStrokeWidth1);
            Path path = new Path();
            path.reset();
            PointF start = Tools.GetAverage(lastPoints[0], lastPoints[1]);
            path.moveTo(start.x, start.y);
            PointF c1 = lastPoints[1];
            PointF end = Tools.GetAverage(lastPoints[1], endPoint);
            //画第一段线
            path.quadTo(c1.x, c1.y, end.x, end.y);
            cvs.drawPath(path, paint);

            path.reset();
            paint.setStrokeWidth(realStrokeWidth - realStrokeWidth1 * 2.0f);

            start = Tools.GetAverage(lastPoints[1], endPoint);
            c1 = endPoint;
            end = Tools.GetAverage(endPoint, cur);

            //画第二段线
            path.moveTo(start.x, start.y);
            path.quadTo(c1.x, c1.y, end.x, end.y);
            cvs.drawPath(path, paint);

            end = Tools.GetAverage(endPoint, cur);
            end = Tools.GetAverage(end, cur);

            paint.setStrokeWidth(realStrokeWidth - realStrokeWidth1 * 2.8f);//4
            start = Tools.GetAverage(endPoint, cur);

            cvs.drawLine(start.x, start.y, end.x, end.y, paint);

            paint.setStrokeWidth(realStrokeWidth - realStrokeWidth1 * 3.5f);//4
            cvs.drawLine(start.x, start.y, cur.x, cur.y, paint);

            r.set(Tools.Point2Rect(lastPoints[1], lastPoints[0], cur, (int)strokeWidth));
            r.inset((int)-strokeWidth, (int)-strokeWidth);
        } else {
            PointF last = GetLastPt();
            PointF cur = new PointF(x ,y);
            if (Tools.GetDistance2(last,cur) <= 2) {
                return;
            }

            PointF endPoint = GetFilterPt(x ,y); /*取5个点，求取平均值*/
            PointF lastP1 = lastPoints[1];
            float lv = (float)Tools.GetDistance2(lastP1, endPoint);

            float angle = (float)Tools.LineAngle2(lastP1, endPoint);
            float angle90 = (short)angle % 90;
            float angle45 = angle90;
            if(angle90 > 45) angle45 = 90 - angle90;
            //取三次数值做平均数
            float angle45Average = (angle45 + lastAnglesAverage[0] + lastAnglesAverage[1]) / 3;

            realStrokeWidth = strokeWidth - (strokeWidth * m_realK) * (angle45Average / 45);//3
            lv = (lv + lastLen) / 2 ;

            float t_w = realStrokeWidth;
            realStrokeWidth = (float) (m_realVC * t_w + (1.0 - m_realVC) * t_w * (2.0 / (lv+2.0)));//2021.07.13
            lastLen = lv;
            float startx = (float) ((lastPoints[1].x + lastPoints[0].x) * 0.5);
            float starty = (float) ((lastPoints[1].y + lastPoints[0].y) * 0.5);
            Path path = new Path();
            path.reset();
            path.moveTo(startx, starty);
            PointF c1 = lastPoints[1];
            float endx = (float) ((lastPoints[1].x + endPoint.x) * 0.5);
            float endy = (float) ((lastPoints[1].y + endPoint.y) * 0.5);

            path.quadTo(c1.x, c1.y, endx, endy);
            paint.setStrokeWidth(realStrokeWidth);
            cvs.drawPath(path, paint);
            lastPoints[0].set(lastPoints[1]);
            lastPoints[1].set(endPoint);

            lastAnglesAverage[0] = lastAnglesAverage[1];
            lastAnglesAverage[1] = angle45Average;
            RectF bound = new RectF();
            path.computeBounds(bound, false);
            r.set((int)bound.left, (int)bound.top, (int)bound.right, (int)bound.bottom);
            r.inset((int)-strokeWidth, (int)-strokeWidth);
        }
    }

    void InitPt(float x, float y) {
        for (int i = 0 ; i < pv.length; ++i) {
            pv[i].set(x ,y);
        }
        for (int i = 0 ; i < lastPoints.length; ++i) {
            lastPoints[i].set(pv[0]);
        }
        lastAnglesAverage[0] = lastAnglesAverage[1] = 45;
        lastLen = 100;
        path.reset();
    }

    PointF GetFilterPt(float x, float y) {
        pv[5].set(x ,y);
        float xx = x, yy = y;
        for (int i = 0 ; i < 4; ++i) {
            xx += pv[i].x;
            yy += pv[i].y;
        }
        pv[4].set(xx /5, yy / 5);
        pv[0].set(pv[1]);
        pv[1].set(pv[2]);
        pv[2].set(pv[3]);
        pv[3].set(x ,y);
        return pv[4];
    }

    boolean IsSamept(float x, float y) {
        if (pv[5].x == x && pv[5].y == y) {
            return true;
        }
        return false;
    }
    PointF GetLastPt() {return pv[5];}
}

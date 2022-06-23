//
// Created by iboard on 2022/2/18.
//

#ifndef DX_COMMON_FUNCUTILS_H
#define DX_COMMON_FUNCUTILS_H
#include <SkPath.h>
#include <SkFont.h>
#include <sstream>

static const float DefaultCoordinateInterval = 80; //每一大格子坐标之间间距这么远
//static float CoordinateIntervalMin = CoordinateIntervalMax / 5; //每一小格坐标之间间距
struct FuncUtils{
    static SkPath gridPath(SkRect bound, SkPoint &coo, float CoordinateIntervalMax) {
        SkPath path;
        path.reset();
        //x负轴
        float interval = CoordinateIntervalMax / 5;
        for (int x = coo.fX - interval; x > bound.left(); x -= interval) {
            path.moveTo(x, bound.top());
            path.lineTo(x, bound.bottom());
        }
        //x正轴
        for (int x = coo.fX + interval; x < bound.right(); x += interval) {
            path.moveTo(x, bound.top());
            path.lineTo(x, bound.bottom());
        }

        //y正轴
        for (int y = coo.fY - interval; y >  bound.top(); y -= interval) {
            path.moveTo(bound.left(), y);
            path.lineTo(bound.right(), y);
        }
//        //y负轴
        for (int y = coo.fY; y < bound.bottom(); y += interval) {
            path.moveTo(bound.left(), y);
            path.lineTo(bound.right(), y);
        }
        return path;
    }

    /*坐标点所在的线*/
   static SkPath gridPath1(SkRect bound, SkPoint coo, float CoordinateIntervalMax) {

        SkPath path;
        path.reset();
        //x负轴
        int count = (int) ((coo.fX - bound.left()) / CoordinateIntervalMax);
        for (int i = 1; i <= count; i++) {
            path.moveTo(coo.fX - CoordinateIntervalMax * i, bound.top());
            path.lineTo(coo.fX - CoordinateIntervalMax * i, bound.bottom());
        }

        //x正轴
        for (int i = 1; i <= count; i++) {
            path.moveTo(coo.fX + CoordinateIntervalMax * i, bound.top());
            path.lineTo(coo.fX + CoordinateIntervalMax * i, bound.bottom());
        }

        //y正轴
        count = (coo.fY - bound.top()) / CoordinateIntervalMax;
        for (int i = 1; i <= count; i++) {
            path.moveTo(bound.left(), coo.fY - CoordinateIntervalMax * i);
            path.lineTo(bound.right(), coo.fY - CoordinateIntervalMax * i);
        }

        //y负轴
        for (int i = 1; i <=count; i++) {
            path.moveTo(bound.left(), coo.fY + CoordinateIntervalMax * i);
            path.lineTo(bound.right(), coo.fY + CoordinateIntervalMax * i);
        }
        return path;
    }
//画坐标轴所在的线
    static SkPath cooPath(SkRect &bound, SkPoint coo) {
        SkPath path;
        path.reset();
        //x轴线
        path.moveTo(bound.left(), coo.fY);
        path.lineTo(bound.right() - 10, coo.fY);
        //y轴线
        path.moveTo(coo.fX, bound.top() + 10);
        path.lineTo(coo.fX, bound.bottom());
        return path;
    }
    //画坐标轴上的两个小箭头
     static SkPath arrowsPath(SkRect &bound, SkPoint &coo) {
        SkPath path;
        path.reset();
//x轴的箭头
        path.moveTo(bound.right(), coo.fY);
        path.lineTo(bound.right() - 10, coo.fY - 10);
        path.lineTo(bound.right() - 10, coo.fY + 10);
        path.close();
//y轴的箭头
        path.moveTo(coo.fX, bound.top());
        path.lineTo(coo.fX - 10, bound.top()+10);
        path.lineTo(coo.fX + 10, bound.top()+10);
        path.close();
        return path;
    }
    static void drawText4Coo(SkCanvas *c, SkRect &bound, SkPoint coo, SkPaint &paint, float CoordinateIntervalMax) {
        //绘制文字
        SkFont font;
        font = font.makeWithSize(30);
        c->drawString("x", bound.right() - 20, coo.fY - 15, font, paint);
        c->drawString("y", coo.fX + 20, bound.top() + 18, font, paint);
        font = font.makeWithSize(15);
        //X正轴文字
        int count = (int) ((bound.right() - coo.fX) / CoordinateIntervalMax);
        c->drawString("0", coo.fX + 10, coo.fY - 5, font, paint);
        for (int i = 1; i <= count; i++) {
            SkString text;
            text.appendS32(5 * i);
            c->drawString(text, coo.fX + CoordinateIntervalMax * i + 5, coo.fY - 5, font, paint);
        }

        //X负轴文字
        count = (int) ((coo.fX - bound.left()) / CoordinateIntervalMax);
        for (int i = 1; i <= count; i++) {
            SkString text;
            text.appendS32(-5 * i);
            c->drawString(text, coo.fX - CoordinateIntervalMax * i + 5, coo.fY - 5, font, paint);
        }

        //y正轴文字
        count = (int) ((coo.fY - bound.top()) / CoordinateIntervalMax);
        for (int i = 1; i <= count; i++) {
            SkString text;
            text.appendS32(5 * i);
            c->drawString(text, coo.fX + 10, coo.fY - CoordinateIntervalMax * i, font, paint);
        }
        //y负轴文字
        count = (int) ((bound.bottom() - coo.fY) / CoordinateIntervalMax);
        for (int i = 1; i <= count; i++) {
            SkString text;
            text.appendS32(-5 * i);
            c->drawString(text, coo.fX + 10, coo.fY + CoordinateIntervalMax * i, font, paint);
        }
    }

    //判断是否数字
    static bool IsNumber(char a) {
        if (a >= '0' && a <= '9') {
            return true;
        }
        return false;
    }
    //判断是否负号
    static bool IsNegative(char a) {
        if (a == '-') {
            return true;
        }
        return false;
    }

    static std::string GetFuncStr(float a, float b, float c, int type) {
       if (type == 0) {
           std::stringstream ss;
           ss << "y = ";
           if (a == 1) {
               ss << "x";
           } else {
               ss << a << "x";
           }
           if (b > 0) {
               ss << "+" << b;
           } else if (b < 0) {
               ss << b;
           }
           return ss.str();
       } else {
           std::stringstream ss;
           ss << "y = ";
           if (a == -1) {
               ss << "-";
           }
           if (a > 1 || a < -1) {
               ss << a;
           }
           ss << "x²"; //

           if (b == -1) {
               ss << " -x";
           } else if (b == 1) {
               ss << " +x";
           } else if (b != 0) {
               if (b >0) {
                   ss << " + " << b << "x";
               } else {
                   ss << " " << b << "x";
               }
           }

           if (c >0) {
               ss << " + " << c;
           } else if (c < 0) {
               ss << " " << c;
           }
           return ss.str();
       }
       return "";
   }
};

#endif //DX_COMMON_SHAPEUTILS_H

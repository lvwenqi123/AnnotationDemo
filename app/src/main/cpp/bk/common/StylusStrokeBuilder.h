//
// Created by TEST on 2019/12/4.
//

#ifndef TEST_STYLUSSTROKEBUILDER_H
#define TEST_STYLUSSTROKEBUILDER_H
#include <SkPath.h>
#include "common.h"
#include <math.h>
/*手写笔锋建造*/

struct StylusStrokeBuilder{
    static const int CubicInterpolationCount = 5;
    static const int QuadInterpolationCount = 5;

    static float Distance(StylusMeta &sm0, StylusMeta &sm1) {
        return  (float)hypot(sm0.x - sm1.x, sm0.y - sm1.y);
    }
    static SkPath* LinkCircle(float x0, float y0, float r0, float x1, float y1, float r1) {
        float lx = x1 - x0;
        float ly = y1 - y0;
        float ll = lx * lx + ly * ly;
        float m = ll - (r0 - r1) * (r0 - r1);
        if (m <= 0) {
            return nullptr;
        }
        m = (float) sqrt(m);
        float k = (r1 - r0) / ll;
        float h = m / ll;

        SkPath *trapezoid = new SkPath();
        trapezoid->reset();
        trapezoid->moveTo(x0 - r0 * (k * lx - h * ly), y0 - r0 * (k * ly - h * -lx));
        trapezoid->lineTo(x0 - r0 * (k * lx + h * ly), y0 - r0 * (k * ly + h * -lx));
        trapezoid->lineTo(x1 - r1 * (k * lx + h * ly), y1 - r1 * (k * ly + h * -lx));
        trapezoid->lineTo(x1 - r1 * (k * lx - h * ly), y1 - r1 * (k * ly - h * -lx));
        trapezoid->close();
        return trapezoid;
    }

    static void LinkCircle(SkPath &path, float x0, float y0, float r0, float x1, float y1, float r1,  SkPath::Direction direction) {
        float lx = x1 - x0;
        float ly = y1 - y0;
        float ll = lx * lx + ly * ly;
        float m = ll - (r0 - r1) * (r0 - r1);
        if (m <= 0) {
            return;
        }
        m = (float) sqrt(m);
        float k = (r1 - r0) / ll;
        float h = m / ll;

        switch (direction) {
            case SkPath::kCW_Direction:
                path.moveTo(x0 - r0 * (k * lx - h * ly), y0 - r0 * (k * ly - h * -lx));
                path.lineTo(x1 - r1 * (k * lx - h * ly), y1 - r1 * (k * ly - h * -lx));
                path.lineTo(x1 - r1 * (k * lx + h * ly), y1 - r1 * (k * ly + h * -lx));
                path.lineTo(x0 - r0 * (k * lx + h * ly), y0 - r0 * (k * ly + h * -lx));
                path.close();
                break;
            case SkPath::kCCW_Direction:
                path.moveTo(x0 - r0 * (k * lx - h * ly), y0 - r0 * (k * ly - h * -lx));
                path.lineTo(x0 - r0 * (k * lx + h * ly), y0 - r0 * (k * ly + h * -lx));
                path.lineTo(x1 - r1 * (k * lx + h * ly), y1 - r1 * (k * ly + h * -lx));
                path.lineTo(x1 - r1 * (k * lx - h * ly), y1 - r1 * (k * ly - h * -lx));
                path.close();
                break;
        }
    }

    static void AddCircle(SkPath &path, StylusMeta &circle)
    {
        path.addCircle(circle.x, circle.y, circle.radius, SkPath::kCCW_Direction);
    }

    static void AddLine(SkPath &path, float x0, float y0, float r0, float x1, float y1, float r1) {
        LinkCircle(path, x0, y0, r0, x1, y1, r1, SkPath::kCCW_Direction);
        path.addCircle(x1, y1, r1, SkPath::kCCW_Direction);
    }

    static void Line(SkPath &path, StylusMeta &from, StylusMeta &to) {
        AddLine(path, from.x, from.y, from.radius, to.x, to.y, to.radius);
    }

    static void Quad1(SkPath &path, StylusMeta &from, StylusMeta &to, StylusMeta &post, float d12, float d23)
    {
        float x0, y0, r0, x1, y1, r1, x2, y2, r2;
        x0 = from.x;
        y0 = from.y;
        r0 = from.radius;
        x1 = to.x - (post.x - from.x) / 2 * d12 / (d12 + d23);
        y1 = to.y - (post.y - from.y) / 2 * d12 / (d12 + d23);
        r1 = to.radius - (post.radius - from.radius) / 2 * d12 / (d12 + d23);
        x2 = to.x;
        y2 = to.y;
        r2 = to.radius;

        float sx, sy, sr, dx, dy, dr;
        sx = from.x;
        sy = from.y;
        sr = from.radius;
        for (int i = 0; i < QuadInterpolationCount; i++) {
            float t = (float) (i + 1) / QuadInterpolationCount;
            dx = (1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2;
            dy = (1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2;
            dr = (1 - t) * (1 - t) * r0 + 2 * (1 - t) * t * r1 + t * t * r2;
            AddLine(path, sx, sy, sr, dx, dy, dr);
            sx = dx;
            sy = dy;
            sr = dr;
        }
    }

    static void Quad0(SkPath &path, StylusMeta &pre, StylusMeta &from, StylusMeta &to, float d12, float d23)
    {
        float x0, y0, r0, x1, y1, r1, x2, y2, r2;
        x0 = from.x;
        y0 = from.y;
        r0 = from.radius;
        x1 = from.x + (to.x - pre.x) / 2 * d23 / (d12 + d23);
        y1 = from.y + (to.y - pre.y) / 2 * d23 / (d12 + d23);
        r1 = from.radius + (to.radius - pre.radius) / 2 * d23 / (d12 + d23);
        x2 = to.x;
        y2 = to.y;
        r2 = to.radius;

        float sx, sy, sr, dx, dy, dr;
        sx = from.x;
        sy = from.y;
        sr = from.radius;
        for (int i = 0; i < QuadInterpolationCount; i++) {
            float t = (float) (i + 1) / QuadInterpolationCount;
            dx = (1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2;
            dy = (1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2;
            dr = (1 - t) * (1 - t) * r0 + 2 * (1 - t) * t * r1 + t * t * r2;
            AddLine(path, sx, sy, sr, dx, dy, dr);
            sx = dx;
            sy = dy;
            sr = dr;
        }
    }

    static void Cubic(SkPath &path, StylusMeta &pre, StylusMeta &from, StylusMeta &to, StylusMeta &post, float d01, float d12, float d23) {
        float x0, y0, r0, x1, y1, r1, x2, y2, r2, x3, y3, r3;

        x0 = from.x;
        y0 = from.y;
        r0 = from.radius;
        x1 = from.x + (to.x - pre.x) / 3 * d12 / (d01 + d12);
        y1 = from.y + (to.y - pre.y) / 3 * d12 / (d01 + d12);
        r1 = from.radius + (to.radius - pre.radius) / 3 * d12 / (d01 + d12);
        x2 = to.x - (post.x - from.x) / 3 * d12 / (d12 + d23);
        y2 = to.y - (post.y - from.y) / 3 * d12 / (d12 + d23);
        r2 = to.radius - (post.radius - from.radius) / 3 * d12 / (d12 + d23);
        x3 = to.x;
        y3 = to.y;
        r3 = to.radius;

        float sx, sy, sr, dx, dy, dr;
        sx = from.x;
        sy = from.y;
        sr = from.radius;
        for (int i = 0; i < CubicInterpolationCount; i++) {
            float t = (float) (i + 1) / CubicInterpolationCount;

            dx = (1 - t) * (1 - t) * (1 - t) * x0 + 3 * (1 - t) * (1 - t) * t * x1 + 3 * (1 - t) * t * t * x2 + t * t * t * x3;
            dy = (1 - t) * (1 - t) * (1 - t) * y0 + 3 * (1 - t) * (1 - t) * t * y1 + 3 * (1 - t) * t * t * y2 + t * t * t * y3;
            dr = (1 - t) * (1 - t) * (1 - t) * r0 + 3 * (1 - t) * (1 - t) * t * r1 + 3 * (1 - t) * t * t * r2 + t * t * t * r3;
            AddLine(path, sx, sy, sr, dx, dy, dr);
            sx = dx;
            sy = dy;
            sr = dr;
        }
    }
};


#endif //TEST_STYLUSSTROKEBUILDER_H

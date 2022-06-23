/*
 * Tools.h
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <algorithm>
#include <SkRect.h>
#include "SkiaLog.h"
#include "common.h"
#include "PixelFormat.h"
#define PI 3.1415926
const int _zh_char_len = 3; //中文字符长度
const int _en_char_len = 1;
const int _ru_char_len = 2; //俄语字符长度

typedef struct _BuildCylinderParam{
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;
	float k = 0.5522848;
	float ox = 0;
	float oy = 0;
	float xe = 0;
	float ye = 0;
	float xm = 0;
	float ym = 0;

	float x0 = 0, y0 = ym;
	float x1 = 0, y1 = ym - oy;
	float x2 = xm - ox, y2 = 0;
	float x3 = xm, y3 = 0;
	_BuildCylinderParam() {

	}
	void Init(SkRect &oval) {
		x = oval.fLeft;
		y = oval.fTop;
		w = oval.width();
		h = oval.height();
		k = 0.5522848;
		ox = (w / 2) * k;
		oy = (h / 2) * k;
		xe = w;
		ye = h;
		xm = w / 2;
		ym = h / 2;
		x0 = 0, y0 = ym;
		x1 = 0, y1 = ym - oy;
		x2 = xm - ox, y2 = 0;
		x3 = xm, y3 = 0;
	}

	void BuildAll(SkPath &path) {
		path.moveTo(x + x0, y + y0);
		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);
	}
	//0水平方向上一半，1水平方向下一半
	void BuildHalf(int type, SkPath &path) {
		switch (type) {
			case 0:
				path.moveTo(x + x0, y + y0);
				path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
				path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
				break;
			case 1:
				path.moveTo(x + xe, y + ym);
				path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
				path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);
				break;
		}
	}

}BuildCylinderParam;
struct Tools{
	static inline SkColorType convertPixelFormat(android::PixelFormat format)
	{
	    switch (format) {
	    case android::PIXEL_FORMAT_RGBX_8888:    return kRGBA_8888_SkColorType;
	    case android::PIXEL_FORMAT_RGBA_8888:    return kRGBA_8888_SkColorType;
	    case android::PIXEL_FORMAT_RGB_565:      return kRGB_565_SkColorType;
	    case android::PIXEL_FORMAT_RGBA_4444:     return kARGB_4444_SkColorType;
	    //case android::PIXEL_FORMAT_RGB_888:       return kIndex_8_SkColorType;
	    default:                        return kUnknown_SkColorType;
	    }
	}
	static uint32_t bytesPerPixel(int format) {
		switch (format) {
			case android::PIXEL_FORMAT_RGBA_FP16:
				return 8;
			case android::PIXEL_FORMAT_RGBA_8888:
			case android::PIXEL_FORMAT_RGBX_8888:
			case android::PIXEL_FORMAT_BGRA_8888:
			case android::PIXEL_FORMAT_RGBA_1010102:
				return 4;
			case android::PIXEL_FORMAT_RGB_888:
				return 3;
			case android::PIXEL_FORMAT_RGB_565:
			case android::PIXEL_FORMAT_RGBA_5551:
			case android::PIXEL_FORMAT_RGBA_4444:
				return 2;
		}
		return 0;
	}

	static SkColorType flinger2skia(android::PixelFormat f)
	{
		switch (f) {
		    case android::PIXEL_FORMAT_RGB_565:
				return kRGB_565_SkColorType;
			case android::PIXEL_FORMAT_RGBX_8888:
			case android::PIXEL_FORMAT_RGBA_8888:
				return kRGBA_8888_SkColorType;
			default:
				return kN32_SkColorType;
		}
	}
    /*角度转换为弧度*/
    static float ToRadians(float deg) {
        return (deg / _Rad_to_deg);
    }
	//atan函数求角度时无方向（矢量），atan2函数求角度时有方向（矢量）。
		static double LineAngle1(SkPoint &p1, SkPoint &p2) {
			//求角度
			double tmp = (p2.fY - p1.fY) / (p2.fX - p1.fX);
			double deg = atan(tmp);
			//弧度
			deg = deg * (double) 180 / PI;
			double angle = atan2((p2.fY - p1.fY), (p2.fX - p1.fX))
					* (double) 180/ PI; //atan 无方向,atan2 有方向 （矢量）
			return angle;
		}

	static double LineAngle2(SkPoint &p1, SkPoint &p2) {
		//求角度
		double tmp = (p2.fY - p1.fY) / (p2.fX - p1.fX);
		double deg = atan(tmp);
		return deg = deg * (double) 180 / PI;
	}

	static bool IsFiniteNumber(double x) {
		return (x <= DBL_MAX && x >= -DBL_MAX);
	}

	static double GetLineLength(SkPoint &p) {
		return hypot(p.fX, p.fY);
	}

	static double GetDistance(SkPoint &p0, SkPoint &p1) {
		return hypot((p1.fX - p0.fX), (p1.fY - p0.fY));
	}
	static SkPoint GetCenter(SkPoint p0, SkPoint p1) {
		return SkPoint::Make((p0.fX + p1.fX) / 2,  (p0.fY + p1.fY) / 2); /*中心点*/
	}

    static double GetDistance(float x0, float y0, float x1, float y1)
    {
        return  hypot((x1 - x0), (y1 - y0));
    }

	static float Rotation(SkPoint &p0, SkPoint &p1) {
		float ret = 0;
		double delta_x = (p0.fX - p1.fX);
		double delta_y = (p0.fY - p1.fY);
		double radians = atan2(delta_y, delta_x);
		ret = (radians * 180.0) / PI;
		return ret;
	}


	static void calcVertexes(float start_x, float start_y, float end_x, float end_y, float& x1, float& y1, float& x2, float& y2)
	{
		float arrow_lenght_ = 15;
		float arrow_degrees_ = 0.5;
		double angle = atan2 (end_y - start_y, end_x - start_x) + M_PI; //cos@ = x / r sin@ = y/r;
		x1 = end_x + arrow_lenght_ * cos(angle - arrow_degrees_);
		y1 = end_y + arrow_lenght_ * sin(angle - arrow_degrees_);
		x2 = end_x + arrow_lenght_ * cos(angle + arrow_degrees_);
		y2 = end_y + arrow_lenght_ * sin(angle + arrow_degrees_);
	}

	static inline void Point2Rect(ARect *r, const int &x1, const int &y1, const int &x2, const int &y2, const int w)
	{
		int minX = x1;
		int maxX = x2;
		int minY = y1;
		int maxY = y2;

		if (minX > maxX) {
			minX = x2;
			maxX = x1;
		}
		if (minY > maxY) {
			minY = y2;
			maxY = y1;
		}
		r->left =   std::min(minX, r->left)  - w;
		r->right =  std::max(maxX, r->right) + w;
		r->top =    std::min(minY, r->top)   - w;
		r->bottom = std::max(maxY, r->bottom) + w;
	}

	static inline void Point2Rect(SkRect &r, const SkPoint p0, const SkPoint p1, const int w)
	{
		int minX = p0.fX;
		int maxX = p1.fX;

		int minY = p0.fY;
		int maxY = p1.fY;

		if (minX > maxX) {
			minX = p1.fX;
			maxX = p0.fX;
		}
		if (minY > maxY) {
			minY = p1.fY;
			maxY = p0.fY;
		}
		r.fLeft   =  std::min(minX, (int)r.fLeft)  - w;
		r.fRight  =  std::max(maxX, (int)r.fRight) + w;
		r.fTop    =  std::min(minY, (int)r.fTop)   - w;
		r.fBottom =  std::max(maxY, (int)r.fBottom) + w;
	}

	static inline void Point2Rect(SkRect &r, const SkPoint p0, const SkPoint p1, const SkPoint p2, const int w)
	{
		r.fLeft = r.fRight = p0.fX;
		r.fTop = r.fBottom = p0.fY;
		int minX = p1.fX;
		int maxX = p2.fX;

		int minY = p1.fY;
		int maxY = p2.fY;

		if (minX > maxX) {
			minX = p2.fX;
			maxX = p1.fX;
		}
		if (minY > maxY) {
			minY = p2.fY;
			maxY = p1.fY;
		}
		r.fLeft   =  std::min(minX, (int)r.fLeft)  - w;
		r.fRight  =  std::max(maxX, (int)r.fRight) + w;
		r.fTop    =  std::min(minY, (int)r.fTop)   - w;
		r.fBottom =  std::max(maxY, (int)r.fBottom) + w;
	}

	static inline void Point2Rect(SkRect &r, const Point p0, const Point p1, const Point p2, const int w)
	{
		r.fLeft = r.fRight = p0.fX;
		r.fTop = r.fBottom = p0.fY;
		int minX = p1.fX;
		int maxX = p2.fX;

		int minY = p1.fY;
		int maxY = p2.fY;

		if (minX > maxX) {
			minX = p2.fX;
			maxX = p1.fX;
		}
		if (minY > maxY) {
			minY = p2.fY;
			maxY = p1.fY;
		}
		r.fLeft   =  std::min(minX, (int)r.fLeft)  - w;
		r.fRight  =  std::max(maxX, (int)r.fRight) + w;
		r.fTop    =  std::min(minY, (int)r.fTop)   - w;
		r.fBottom =  std::max(maxY, (int)r.fBottom) + w;
	}

	static ARect Point2Rect(SkPoint p1, SkPoint p2, int w)
	{
		ARect ret;
		ret.left = min((int)p1.fX, (int)p2.fX) - w;
		ret.top = min((int)p1.fY, (int)p2.fY) - w;
		ret.right = max((int)p1.fX, (int)p2.fX) + w;
		ret.bottom = max((int)p1.fY, (int)p2.fY) + w;
		return ret;
	}

	static inline void Point2Rect(ARect *r, const int &x1, const int &y1, const int w)
	{
		r->left = std::min(x1, r->left) - w;
		r->right = std::max(x1, r->right) + w;
		r->top = std::min(y1, r->top) - w;
		r->bottom = std::max(y1, r->bottom) + w;
	}

	static inline void SinglePoint2Rect(ARect *r, const int &x, const int &y, const int w)
	{
		r->left = x - w / 2;
		r->top = y - w / 2;
		r->right = x + w / 2;
		r->bottom = y + w / 2;
	}

	static inline void SinglePoint2Rect(ARect *r, const int &x, const int &y, const int w, const int h) {
		r->left = x - w / 2;
		r->top = y - h / 2;
		r->right = x + w / 2;
		r->bottom = y + h / 2;
	}
	static inline void MergeRect(ARect *dst, ARect *src) //合并矩形
	{
		dst->left =  dst->left < src->left ? dst->left : src->left;
		dst->right = dst->right > src->right ? dst->right : src->right;
		dst->top = dst->top < src->top ? dst->top : src->top;
		dst->bottom = dst->bottom > src->bottom ? dst->bottom : src->bottom;
	}

	static inline void MergeRect(SkRect *dst, SkRect src) //�ϲ�����
	{
		if (!dst->isEmpty()) {
            dst->fLeft = std::min(dst->fLeft, src.fLeft);
            dst->fRight = std::max(dst->fRight, src.fRight);
            dst->fTop = std::min(dst->fTop, src.fTop);
            dst->fBottom = std::max(dst->fBottom, src.fBottom);
		} else {
            *dst = src;
		}
	}

	static inline void MergeRect(SkRect *dst, const SkPoint p) //�ϲ�����
	{
		if (!dst->isEmpty()) {
			dst->fLeft = std::min(dst->fLeft, p.fX);
			dst->fRight = std::max(dst->fRight, p.fX);
			dst->fTop = std::min(dst->fTop, p.fY);
			dst->fBottom = std::max(dst->fBottom, p.fY);
		} else {
			SkPoint pp = p;
			pp.offset(2 ,2);
			dst->set(p, pp);
		}
	}

	static inline void MergeRect(ARect *dst, ARect *src, int w, int h) //�ϲ�����
	{
		dst->left = std::min(dst->left, src->left);
		dst->right = std::max(dst->right, src->right);
		dst->top = std::min(dst->top, src->top);
		dst->bottom = std::max(dst->bottom, src->bottom);
		dst->left -= w;
		dst->right += w;
		dst->top -= h;
		dst->bottom += h;
	}

	static inline void ResetRect(ARect *r, const int &x, const int &y)
	{
		r->left = r->right = x;
		r->top = r->bottom = y;
	}

	static inline void ResetRect(SkRect *r, int value)
	{
		r->fLeft = r->fRight  = value;
		r->fTop  = r->fBottom = value;
	}

	static inline void ResetRect(ARect *r, const int &x, const int &y, const int &w)
	{
		r->left = x - w;
		r->right = x + w;
		r->top =  y - w;
		r->bottom = y + w;;
	}

    static inline void ResetRect(SkRect *r, const float &x, const float &y)
    {
        r->fLeft = r->fRight = x;
        r->fTop = r->fBottom = y;
    }

//	static inline void ARect2Rect(const ARect *src, Rect *dst) //转换矩形
//	{
//		dst->left = src->left;
//		dst->top = src->top;
//		dst->right = src->right;
//		dst->bottom = src->bottom;
//	}

//	static inline void SkRect2Rect(const SkRect *src, Rect *dst)
//	{
//		dst->left = src->fLeft;
//		dst->top = src->fTop;
//		dst->right = src->fRight;
//		dst->bottom = src->fBottom;
//	}

	static inline void ARect2SkRect(const ARect *src, SkRect *dst) //转换矩形
	{
		dst->fLeft = (float)src->left;
		dst->fTop = (float)src->top;
		dst->fRight = (float)src->right;
		dst->fBottom = (float)src->bottom;
	}
	static inline void ARect2SkRect(const ARect *src, SkRect *dst, float w) //转换矩形
	{
		dst->fLeft = src->left - w;
		dst->fTop = src->top - w;
		dst->fRight = src->right + w;
		dst->fBottom = src->bottom + w;
	}

	static inline bool Contains(const SkRect *r, unsigned int x, unsigned int y) {
		return !(x < r->fLeft || y < r->fTop || x > r->fRight || y > r->fBottom);
	}
	static inline bool Contains(const SkRect *r, const float x,  const float y) {
		return !(x < r->fLeft || y < r->fTop || x > r->fRight || y > r->fBottom);
	}
	static inline bool Contains(const ARect *r, int32_t x, int32_t y) {
		return !(x < r->left || y < r->top || x > r->right || y > r->bottom);
	}

	static inline bool Contains(const ARect *r, float x, float y) {
		return !(x < r->left || y < r->top || x > r->right || y > r->bottom);
	}
	static inline bool Contains(const ARect *r, SkPoint p) {
		return !(p.fX < r->left || p.fY < r->top || p.fX > r->right || p.fY > r->bottom);
	}
	static inline bool Contains(const ARect *r, int32_t x, int32_t y, int32_t w) {
		return (x >= (r->left - w) && x <= (r->right + w) && y >= (r->top - w) && y <= (r->bottom + w));
	}

	static inline bool  RectIntersect(SkRect r1, ARect r2) //判断两个矩形是否相交
	{
		return !(r1.fLeft > (float)r2.right || r1.fTop > (float)r2.bottom || (float)r2.left > r1.fRight || (float)r2.top > r1.fBottom);
	}
    static inline bool RectIntersect(const SkRect r1, const SkRect r2) //判断两个矩形是否相交
    {
        return !(r1.fLeft > r2.fRight || r1.fTop > r2.fBottom || r2.fLeft > r1.fRight || r2.fTop > r1.fBottom);
    }

	static inline bool RectContains(const SkRect dst, SkRect src) //判断dst是否包含src
	{
		return (src.fLeft >= dst.fLeft && src.fRight <= dst.fRight && src.fTop >= dst.fTop && src.fBottom <= dst.fBottom);
	}

	static inline bool RectContains1(const SkRect dst, SkRect src) //判断dst是否包含src
	{
		float x = src.fLeft - dst.fLeft;
		float y = src.fTop  - dst.fTop;
        if (x <= 0 || y <= 0) return false;
        x = src.fRight - dst.fRight;
        y = src.fBottom - dst.fBottom;
        if (x >= 0 || y >= 0) return false;
        return true;
	}

	static inline bool RectContains1(const SkRect dst, ARect src) //判断dst是否包含src
	{
		float x = src.left - dst.fLeft;
		float y = src.top  - dst.fTop;
		if (x <= 0 || y <= 0) return false;
		x = src.right - dst.fRight;
		y = src.bottom - dst.fBottom;
		if (x >= 0 || y >= 0) return false;
		return true;
	}


	static inline bool OnRectRight(const SkRect r, float x, float y) {
		if (x > r.right() && y > r.top() && y < r.bottom()) {
			return true;
		}
		return false;
	}

	static inline SkRect ConverRect(const SkRect r) {
    	SkRect skr;
		skr.fLeft = std::min(r.fLeft, r.fRight);
		skr.fRight = std::max(r.fLeft, r.fRight);
		skr.fTop = std::min(r.fTop, r.fBottom);
		skr.fBottom = std::max(r.fTop, r.fBottom);
		return skr;
    }
	//计数中文字符个数
	static inline int ChineseCharNum(const std::string &text) {
		const char *str = text.c_str(); //一个中文字符占3个字节
		int cnt = 0;
		int n = 0;
		while (str[n] != '\0') {
			if ((str[n] & 0x80)) {
				n += _zh_char_len;
				cnt++;
			} else {
				n++;
			}
		}
		return cnt;
	}

	static void fivePoints(std::vector<SkPoint>&pv, float xA, float yA, int rFive) {
		float xB = 0;
		float xC = 0;
		float xD = 0;
		float xE = 0;
		float yB = 0;
		float yC = 0;
		float yD = 0;
		float yE = 0;
		xD = (float) (xA - rFive * sin(ToRadians(18)));
		xC = (float) (xA + rFive * sin(ToRadians(18)));
		yD = yC = (float) (yA + cos(ToRadians(18)) * rFive);
		yB = yE = (float) (yA + sqrt(pow((xC - xD), 2) - pow((rFive / 2), 2)));
		xB = xA + (rFive / 2);
		xE = xA - (rFive / 2);

		SkPoint pt;

		pt.set(xA, yA);
		pv.push_back(pt);

		pt.set(xD, yD);
		pv.push_back(pt);

		pt.set(xB, yB);
		pv.push_back(pt);

		pt.set(xE, yE);
		pv.push_back(pt);

		pt.set(xC, yC);
		pv.push_back(pt);

		pt.set(xA, yA);
		pv.push_back(pt);
	}

	static void RectScaleAndOffset(SkRect &rect, float sx, float sy, float dx, float dy) {
		rect.fLeft   *= sx;
		rect.fRight  *= sx;
		rect.fTop    *= sy;
		rect.fBottom *= sy;
		rect.offset(dx, dy);
    }

    static std::string GetTimeStr() {
		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970年到现在的秒数
    	tm nowTime;
		localtime_r(&time.tv_sec, &nowTime);
		char current[256];
		memset(current, 0, 256);
		snprintf(current, 256, "%04d-%02d-%02d-%02d-%02d-%02d", nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday,
				nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
		std::string str = current;
		return str;
    }

    static ARect SkRect2Arect(const SkRect r) {
        ARect rr;
        rr.left = r.fLeft;
        rr.top = r.fTop;
        rr.right = r.fRight;
        rr.bottom = r.fBottom;
        return rr;
    }

 	static SkRect GetCurrRect(SkRect rectf, float padding, int w, int h) {
		int left = (int) (rectf.left() - padding);
		if (left < 0) {
			left = 0;
		}
		int top = (int) (rectf.top() - padding);
		if (top < 0) {
			top = 0;
		}
		int right = (int) (rectf.right() + padding);
		if (right > w) {
			right = w;
		}
		int bottom = (int) (rectf.bottom() + padding);
		if (bottom > h) {
			bottom = h;
		}
		SkRect rr = SkRect::MakeLTRB(left, top, right, bottom);
		return rr;
	}

	static void SkRect2ARect(ARect &dst, const SkRect src) {
		dst.left = src.left();
		dst.top = src.top();
		dst.right = src.right();
		dst.bottom = src.bottom();
	}
	static void BuildCylinder(SkRect top, SkRect bottom, SkPath &path) {
		float x = top.fLeft;
		float y = top.fTop;

		float w = top.width();
		float h = top.height();
		float k = 0.5522848;
		float ox = (w / 2) * k;
		float oy = (h / 2) * k;
		float xe = w;
		float ye = h;
		float xm = w / 2;
		float ym = h / 2;

		//拟合椭圆下曲线
		path.moveTo(x + xe, y + ym);
		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);

//拟合圆柱体下椭圆
		x = bottom.fLeft;
		y = bottom.fTop;
		w = bottom.width();
		h = bottom.height();
		ox = (w / 2) * k;
		oy = (h / 2) * k;
		xe = w;
		ye = h;
		xm = w / 2;
		ym = h / 2;

		float x0 = 0, y0 = ym;
		float x1 = 0, y1 = ym - oy;
		float x2 = xm - ox, y2 = 0;
		float x3 = xm, y3 = 0;

		path.lineTo(x + x0, y + y0);
		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);

		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);

		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
		path.close();
    }
//拟合椭圆的方法
	void static Cylinder(SkPath &path, SkRect oval) {
    	float x = oval.fLeft;
    	float y = oval.fTop;
    	float w = oval.width();
		float h = oval.height();
		float k = 0.5522848;
		float ox = (w / 2) * k;
		float oy = (h / 2) * k;
		float xe = w;
		float ye = h;
		float xm = w / 2;
		float ym = h / 2;

		float x0 = 0, y0 = ym;
		float x1 = 0, y1 = ym - oy;
		float x2 = xm - ox, y2 = 0;
		float x3 = xm, y3 = 0;

		path.moveTo(x + x0, y + y0);
		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);
    }


	static void BuildRoundTable(SkRect top, SkRect bottom, SkPath &path) {
		path.reset();

		float x = top.fLeft;
		float y = top.fTop;

		float w = top.width();
		float h = top.height();
		float k = 0.5522848;
		float ox = (w / 2) * k;
		float oy = (h / 2) * k;
		float xe = w;
		float ye = h;
		float xm = w / 2;
		float ym = h / 2;

		//拟合椭圆下曲线
		path.moveTo(x + xe, y + ym);
		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);

//拟合圆柱体下椭圆
		x = bottom.fLeft;
		y = bottom.fTop;
		w = bottom.width();
		h = bottom.height();
		ox = (w / 2) * k;
		oy = (h / 2) * k;
		xe = w;
		ye = h;
		xm = w / 2;
		ym = h / 2;

		float x0 = 0, y0 = ym;
		float x1 = 0, y1 = ym - oy;
		float x2 = xm - ox, y2 = 0;
		float x3 = xm, y3 = 0;

		path.lineTo(x + x0, y + y0);
		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);

		path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
		path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);

		path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
		path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
		path.close();
	}


	static void BuildCone(SkPoint top, SkRect oval, SkPath &path) {
		float x = oval.fLeft;
		float y = oval.fTop;
		float w = oval.width();
		float h = oval.height();
		float k = 0.5522848;
		float ox = (w / 2) * k;
		float oy = (h / 2) * k;
		float xe = w;
		float ye = h;
		float xm = w / 2;
		float ym = h / 2;

		float x0 = 0, y0 = ym;
		float x1 = 0, y1 = ym - oy;
		float x2 = xm - ox, y2 = 0;
		float x3 = xm, y3 = 0;

		if (top.fY > oval.bottom()) {
			path.moveTo(x + x0, y + y0);
			path.cubicTo(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
			path.cubicTo(x + xm + ox, y + 0, x + xe, y + ym - oy, x + xe, y + ym);
			path.lineTo(top);
			path.close();
		} else {
			path.moveTo(x + xe, y + ym);
			path.cubicTo(x + xe, y + ym + oy, x + xm + ox, y + ye, x + xm, y + ye);
			path.cubicTo(x + xm - ox, y + ye, x + 0, y + ym + oy, x + 0, y + ym);
			path.lineTo(top);
			path.close();
    	}
    }




	static void BuildHemisphere(bool top, SkRect oval1, SkRect oval2, SkPath &path) {
		BuildCylinderParam param;
		param.Init(oval1);
    	if (top) {
			param.BuildHalf(0, path);
			param.Init(oval2);
			param.BuildHalf(1, path);
    	} else {
			param.BuildHalf(1, path);
			param.Init(oval2);
			param.BuildHalf(0, path);
    	}
    }
};





#endif /* TOOLS_H_ */

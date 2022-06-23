/*
 * abstract.cpp
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */

#include "abstract.h"
#include "common.h"
#include "SkiaLog.h"
#include "Tools.h"
#include <SkTypeface.h>

ARect AbstractAction::rect_;
unsigned int AbstractAction::objId_ = 0;
SkPaint AbstractAction::paint_;
sk_sp<SkPathEffect> AbstractAction::pathEffect_ = nullptr;
sk_sp<SkTypeface> AbstractAction::font_ = nullptr;
SkPoint AbstractAction::ctrlPt_[3] = {0};
unsigned long AbstractAction::strokeId_ = 0;
pthread_mutex_t AbstractAction::mutex_;
int8_t* AbstractAction::cacheMem_ = nullptr;
SkBitmap AbstractAction::cacheBmp_;
MemoryPool<BezierCurve, 4096>* AbstractAction::pool_;
PaintersPtr AbstractAction::painters_;
AbstractAction::AbstractAction(NativeWindowInterface *nativeWindowObj, const int &OperatorType, const int &strokeColor, const float &strokeWidth):Operation0bjects()
{
	strokeWidth_ = strokeWidth;
	color_ = strokeColor;
	window_ = nativeWindowObj;
	operatorType_ = OperatorType;
	curPointer_ = -1;
	event_ = MotionEvent_ACTION_Invalid;
	if (window_) c_ = window_->GetSkCanvas();
	if (operatorType_ != OperatorType_Ruler)
		id_ = objId_++;
	ResetPaint(OperatorType);
}

AbstractAction::~AbstractAction()
{
}

void AbstractAction::Init(SkImageInfo &ii)
{
    paint_.setStrokeWidth(5);
	paint_.setColor(SK_ColorRED);
	paint_.setStyle(SkPaint::kStroke_Style);
	paint_.setAntiAlias(true);
	paint_.setStrokeJoin(SkPaint::kRound_Join);
	paint_.setStrokeCap(SkPaint::kRound_Cap);
	paint_.setDither(true);
	if (!pathEffect_) {
		float intervals[4] = { 5, 5, 5, 5 };
        pathEffect_ = SkDashPathEffect::Make(intervals, 4, SK_ScalarHalf);
	}
    strokeId_ = 1;
	pthread_mutex_init(&mutex_, nullptr);
	if (!pool_) {
        pool_ = new MemoryPool<BezierCurve, 4096>;
	}

	if (!painters_) {
		painters_ = std::make_shared<Painters>();
	}
}

void AbstractAction::AddPainter(int boardId, int num)
{
	if (painters_) {
		painters_->AddPainter(boardId, num);
	}
}

void AbstractAction::LoadFont(int language)
{
    if (!font_) {
        std::string fontPath = "";
        if (language == 0) { //默认
            fontPath = "/system/fonts/DroidSansFallback.ttf";
        } else { //俄语
            fontPath = "/system/fonts/RobotoCondensed-Regular.ttf";
        }
        font_ = SkTypeface::MakeFromFile(fontPath.c_str()); //加载字体
    }
}

void AbstractAction::UnInit()
{
    pathEffect_ = nullptr;
	font_ = nullptr;
    strokeId_ = 0;
    objId_ = 0;
    paint_.reset();
	cacheBmp_.setPixels(NULL);
	if (cacheMem_) {
		delete []cacheMem_;
		cacheMem_ = nullptr;
	}
	if (pool_) {
	    delete pool_;
        pool_ = nullptr;
	}
	painters_ = nullptr;
}

void AbstractAction::ResetPaint(const int operatorType)
{
	if (operatorType == OperatorType_Draw || operatorType == OperatorType_Select || operatorType == OperatorType_Text) {
		paint_.setStrokeWidth(strokeWidth_);
		paint_.setStyle(SkPaint::kStroke_Style);
		paint_.setColor(color_);
		paint_.setPathEffect(nullptr);
	}
}


bool AbstractAction::Point2Path(MultiPointInfo &pointInfo, float x, float y, int endFlag, bool flag) {
	if (event_ == MotionEvent_ACTION_DOWN || event_ == MotionEvent_ACTION_POINTER_DOWN) {
		pointInfo.lastPoint.set(x, y);
		pointInfo.InitPv(x ,y);
        pointInfo.event = event_;
        pointInfo.color = color_;
        pointInfo.id = GetStrokeId();
		return true;
	} else if (event_ == MotionEvent_ACTION_UP || event_ == MotionEvent_ACTION_POINTER_UP) {
		if (!pointInfo.IsSamept(x, y)) {
			if (pointInfo.path.isEmpty()) {
				pointInfo.path.moveTo(pointInfo.lastPoint);
			}
            SkPoint pt = pointInfo.GetPt(x ,y);
            pointInfo.path.lineTo(pt);
			ctrlPt_[0] = pointInfo.startPoint;
			ctrlPt_[1] = pointInfo.lastPoint;
			ctrlPt_[2].fX = x;
			ctrlPt_[2].fY = y;
			return true;
		} else {
            pointInfo.reset();
			return false;
		}
	}
    pointInfo.event = -1;
	//防止丢失按下事件
	if (pointInfo.id == 0) {
        pointInfo.lastPoint.set(x, y);
        pointInfo.InitPv(x ,y);
        pointInfo.event = MotionEvent_ACTION_DOWN;
        pointInfo.id = GetStrokeId();
		pointInfo.color = color_;
		INFO("lost down Pt id = %ld", pointInfo.id);
		return true;
	}
	//去除重复点
//    if (pointInfo.IsSamept(x, y)) {
//        return false;
//    }
    //过滤点
    if (abs(pointInfo.lastPoint.fX - x) <= 2 && abs(pointInfo.lastPoint.fY - y) <= 2) {
        return false;
    }
    if (pointInfo.path.isEmpty()) {
		pointInfo.path.moveTo(pointInfo.lastPoint);
	}
	SkPoint pt = pointInfo.GetPt(x ,y);
	pointInfo.path.lineTo(pt);
	pointInfo.lastPoint.set(pt.fX, pt.fY);
	return true;
}


bool AbstractAction::Point2Path(int pointerId, float x, float y, int endFlag)
{
    if (pointerId >= 0 && pointerId < _MultiPointMaxNum) {
    	MultiPointInfo &info = painter_->GetPointInfo(pointerId);
        if (Point2Path(info, x, y, endFlag, true)) {
            SetCurPointId(pointerId);
            return true;
        }
    }
    return false;
}

void AbstractAction::Draw(RefreshMode pos, int shouldFlush)
{
	if (operatorType_ != OperatorType_Select && window_->IsFbOn()) {
	    DrawStroke(GetPenType() == PenType_Pencil);
	} else {
		if (curPointer_ >=0) {
			paint_.setColor(color_);
            MultiPointInfo &info = painter_->GetPointInfo(curPointer_);
			c_->drawPath(info.path, paint_);
			if (shouldFlush) {
				SkCanvas *c = window_->LockCanvas(&rect_);
				if (c) {
					SkRect r;
					Tools::ARect2SkRect(&rect_, &r);
                    c->drawBitmapRect(window_->GetBmp(), r, r, nullptr);
					window_->unlockCanvasAndPost(c);
				}
			}
			Tools::ResetRect(&info.r, info.startPoint.fX,
                             info.startPoint.fY);
			curPointer_ = -1;
		}
	}
}

void AbstractAction::DrawStroke(bool save)
{
	if (curPointer_ >= 0) {
	    MultiPointInfo &info = painter_->GetPointInfo(curPointer_);
		paint_.setColor(info.color);
		paint_.setStrokeWidth(strokeWidth_);
		window_->DrawPath(info.path, info.path, paint_, save);
		curPointer_ = -1;
	}
}

void AbstractAction::DrawByRect(SkCanvas *c, SkPaint &paint, SkRect r)
{
	Draw(c);
}

void AbstractAction::Offset(float dx, float dy){}
void AbstractAction::ScaleAndOffset(float sx, float sy, float dx, float dy){}

void AbstractAction::SetState(State state)
{
	pthread_mutex_lock(&mutex_);
	state_ = state;
	pthread_mutex_unlock(&mutex_);
}

State AbstractAction::GetState()
{
	State state;
	pthread_mutex_lock(&mutex_);
	state = state_;
	pthread_mutex_unlock(&mutex_);
	return state;
}

void AbstractAction::SetPaint()
{
	paint_.setColor(color_);
	paint_.setStrokeWidth(strokeWidth_);
	paint_.setPathEffect(nullptr);
}

void AbstractAction::ResetId(int pointerId)
{
    if (painter_) {
        painter_->ResetId(pointerId);
    }
}

void AbstractAction::AssociatePainter(int blackBoardId)
{
    if (painters_) {
        painter_ = painters_->GetPainter(blackBoardId);
    }
}

void AbstractAction::Draw2Screen(SkCanvas *c, SkRect &bound)
{

}

void AbstractAction::SetViewRect(int boardId, SkRect &r)
{
    viewRect_ = r;
}
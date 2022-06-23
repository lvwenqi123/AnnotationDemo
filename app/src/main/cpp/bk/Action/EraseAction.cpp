/*
 * DrawAction.cpp
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */
#include "EraseAction.h"
#include "common.h"
#include "Tools.h"
//#include "RunTimeTest.h"
#include "LogUtil.h"

static const char *tag = "EraseAction";
static void DrawBitmap(SkCanvas *c, SkBitmap &bmp, SkRect r, int x, int y);
void DrawBitmap(SkCanvas *c, SkBitmap &bmp, SkRect r, int x, int y)
{
//	c->save();
//	c->clipRect(r);
//	c->drawBitmap(bmp, x, y);
//	c->restore();
	//c->drawBitmapRectToRect(bmp, &r, r);
	c->drawBitmapRect(bmp, r, r, NULL);
}

EraseAction::EraseAction(NativeWindowInterface *nativeWindowObj, int OperatorType, const int &strokeColor, const float &strokeWidth)
:AbstractAction(nativeWindowObj, OperatorType, strokeColor, strokeWidth)
{
	erasePicW_ = erasePicH_ = 0;
    //strokeHandle_ = std::make_shared<StrokeHandle>();
    Tools::ResetRect(&lastRect_, 0, 0);
	Tools::ResetRect(&eraseRect_, 0, 0);
	path_.reset();
}
EraseAction::~EraseAction()
{
	ENTER_F
	rlist_.clear();
	eraseObjs_.clear();
    eraseObjIds_.clear();
	strokeHandle_ = nullptr;
	path_.reset();
	EXIT_F
}

void EraseAction::SetTouchEvent(int pointerId, int event)
{
	event_ = event;
	if (MotionEvent_ACTION_POINTER_UP == event_ ||
			event_ == MotionEvent_ACTION_UP) {
		INFO("EraseAction up event = %d pointerId = %d eraserPointer_ = %d", event, pointerId, eraserPointer_);
		if (event_ == MotionEvent_ACTION_UP) {
			curPointer_ = 1;
			Tools::ResetRect(&eraseRect_, 0, 0);
			SetState(State_Up);
			return;
		}
		if (pointerId >= 0) {
			if (pointerId == eraserPointer_) {
				eraserPointer_ = -1;
				curPointer_ = pointerId;
				Tools::ResetRect(&eraseRect_, 0, 0);
				if (lastRect_.left > 0 || lastRect_.right > 0) {
					SkRect skr = SkRect::MakeEmpty();
					Tools::ARect2SkRect(&lastRect_, &skr);
#if 0
					window_->DrawBitmapRectToRect(*bg_, skr, skr);
					Tools::ResetRect(&lastRect_, 0, 0);
					Tools::ResetRect(&eraseRect_, 0, 0);
#else
//					SkCanvas *c = window_->LockCanvas(&lastRect_);
//					if (c) {
//						window_->DrawBitmapRectToRect(*bg_, skr, skr);
//						c->drawBitmapRect(window_->GetBmp(), skr, skr, nullptr);
//						window_->unlockCanvasAndPost(c);
//						Tools::ResetRect(&lastRect_, 0, 0);
//						Tools::ResetRect(&eraseRect_, 0, 0);
//					}
#endif
				}
			}
		}
	}
}

void EraseAction::AdjustEraseSize()
{
	if (viewRect_.fLeft == 0.0) {
		int len = (int)viewRect_.right() - eraseRect_.right;
		if (len < 0) {
			eraseRect_.right += len;
		}
	} else {
		int len = (int)viewRect_.fLeft - eraseRect_.left;
		if (len >0) {
			eraseRect_.left += len;
		}
	}
}

void EraseAction::AddPoint(int pointerId, float x, float y, float size, int endFlag) {
    if (eraserPointer_ == -1) {
        eraserPointer_ = pointerId;
    }
    if (pointerId != eraserPointer_) return;
    Tools::SinglePoint2Rect(&eraseRect_, x, y, erasePicW_, erasePicH_);
	AdjustEraseSize();
    curPointer_ = eraserPointer_;
    SkRect r;
    Tools::ARect2SkRect(&eraseRect_, &r);
//    if (event_ != MotionEvent_ACTION_UP) {
//		SetState(State_Erase);
//    }
//    pthread_mutex_lock(&mutex_);
//    rlist_.push_back(eraseRect_);
//    pthread_mutex_unlock(&mutex_);
}

bool EraseAction::GetARect(ARect &r)
{
	bool ret = false;
	pthread_mutex_lock(&mutex_);
	/*此处是为了考虑在橡皮擦的情况下调用漫游，如果双指直接放上去，rlist_.size()就对于1，事实上这一点没必要调用橡皮擦进行遍历*/
	if (!rlist_.empty() && (state_ == State_Tidy || rlist_.size() > 0)) {
		r = rlist_.front();
		rlist_.pop_front();
		ret = true;
		LOG_D(tag, "size = %d", (int)rlist_.size());
	}
	pthread_mutex_unlock(&mutex_);
	return ret;
}

void EraseAction::Draw(RefreshMode mode, int shouldFlush)
{
	return DrawErase(mode, shouldFlush);
}

void EraseAction::DrawErase(RefreshMode mode, int shouldFlush)
{
#if 0
	if (curPointer_ >= 0) {
		ResetPaint(OperatorType_Erase);
        ARect rect = eraseRect_;
        if (lastRect_.left > 0 || lastRect_.right > 0) {
            Tools::MergeRect(&rect, &lastRect_, 0, 0);
        }
        SkCanvas *c = window_->LockCanvas(&rect);
        LOG_D(tag, "lock cvs");
        if (c) {
            if (lastRect_.left >0 || lastRect_.right > 0) {
                SkRect skr1;
                Tools::ARect2SkRect(&lastRect_, &skr1);
                window_->DrawBitmapRectToRect(*bg_, skr1, skr1);
            }
            SkRect skr;
            Tools::ARect2SkRect(&eraseRect_, &skr);
            c_->drawBitmap(*window_->GetEraseBmp(), skr.fLeft, skr.fTop);
            SkRect r;
            Tools::ARect2SkRect(&rect, &r);
            c->drawBitmapRect(window_->GetBmp(), r, r, nullptr);
            window_->unlockCanvasAndPost(c);
            memcpy(&lastRect_, &eraseRect_, sizeof(eraseRect_));
        } else {
            LOG_D(tag, "LockCanvas error");
        }
        event_ = MotionEvent_ACTION_Invalid;
		curPointer_ = -1;
	}
#endif
}

void EraseAction::Draw(SkCanvas *c)
{
	Tools::ResetRect(&eraseRect_, 0, 0);
	SkPaint erasePaint = paint_;
	erasePaint.setAlpha(0);
	erasePaint.setStyle(SkPaint::kFill_Style);
	erasePaint.setBlendMode(SkBlendMode::kDstIn);
	erasePaint.setStrokeWidth(2);
	c->drawPath(path_, erasePaint);
#if 0
    SkRect r = bound_;
    if (!bound1_.isEmpty()) {
		INFO("---------------2222-");
		r = bound1_;
    }
    INFO("R = l = %f t = %f r = %f b = %f", r.fLeft, r.fTop, r.fRight, r.fBottom);
    int res = c->saveLayer(r, nullptr);
    SkPaint erasePaint = paint_;
	erasePaint.setAlpha(100);
	erasePaint.setStyle(SkPaint::kFill_Style);
	erasePaint.setBlendMode(SkBlendMode::kSrcIn);
	erasePaint.setStrokeWidth(2);
	c->drawPath(path_, erasePaint);
	c->restoreToCount(res);
	INFO("RES = %d", res);
#endif
}

void EraseAction::AddObj(AbstractActionPtr &obj, bool recordId)
{
	eraseObjs_.push_back(obj);
	if (recordId) {
		eraseObjIds_.push_back(obj->GetId());
	}
}

void EraseAction::Erase(ARect *r)
{
	strokeHandle_->EraseStroke(r);
}

bool EraseAction::Tidy(AbstractActionPtr &obj)
{
    bool ret = strokeHandle_->TidyStroke(obj);
	return ret;
}

bool EraseAction::Empty()
{
	return !(!strokeHandle_->Empty() || !eraseObjIds_.empty());
}

void EraseAction::ScaleAndOffset(float sx, float sy, float dx, float dy)
{
	strokeHandle_->ScaleAndOffset(sx, sy, dx, dy, State_InForm);
}

int EraseAction::Redo()
{
	return 0;
}

int EraseAction::Undo()
{
	return 0;
}

void EraseAction::Draw2Screen(SkCanvas *c, SkRect &bound) {
	if (curPointer_ >= 0) {
		ARect rect = eraseRect_;
		if (lastRect_.left > 0 || lastRect_.right > 0) {
			Tools::MergeRect(&rect, &lastRect_, 0, 0);
		}
		if (lastRect_.left > 0 || lastRect_.right > 0) {
			SkRect skr1;
			Tools::ARect2SkRect(&lastRect_, &skr1);
			skr1.outset(2,2);
			window_->DrawBitmapRectToRect(*bg_, skr1, skr1);
		}
		if (event_ != MotionEvent_ACTION_UP || event_ != MotionEvent_ACTION_POINTER_UP) {
			if (eraseRect_.left != 0 && eraseRect_.right != 0) {
				DrawEraseIcon(c_, eraseRect_);
			}
		}
		memcpy(&lastRect_, &eraseRect_, sizeof(eraseRect_));
		SkRect r;
		Tools::ARect2SkRect(&eraseRect_, &r);
		path_.addRect(r, SkPath::kCCW_Direction);
		Tools::MergeRect(&bound_, r);
		event_ = MotionEvent_ACTION_Invalid;
		curPointer_ = -1;
	}
}

void EraseAction::SetEraseSize(int w, int h)
{
	if (w > h) {
		if (w >= 160) w = 160;
		if (h >= 70) h = 70;
	} else {
		if (w >= 70) w = 70;
		if (h >= 160) h = 160;
	}
	if (abs(erasePicW_ - w) >= 10 || abs(erasePicH_ - h) >= 10) {
		int ww = w - 10;
		int hh = h - 10;
		if (ww > 20 && hh > 20) {
			erasePicW_ = ww;
			erasePicH_ = hh;
		} else if (erasePicW_ == 0 && erasePicH_ == 0) {
			erasePicW_ = w;
			erasePicH_ = h;
		}
	}
}

void EraseAction::DrawEraseIcon(SkCanvas *c, ARect &r)
{
	if (c) {
		SkRect skr;
		skr.outset(-4, -4);
		Tools::ARect2SkRect(&eraseRect_, &skr);
		SkPaint paint = paint_;
		paint.setColor(SK_ColorGRAY);
        paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(2);
		//c->drawRoundRect(skr, 15, 15, paint);
		c->drawRect(skr, paint);
	}
}
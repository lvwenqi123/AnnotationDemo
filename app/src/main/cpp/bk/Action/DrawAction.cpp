/*
 * DrawAction.cpp
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */
#include "DrawAction.h"

#include <SkPath.h>

#include "SkiaLog.h"
#include "Tools.h"
#include "LogUtil.h"
#include "EraseAction.h"
#include "StylusStrokeBuilder.h"
#include "StylusStrokeRecord.h"

static const char *tag = "DrawAction";
static float _minStrokeWidth = 1;
static float _radiusFactor = 20;
RadiusCalculator DrawAction::radiusFactor_;
SignPenParam DrawAction::signPen_;
static float _defaultW = 0.3;

DrawAction::DrawAction(NativeWindowInterface *nativeWindowObj, int OperatorType, const uint32_t &strokeColor, const float &strokeWidth)
:AbstractAction(nativeWindowObj, OperatorType, strokeColor, strokeWidth)
{
    ResetPaint(OperatorType);
    signPen_.reset();
    path_.reset();
}
DrawAction::~DrawAction()
{
    strokeHandle_ = nullptr;
}

void DrawAction::SetTouchEvent(int pointerId, int event)
{
    event_ = event;
#if 0
    if (event_ == MotionEvent_ACTION_UP) {
        if (penType_ == PenType_Pressure) {
            if (_debug) {
                LOG_D(tag, "<<=====up Msg id = %d", radiusFactor_.GetReferObjId());
            }
            if (record_) {
                SkPath path;
                path.reset();
                RecordPtr obj = strokeHandle_->GetObjById(radiusFactor_.GetReferObjId());
                if (obj) {
                    obj->BuildPath(path);
                } else {
                    LOG_E(tag, "the obj is null");
                }
                window_->DrawPath(path, paint_, record_);
                radiusFactor_.Reset();
            }
        }
    }
#endif
}

void DrawAction::AddPoint(int pointerId, float x, float y, float size, int endFlag)
{
    HandlePoint(pointerId, x, y, size, endFlag);
    if (event_ == MotionEvent_ACTION_UP) {
        if (penType_ == PenType_Pressure || penType_ == PenType_Sign) {
            if (_debug) {
                LOG_D(tag, "<<=====up Msg id = %d", radiusFactor_.GetReferObjId());
            }
            if (record_) {
                SkPath path;
                path.reset();
                RecordPtr obj = strokeHandle_->GetObjById(radiusFactor_.GetReferObjId());
                if (obj) {
                    obj->BuildPath(path);
                } else {
                    LOG_E(tag, "the obj is null");
                }
                //window_->DrawPathByFb(path, paint_, record_);
                window_->DrawPath(path, paint_, record_);
                radiusFactor_.Reset();
                signPen_.reset();
            }
        }
    }
    event_ = MotionEvent_ACTION_Invalid;
}

void DrawAction::Erase(ARect *r)
{
    strokeHandle_->EraseStroke(r);
}

bool DrawAction::Tidy(AbstractActionPtr &obj)
{
    return strokeHandle_->TidyStroke(obj);
}

void DrawAction::Draw(SkCanvas *c)
{
    if (record_ && state_ == State_Normal) {
//        int res = -1;
//        if (!bound1_.isEmpty()) {
//            res = c->saveLayer(bound1_, nullptr);
//        }
        strokeHandle_->DrawStroke(c, paint_);
//        if (res >=0 ) {
//            c_->restoreToCount(res);
//        }
    }
}

SkRect& DrawAction::GetBound()
{
    if (record_) {
        bound_ = SkRect::MakeEmpty();
        bound_ = strokeHandle_->GetBound();
    }
    return bound_;
}

void DrawAction::ScaleAndOffset(float sx, float sy, float dx, float dy)
{
    strokeHandle_->ScaleAndOffset(sx, sy, dx, dy, State_InForm);
}

void DrawAction::Offset(float dx, float dy)
{
    strokeHandle_->Offset(dx, dy);
}

void DrawAction::SetPenType(PenType penType) {
    penType_ = penType;
    ResetPaint(operatorType_);
    if (record_ && !strokeHandle_) {
        strokeHandle_ = std::make_shared<StrokeHandle>();
    }
    if (penType_ == PenType_Pressure || penType_ == PenType_Sign) { //压感笔
        paint_.setStrokeWidth(0);
        paint_.setColor(color_);
        paint_.setStyle(SkPaint::kFill_Style);
        float factor = _minStrokeWidth * _radiusFactor / 2;
        radiusFactor_.SetValue(factor);
        if (record_) radiusFactor_.SetReferObjId(AbstractAction::GetStrokeId());
    }
    //LOG_E(tag, "penType_ = %d", penType_);
}

void DrawAction::HandlePoint(int pointerId, float x, float y, float size, int endFlag)
{
    if (penType_ == PenType_Pencil || penType_ == PenType_Text || penType_ == PenType_Shape) {
        if (Point2Path(pointerId, x, y, endFlag)) {
            if (record_) {
                MultiPointInfo info = painter_->GetPointInfo(pointerId);
                int id = info.id;
                strokeHandle_->AddStroke(id, info.GetPt(), info.color, strokeWidth_);
                //strokeHandle_->AddStroke(id, ctrlPt_[0], ctrlPt_[1], ctrlPt_[2], info.color, strokeWidth_);
                if (event_ == MotionEvent_ACTION_UP || event_ == MotionEvent_ACTION_POINTER_UP) {
                    ResetId(pointerId);
                }
            }
            bool exist = false;
            for (int i = 0 ; i < pointerIdList_.size(); ++i) {
                if (pointerIdList_[i] == pointerId) {
                    exist = true;
                    break;
                }
            }
            if (!exist) pointerIdList_.push_back(pointerId);
        }
    } else if (penType_ == PenType_Pressure || penType_ == PenType_Sign) { //压感笔
        if (event_ == MotionEvent_ACTION_POINTER_DOWN) { //不处理这两种消息
            if (event_ == MotionEvent_ACTION_POINTER_DOWN) {
                LOG_D(tag, "POINTER_DOWN pointerId = %d", pointerId);
            }
            event_ = MotionEvent_ACTION_Invalid;
            curPointer_ = -1;
            return;
        }
        StylusMeta sm;
        if (event_ == MotionEvent_ACTION_DOWN ) {
            if (_debug) {
                LOG_D(tag, "=====>>down Msg id = %d pointerId = %d", radiusFactor_.GetReferObjId(), pointerId);
            }
            radiusFactor_.SetPointerId(pointerId);
            if (penType_ == PenType_Sign) {
                signPen_.reset();
                sm.SetValue(x, y, 1);
                //window_->DrawCircle(x, y, _defaultW, paint_, record_);
                lastsm_ = sm;
                signPen_.reset();
                size = _defaultW;
                ++signPen_.cnt;
            } else { //压感笔
                sm.SetValue(x, y, radiusFactor_.Calculate(size));
                radiusFactor_.SetPointerId(pointerId);
            }
            if (window_->IsEnableDraw()) {
                path_.reset();
                path_.addCircle(x, y, size, SkPath::kCCW_Direction);
                //window_->DrawCircleByFb(x, y, size, paint_, record_);
            }
            curPointer_ = pointerId;
            lastsm_ = sm;
        } else if(event_ == MotionEvent_ACTION_UP || event_ == MotionEvent_ACTION_POINTER_UP) {
            if (penType_ == PenType_Sign) {
                if (radiusFactor_.GetPointerId() == pointerId) {
                    sm.SetValue(x, y, 1);
                } else return;
            } else {
                return;
            }
        } else {//move消息
            if (radiusFactor_.GetPointerId() != pointerId) {
                event_ = MotionEvent_ACTION_Invalid;
                curPointer_ = -1;
                if (_debug) {
                    LOG_D(tag, "move pointerId = %d radiusFactor_ pid = %d size = %f", pointerId, radiusFactor_.GetPointerId(), size);
                }
                return;
            }
            if (lastsm_.x == x && lastsm_.y == y) { //过滤掉重复点,有重复点会导致画不出笔锋
                return;
            }
            ++signPen_.cnt;
            if (penType_ == PenType_Sign) {
                SkPoint p1 = {lastsm_.x, lastsm_.y};
                SkPoint p2 = {x, y};
                float degree = Tools::LineAngle2(p1, p2);
                degree = abs(degree);
                if (signPen_.lastDegree == -1) signPen_.lastDegree = degree;
                signPen_.lastDegree += degree;
                signPen_.lastDegree /= 2;
                float m = signPen_.lastDegree / 90;
                if (m <= 0.2 || m >= 0.77) {
                    m = 0;
                }
                m *= 0.5;
                float w = strokeWidth_ * (1 - m) + 0;
                if (degree >= 70) {
                    w = strokeWidth_ * 0.8;
//                    INFO("DRAW 1 W = %f", w);
//                    if (signPen_.lastW <= 2 && signPen_.cnt <= 4) {
//                        w = signPen_.lastW = strokeWidth_ * 0.99;  //0.9
//                    } else {
//                        if (first) {
//                            first = false;
//                            w = signPen_.lastW * 0.85; //0.85
//                        } else {
//                            w = signPen_.lastW;
//                        }
//                    }
                } else first = true;
                if (signPen_.lastW != 0) {
                    signPen_.lastW += w;
                    signPen_.lastW *= 0.5;
                    sm.SetValue((lastsm_.x + x) / 2, (lastsm_.y + y) / 2, signPen_.lastW);
                } else {
                    sm.SetValue(x, y, 1);
                }
            } else {
                sm.SetValue(x, y, radiusFactor_.Calculate(size));
            }
            SkPath *path = nullptr;
            if (window_->IsEnableDraw()) {
                path = StylusStrokeBuilder::LinkCircle(lastsm_.x, lastsm_.y, lastsm_.radius, sm.x, sm.y, sm.radius);
            }
            if (path != nullptr) {
                //path_.reset();
                path_.addCircle(sm.x, sm.y, sm.radius, SkPath::kCCW_Direction);
                path_.addPath(*path);
                //window_->DrawCircle(sm.x, sm.y, sm.radius, paint_, record_);
                //window_->DrawCircleByFb(sm.x, sm.y, sm.radius, paint_, record_);
                //window_->DrawPath(*path, paint_, record_);
                //window_->DrawPathByFb(*path, paint_, record_);
                path->reset();
                delete path;
                path = nullptr;
            }
            curPointer_ = pointerId;
            lastsm_ = sm;
        }
        if (record_) {
            StylusMetaPtr ptr = std::make_shared<StylusMeta>();
            ptr->SetValue(sm.x, sm.y, sm.radius);
            strokeHandle_->AddStroke(radiusFactor_.GetReferObjId(), ptr, color_);
        }
    }
}

void DrawAction::DrawByRect(SkCanvas *c, SkPaint &paint, SkRect r)
{
    if (record_ && state_ == State_Select) {
        strokeHandle_->MustDraw(c, paint_);
    }
}

void DrawAction::SetState(State state)
{
    state_ = state;
    if (state_ != State_UndoByErase && state_ != State_RedoByErase) {
        if (strokeHandle_) {
            strokeHandle_->SetState(state);
        }
    }
}

int DrawAction::Redo()
{
    return 0;
}

int DrawAction::Undo()
{
    return 0;
}

void DrawAction::Draw(RefreshMode pos, int shouldFlush)
{
    if (window_->IsFbOn()) {
        if (GetPenType() == PenType_Pencil) {
            DrawStroke(GetPenType() == PenType_Pencil);
        } else {
            window_->DrawPath(path_, path_, paint_, true);
        }
    } else {
        if (curPointer_ >= 0) {
            if (GetPenType() == PenType_Pencil) {
                paint_.setColor(color_);
                MultiPointInfo &info = painter_->GetPointInfo(curPointer_);
                c_->drawPath(info.path, paint_);
                curPointer_ = -1;
                Tools::ResetRect(&info.r, info.startPoint.fX,
                                 info.startPoint.fY);
            } else {
                c_->drawPath(path_, paint_);
                SkRect r = path_.getBounds();
                Tools::SkRect2ARect(rect_, r);
            }

            if (shouldFlush) {
                INFO("#1 l = %d t = %d r = %d t = %d", rect_.left, rect_.top, rect_.right, rect_.bottom);
                SkCanvas *c = window_->LockCanvas(&rect_);
                if (c) {
                    SkRect r;
                    Tools::ARect2SkRect(&rect_, &r);
                    c->drawBitmapRect(window_->GetBmp(), r, r, nullptr);
                    window_->unlockCanvasAndPost(c);
                    INFO("#2");
                }
            }
            curPointer_ = -1;
        }
    }
}

void DrawAction::Draw2Screen(SkCanvas *c, SkRect &bound) {
    if (GetPenType() == PenType_Pencil) {
        if (!pointerIdList_.empty()) {
            paint_.setColor(color_);
            for (int i = 0 ;i < pointerIdList_.size(); ++i) {
                MultiPointInfo &info = painter_->GetPointInfo(pointerIdList_[i]);
                if (info.event == MotionEvent_ACTION_DOWN || info.event == MotionEvent_ACTION_POINTER_DOWN) {
                    c_->drawPoint(info.lastPoint, paint_);
                } else {
                    c_->drawPath(info.path, paint_);
                }
                info.path.reset();
                Tools::ResetRect(&info.r, info.startPoint.fX, info.startPoint.fY);
            }
            pointerIdList_.clear();
        }
    } else {
        if (curPointer_ >= 0) {
            c_->drawPath(path_, paint_);
            SkRect r = path_.getBounds();
            path_.reset();
            Tools::SkRect2ARect(rect_, r);
            curPointer_ = -1;
        }
    }
}
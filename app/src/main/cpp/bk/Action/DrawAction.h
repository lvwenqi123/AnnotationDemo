/*
 * DrawAction.h
 *
 *  Created on: 2018 陈刚
 *      Author: chengang
 */

#ifndef DRAWACTION_H_
#define DRAWACTION_H_
#include <memory>
#include "abstract.h"
#include "StrokeRecord.h"
#include "common.h"
#include "StrokeHandle.h"
#include "Record.h"
#include <queue>

typedef struct _RadiusCalculator{
    float factor;
    int id; //所属对象id
    int pointerId;
	_RadiusCalculator() {
        factor = 0;
		id = 0;
        pointerId = -1;
    }
    void SetValue(float fFactor) {
        factor = fFactor;
	}
	void SetReferObjId(int referId) {
		id = referId;
	}
	int GetReferObjId() {return id;}
    float Calculate(float pressure) {
        return (pressure * 0.2835f + 0.04725f) * factor;
    }
    void SetPointerId(int iPointerId) {pointerId = iPointerId;}
    int GetPointerId() const {return pointerId;}
    void Reset() {
        factor = -1;
        id = -1;
        pointerId = -1;
	}
}RadiusCalculator;
typedef std::shared_ptr<RadiusCalculator>RadiusCalculatorPtr;

typedef struct _SignPenParam{
	float lastDegree = -1;
	float lastW = -1;
	int cnt = 0;
	_SignPenParam() {
		lastDegree = -1;
		lastW = -1;
		cnt = 0;
	}
	void reset() {
		lastDegree = -1;
		lastW = -1;
		cnt = 0;
	}
}SignPenParam;

class DrawAction : public AbstractAction{
public:
	DrawAction(NativeWindowInterface *nativeWindowObj, int OperatorType, const uint32_t &strokeColor, const float &strokeWidth);
	~DrawAction();
	virtual void SetTouchEvent(int pointerId, int event)override;
	virtual void AddPoint(int pointerId, float x, float y, float size, int endFlag=1) override;
    virtual void Erase(ARect *r)override;
	virtual void Draw(SkCanvas *c) override;
    virtual void DrawByRect(SkCanvas *c, SkPaint &paint, SkRect r)override;
    virtual bool Tidy(AbstractActionPtr &obj) override;
	virtual void ScaleAndOffset(float sx, float sy, float dx, float dy)override;
	virtual void Offset(float dx, float dy) override;
	virtual void SetPenType(PenType penType) override;
    virtual void SetState(State state)override;
	virtual void Draw(RefreshMode pos, int shouldFlush)override;
    virtual int Redo()override;
    virtual int Undo()override;
	SkRect& GetBound()override;
    StrokeHandlePtr& GetStrokeHandle() {return strokeHandle_;}
	virtual void Draw2Screen(SkCanvas *c, SkRect &bound) override; /*直接刷新到屏幕*/
	inline virtual void SetBound(SkRect r) override {bound1_ = r;}
protected:
	void HandlePoint(int pointerId, float x, float y, float size, int endFlag);
	bool IsScreenModePortrait() {return (window_->GetScreenMode() == ScreenMode_Portrait);}
protected:
    StrokeHandlePtr strokeHandle_ = nullptr;
    int64_t lastMs_ = 0;
    float lastStrokeWidth_ = 0;
    StylusMeta lastsm_,lastsmfb_;
    bool record_ = true;
	static RadiusCalculator radiusFactor_;
	static SignPenParam signPen_;
	bool first = true;
	SkPath path_;
	std::vector<int>pointerIdList_;
	SkRect bound1_ = SkRect::MakeEmpty();
};
typedef std::shared_ptr<DrawAction>DrawActionPtr;



#endif /* DRAWACTION_H_ */

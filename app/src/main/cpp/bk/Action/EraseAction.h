/*
 * DrawAction.h
 *
 *  Created on: 2018年8月8日
 *      Author: chengang
 */

#ifndef ERASEACTION_H_
#define ERASEACTION_H_
#include <memory>
#include <list>
#include "SkiaLog.h"
#include "abstract.h"
#include "Record.h"
#include "StrokeHandle.h"

class EraseAction : public AbstractAction{
public:
	EraseAction(NativeWindowInterface *nativeWindowObj, int OperatorType, const int &strokeColor, const float &strokeWidth);
	~EraseAction();
	virtual void AddPoint(int pointerId, float x, float y, float size, int endFlag=1)override;
	virtual void Draw(RefreshMode mode, int shouldFlush = 0)override;
	virtual void Draw(SkCanvas *c)override;
	virtual void SetTouchEvent(int pointerId, int event)override;
	virtual void ScaleAndOffset(float sx, float sy, float dx, float dy)override;
	virtual int Redo()override;
	virtual int Undo()override;
	virtual void Draw2Screen(SkCanvas *c, SkRect &bound)override;
	virtual void SetEraseSize(int w, int h)override;

	virtual RecordMap& GetStrokeRecordMap() {return strokeHandle_->GetStrokesMap();}
    virtual void Erase(ARect *r)override;
    virtual bool Tidy(AbstractActionPtr &obj)override;
    void AddObj(AbstractActionPtr &obj, bool recordId);
    auto &GetObjs() {return eraseObjs_;}
    auto &GetObjsId(){return eraseObjIds_;};
    bool GetARect(ARect &r);
    bool Empty();
    StrokeHandlePtr& GetStrokeHandle() {return strokeHandle_;}
	virtual void SetBound(SkRect r) override {bound1_ = r;}


protected:
	virtual void DrawErase(RefreshMode mode, int shouldFlush = 0);
	void DrawEraseIcon(SkCanvas *c, ARect &icon);
	void AdjustEraseSize();
protected:
	int erasePicW_;
	int erasePicH_;
	std::list<ARect>rlist_; //橡皮擦运行轨迹，缓存队列
	int eraserPointer_ = -1;
	std::list<AbstractActionPtr> eraseObjs_;
	std::list<int>eraseObjIds_;
    StrokeHandlePtr strokeHandle_ = nullptr;
	ARect lastRect_;
	ARect eraseRect_;
	SkPath path_;
	SkRect bound1_ = SkRect::MakeEmpty();
};

typedef std::shared_ptr<EraseAction>EraseActionPtr;



#endif /* DRAWACTION_H_ */

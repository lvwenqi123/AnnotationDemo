#ifndef _WHITE_ABSTRACTACTION_H_
#define _WHITE_ABSTRACTACTION_H_

#include <memory>
#include <SkCanvas.h>
#include <SkPath.h>
#include <SkPaint.h>
#include <vector>
#include <iostream>
#include <string>
#include <SkPoint.h>
#include <map>
#include <list>
#include "Tools.h"
#include "common.h"
#include "NativeWindowInterface.h"
#include <SkDashPathEffect.h>
#include "LogUtil.h"
#include "MemoryPool.h"
#include "Operation0bjects.h"
#include "Painter.h"

class AbstractAction;
typedef std::shared_ptr<AbstractAction> AbstractActionPtr;
typedef std::list<AbstractActionPtr>ActionObjs;
typedef std::list<AbstractActionPtr>ActionList;
typedef std::list<AbstractActionPtr>AbstractActionList;
typedef std::vector<AbstractActionPtr>AbstractActionVecor;
typedef std::map<int, SkPath>MulPointPaths;
typedef std::map<unsigned int, AbstractActionPtr>AbstractActionMap;

static const int _MultiPointMaxNum = 10;

class AbstractAction : public Operation0bjects{
public:
	AbstractAction(NativeWindowInterface *nativeWindowObj, const int &operatorType, const int &strokeColor, const float &strokeWidth=5);
	virtual ~AbstractAction();
	virtual void Draw(SkCanvas *c) = 0;
	virtual void Draw(RefreshMode pos, int shouldFlush = 0);
    virtual void AddPoint(int pointerId, float x, float y, float size, int endFlag=1)=0;
	virtual void Erase(ARect *r) {}
	virtual bool Tidy(AbstractActionPtr &obj) {return false;}
    virtual bool SupportScaleAndOffset(){return support_;}
    virtual void Offset(float dx, float dy) override;
    virtual void ScaleAndOffset(float sx, float sy, float dx, float dy)override;
    virtual void DrawByRect(SkCanvas *c, SkPaint &paint, SkRect r)override;
    virtual void Draw2Screen(SkCanvas *c, SkRect &bound); /*直接刷新到屏幕*/

	virtual void SetTouchEvent(int pointerId, int event) {event_ = event;}
	inline virtual bool Locked() {return (state_ == State_Select);}
	inline virtual bool Closed() {return isClosed_;}
    inline virtual SkRect& GetBound() {return bound_;} //获取外接矩形
    inline virtual void SetBound(SkRect r) {bound_ = r;}

	inline virtual void SetPenType(PenType penType) {penType_ = penType;}
	inline virtual PenType GetPenType() {return penType_;}

	inline virtual int GetOperatorType() {return operatorType_;}
    inline virtual int GetId() {return id_;}	
    virtual void ResetPaint(const int operatorType);

	virtual void AssociatePainter(int blackBoardId);
	virtual void SetEraseSize(int w, int h){}
	virtual void SetViewRect(int boardId, SkRect &r);

	inline virtual void SetColor(int color) {
		color_ = color;
		paint_.setColor(color);
	}
	inline virtual void SetStrokeWidth(float width) {strokeWidth_ = width;}
	virtual int Redo() {return 0;}
	virtual int Undo() {return 0;}
	virtual bool CanDel() {return false;}

	virtual void SetUndoId(int id) {undoId_ = id;}
	inline virtual int GetUndoId() {return undoId_;}
	virtual void SetState(State state)override;
	virtual State GetState() override;
	static void Init(SkImageInfo &ii);
	static void UnInit();
	static unsigned long GetStrokeId() {return ++strokeId_;};
	static sk_sp<SkTypeface> &GetFont() {return font_;}
	static MemoryPool<BezierCurve, 4096> *GetMemPool() {return pool_;}
	static SkPaint &GetPaint() {return paint_;}
	static sk_sp<SkPathEffect> &GetPathEffect() {return pathEffect_;}
	static void LoadFont(int language);
	static void AddPainter(int boardId, int num);

protected:
    bool Point2Path(int pointerId, float x, float y, int endFlag=1);
	void DrawStroke(bool save);
	void SetPaint();
    bool CheckState(State state) const {return  (state_ == state);}
    void SetCurPointId(int pointerId) {curPointer_ = pointerId;}
    void ResetId(int pointerId);
private:
    bool Point2Path(MultiPointInfo &pointInfo, float x, float y, int endFlag, bool flag=true);
protected:
	static ARect rect_;
	static SkPaint paint_;
    static sk_sp<SkPathEffect> pathEffect_;
	static sk_sp<SkTypeface> font_;
	static PaintersPtr painters_;
protected:
	int event_ = MotionEvent_ACTION_Invalid;
	NativeWindowInterface *window_;
	int operatorType_ = OperatorType_Invalid;
	int curPointer_;
	SkBitmap *bg_ = NULL;
	bool isClosed_ = false;
	SkCanvas *c_ = NULL;
	unsigned int id_ = 0;
	float strokeWidth_ = 3;
	uint32_t color_ = 0;
	PenType penType_ = PenType_Invalid;
	bool support_ = true;
	bool first_ = true;
	int undoId_ = 0;
	PainterPtr painter_;
	static SkPoint ctrlPt_[3];
	static unsigned int objId_;
	static unsigned long strokeId_;
	State state_ = State_Normal; //每个对象的运行状态
	SkRect bound_ = {0,0,0,0};
	SkRect viewRect_;
    static pthread_mutex_t mutex_;
    static int8_t* cacheMem_;
	static SkBitmap cacheBmp_;
	static MemoryPool<BezierCurve, 4096> *pool_;
};
#endif

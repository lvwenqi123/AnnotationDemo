#ifndef _COMMON_H_
#define _COMMON_H_

#include "SkCanvas.h"
#include <SkImageEncoder.h>
#include "android/rect.h"
#include <vector>
#include <iostream>
#include <jni.h>
#include <list>
#include <forward_list>
#include <SkPath.h>
using namespace std;



const int _FB_WIDTH = 1920;
const int _FB_HEIGHT = 1080;
const int _FB_BUF_SIZE = _FB_WIDTH * _FB_HEIGHT * 4;

const int _BMP_W = 1920;
const int _BMP_H = 1080;

const int _refreshFrameBuf = 1;
const int __refreshWindow = 0;
static bool _debug = false;
static const float _Rad_to_deg  = 45.0 / atan(1.0);
static std::string _rootDir;
//const  std::string _workDir = "/DrawViewX/";
const  std::string _confDir = "/Pic/";
const  std::string _tmpDir =  "/Tmp/";
const  std::string _pdfDir =  "/Pdf";
const  std::string _cfgDir  = "/.Cfg/";
const  std::string _iwbDir  = "/Iwb/";
const  std::string _localBgDir  = "/LocalBg/";

const  std::string _pageDir = "Page/";
const  std::string _saveDir = "Save/";
const  std::string _capturePicPath = "/storage/emulated/0/temp/snap1920-1080-0.bmp";

const float _colorChangeSize0 = 0.5;
const float _colorChangeSize1 = 0.24;
const int _maxScaleValue = 300; //最大缩放倍数

const static std::string _strBgLocalPic = "localPic";   //本地图片/自定义图片
const static std::string _strBgDefault = "default";   //本地图片/自定义图片

const SkRect _ScreenArea[] = { //分屏
        {0, 0, 960, 1080},
        {960, 0, 1920, 1080},
};

typedef struct _SplitScreen{
	SkRect area;
	int color;
	bool flag;
	_SplitScreen() {
		color = 0;
	}
}SplitScreen;


#define FB_ON 1
#define FB_OFF 0


enum FileType{
	FileType_Pic,
	FileType_Iwb,
};
enum MotionEvent{
	MotionEvent_ACTION_DOWN = 0,
	MotionEvent_ACTION_UP,
	MotionEvent_ACTION_MOVE,
	MotionEvent_ACTION_CANCEL,
	MotionEvent_ACTION_OUTSIDE,
	MotionEvent_ACTION_POINTER_DOWN,
	MotionEvent_ACTION_POINTER_UP,
	MotionEvent_ACTION_Invalid
};

typedef enum _State{
    State_Normal = 0,
    State_Erase,
    State_Tidy,
    State_Select,
	State_Hide,
    State_Remove,
	State_PicRemark, //图片批注
	State_UpdateForm, //更新表格
	State_InForm, //插入表格
	State_SelectAndInForm,  //选中且插入表格
	State_HideAndInForm,    //隐藏且插入表格
	State_RemoveAndInForm,
	State_UndoByErase,
	State_RedoByErase,
	State_OnlyDrawForm,    //只画表格
}State;

enum DrawState{
	DrawState_Stop = 0,
	DrawState_Run,
	DrawState_RunNow,
	DrawState_Complete,
};
/*draw erase*/
enum OperatorType{
    OperatorType_Invalid = -1,
	OperatorType_Draw = 0,
	OperatorType_Erase,
	OperatorType_Mix,
	OperatorType_Bmp,
	OperatorType_Ruler,
	OperatorType_Select,
	OperatorType_Roam,
	OperatorType_Form,
	OperatorType_Text, //图形、手写等
};

enum ToolType{
	ToolType_Ruler = 1,
	ToolType_TriangleRuler,
};
enum Mode{
    Mode_Draw = 20,
    Mode_Erase,  //橡皮擦
	Mode_Select, //选择
	Mode_Roam,   //漫游
	Mode_Shape,  //图形模式
};

enum TouchBoxCode{
	TouchBoxCode_Iboard = 1000,
	TouchBoxCode_FuChuang,
};
//偏移值
typedef struct _Offset{
    float dx;
    float dy;
    _Offset() {
        dx = 0;
        dy = 0;
    }
}Offset;
typedef std::shared_ptr<Offset>OffsetPtr;

typedef struct _MovePosParam{
	float sx;      //放大x
	float sy;      //放大y
	float degree;  //旋转度数
	float rx;      //旋转点参考点x
	float ry;      //旋转点参考点y
	float dx;      //x轴平移偏移量
	float dy;      //y轴平移偏移量
    bool hide;
	int updateForm;
	_MovePosParam() {
		sx = 0;
		sy = 0;
		degree = 0;
		rx = 0;
		ry = 0;
		dx = 0;
		dy = 0;
        updateForm = -1;
        hide = false;
	}
	bool hasRotate() {return (degree != 0.00);}
}MovePosParam;
typedef std::shared_ptr<MovePosParam>MovePosParamPtr;

enum WhiteBoardMode{
	WhiteBoardMode_General = 0,
	WhiteBoardMode_Tool
};

enum MultiTouchState{
	MultiTouchState__Prepare = 0,
	MultiTouchState_Ready
};

enum RefreshMode{
	RefreshMode_All = 0, //ȫ��ˢ��
	RefreshMode_Part     //�ֲ�ˢ��
};

enum ThreadRunState{
	RunState_SavePic = 0,
	RunState_ClearTool,
	RunState_Running,
	RunState_Complete,
};

enum ScreenMode{/*横屏还是竖屏*/
	ScreenMode_Invalid = -1,
	ScreenMode_Landscape = 0, //横屏
	ScreenMode_Portrait  = 1,//竖屏
};

enum BmpResIdx{
	ResIdx_Close = 100,
	ResIdx_Stretch,
	ResIdx_Rotate,
	ResIdx_Ui,
};
enum PathType {
	PathType_Dir =0,
	PathType_File,
};

enum ActionGesture {
	ActionGesture_Invalid = -1,
	ActionGesture_Drag = 0x1,
	ActionGesture_Sacle,
	ActionGesture_Rotate,
	ActionGesture_Move,
	ActionGesture_DrawLine,
	ActionGesture_Close,
};

enum MethodIdx{
	MethodIdx_EnableFb = 0,
	MethodIdx_RefreshFb,
	MethodIdx_GetToolsRect,
	MethodIdx_CloseTool,
	MethodIdx_StopPoint,
	MethodIdx_ScalingFactor,
	MethodIdx_ShowButtons,
    MethodIdx_CloseButtons,
	MethodIdx_ChangeMode,
	MethodIdx_SaveProgress, //保存进度
	MethodIdx_SetMouseStyle,
};
struct PenProperty {
	SkScalar width;
	SkColor color;
};

struct SaveParam{
    int fileType;
    int value;
};

typedef struct _Line{
    SkPoint p1;
    SkPoint p2;
    _Line() {
        p1 = SkPoint::Make(0,0);
        p2 = SkPoint::Make(0,0);
    }
}Line;

struct BitmapInfo{
	int w;
	int h;
	int bufSize;
	void *buf;
	BitmapInfo() {
		w = 0;
		h = 0;
		bufSize = 0;
		buf = NULL;
	}
};

struct MultiPointInfo{
	SkPath path;
	ARect  r;
	SkPoint startPoint;
	SkPoint lastPoint;
	char losed; //丢点标志
	unsigned long id;
	int color;
	float strokeWidth;
    MultiPointInfo() {
        startPoint.set(0, 0);
        lastPoint.set(0,0);
        id = 0;
		strokeWidth = 1;
    }
    void reset() {
        startPoint.set(0, 0);
        lastPoint.set(0,0);
        id = 0;
        path.reset();
        color = 0;
		strokeWidth = 1;
    }
};

enum PenType{
    PenType_Invalid = -1,
    PenType_Pencil = 0,
    PenType_Text,
	PenType_Shape, //图形笔
	PenType_Pressure, //压感笔
	PenType_Sign,     //签字笔
	PenType_Math,     //数学
	PenType_Laser,    //激光笔
};

enum ShapeType {
    Shape_Invalid = -2,
	Shape_HWR = -1, //手写识别handwriting recognition(HWR)
	Shape_Circle = 0,
	Shape_Oval,
	Shape_GeneralIsoscelesTriangle,  //普通等腰三角形，即顶角是锐角或钝角
    Shape_Parallelogram,             //平行四边形
	Shape_RightTriangle,             //直角三角形
	Shape_Trapezoid,                 //梯形
    Shape_Rect,                     //矩形
	Shape_Rhombus,                  	//菱形
    Shape_Pentagon,                		//五边形
    Shape_Line,                    		//直线
	Shape_DoubleSidedArrow,       		 //双向箭头
	Shape_Arrowhead,              		//单向箭头
	Shape_ImaginaryLine,          		//虚线
	Shape_Curve = 13,                    //曲线
	Shape_RoundRect = 14,                //圆角矩形
    Shape_Triangle,                 //普通三角形
    Shape_Polygon,                  //多边形
    Shape_Cube,                     //立方体
    Shape_Cylinder,                 //圆柱体
    Shape_Sphere,                   //圆球体
    Shape_Hemisphere,               //半球体
    Shape_Cone,                     //圆锥体
    Shape_TriangularPyramid,        //三棱锥
	Shape_Quadrangular,             //四棱锥
    Shape_RoundTable,               //圆台
    Shape_Custom0,                  //自定义
	Shape_Arc,                      //弧形
    Shape_Angle,
    Shape_Func,
};

enum ShapeWindowLabelPos{
	ShapeWindowLabelPos_2dShape = 0,
	ShapeWindowLabelPos_3dShape,
	ShapeWindowLabelPos_bmp,
};

typedef struct _Point{
	float fX;
	float fY;
	char del;
	_Point() {
        fX = 0;
        fY = 0;
		del = 0;
	}
    void offset(float dx, float dy) {
        fX += dx;
        fY += dy;
    }
    void roateAndscale(SkMatrix &m){
        SkPoint p = m.mapXY(fX, fY);
        fX = p.fX;
        fY = p.fY;
	}

    void scaleAndoffset(float sx, float sy, float dx, float dy)
    {
        fX *= sx;
        fY *= sy;
        fX += dx;
        fY += dy;
    }
}Point;

typedef struct _BezierCurve{
	Point startPoint; //起点
	Point ctrlPoint; //控制点
	Point endPoint; //结束点
    SkRect bound; //外接矩形
    std::vector<Point> insertPt; //插的点
    char del;
    char insertPtDelCnt;
    char totalPtDelCnt;
    _BezierCurve() {
        del = 0;
        insertPtDelCnt = 0;
        totalPtDelCnt = 0;
    }
}BezierCurve;
typedef std::shared_ptr<BezierCurve>BezierCurvePtr;
struct BezierCurveRects {
    SkRect r;
    std::list<BezierCurve*>beziers;
	BezierCurveRects() {
		r = {0,0,0,0};
	}
};
typedef std::shared_ptr<BezierCurveRects>BezierCurveRectsPtr;
typedef std::list<BezierCurve*>BezierCurveList;

/*笔元素*/
typedef struct _StylusMeta{
	float x;
    float y;
    float radius;
    _StylusMeta() {
		x = 0;
		y = 0;
		radius = 0;
	}
	void SetValue(float fx, float fy, float fradius) {
        x = fx;
        y = fy;
        radius = fradius;
    }
	bool Equals(_StylusMeta &obj) {
		return (x == obj.x) && (y == obj.y) && (radius == obj.radius);
	}
}StylusMeta;
typedef std::shared_ptr<StylusMeta>StylusMetaPtr;

//ShowButtons 参数
typedef struct _ShowButtonsParam{
	int type;
	SkRect r;
	_ShowButtonsParam() {
		type = 0;
		r.setEmpty();
	}
}ShowButtonsParam;

//操作系统名称定义
enum SysName{
    SysName_Unknown = 0,
    SysName_Hi3751V560 = 1,
	SysName_RuiPingM7332 = 2, //mtk 安卓11板子
	SysName_LangGuo311d2,
	SysName_KJD311d2,
	SysName_RP982,
};

typedef void (CallVoidMethod)(jobject androidObj, jmethodID methodId, int methodIdx, void* param);
typedef std::list<std::string>StrList;
typedef std::vector<std::string>StrVec;
typedef std::list<int>IntList;
#endif

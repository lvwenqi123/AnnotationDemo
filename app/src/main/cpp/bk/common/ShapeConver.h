//
// Created by chengang on 2020/6/17.
//

#ifndef INC_811_INK_SHAPECONVER_H
#define INC_811_INK_SHAPECONVER_H
#include <string.h>
#include <iostream>
#include "common.h"
#include <sstream>

static const  std::string _shapeStr[] = {
        "circle",
        "ellipse",                   //椭圆
        "generalIsoscelesTriangle",  //普通等腰三角形，即顶角是锐角或钝角
        "parallelogram",             //平行四边形
        "rightTriangle",             //直角三角形
        "trapezoid",                 //梯形
        "rect",                     //矩形
        "rhombus",                    //菱形
        "pentagon",                        //五边形
        "line",                            //直线
        "doubleSidedArrow",             //双向箭头
        "arrowhead",                    //单向箭头
        "imaginaryLine",                //虚线
        "curve",                    //曲线
        "roundRect",                //圆角矩形
        "triangle",                 //普通三角形
        "polygon",                  //多边形
        "cube",                     //立方体
        "cylinder",                 //圆柱体
        "sphere",                   //圆球体
        "hemisphere",               //半球体
        "cone",                     //圆锥体
        "triangularPyramid",        //三棱锥
        "quadrangular",             //四棱锥
        "roundTable",               //圆台
        "custom"
};
/*形状转换*/
struct ShapeConvert{
    static std::string ShapeType2Str(ShapeType type)
    {
        std::stringstream ss;
        if (Shape_Triangle == type || Shape_RightTriangle == type || Shape_GeneralIsoscelesTriangle == type) {
            ss << "triangle";
        } else {
            ss << _shapeStr[type];
        }
        return ss.str();
    }
    static ShapeType Str2ShapeType(std::string &str)
    {
        int len = sizeof(_shapeStr) / sizeof(_shapeStr[0]);
        for (int i = 0; i < len; ++i) {
            if (str == _shapeStr[i]) {
                return (ShapeType)i;
            }
        }
        return Shape_Invalid;
    }

};

#endif //INC_811_INK_SHAPECONVER_H

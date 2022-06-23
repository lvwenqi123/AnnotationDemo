#ifndef FUNC_INFO_H_
#define FUNC_INFO_H_
#include <iostream>
#include <string>
#include "FuncUtils.h"
#include "SkiaLog.h"
#include <math.h>
#include <stdlib.h>

using namespace std;
//普通的函数
class FuncInfo {
protected:
    float a = 0;
    float b = 0;
    float c = 0;
    std::string funStr;
    int type;
    bool flag_ = false;
public:
    FuncInfo(string fun, int type) {
        this->funStr = fun;
        this->type = type;
    }

    FuncInfo(float a, float b, float c, int type) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->type = type;
        flag_ = true;
    }

     void Parse() {
        if (!flag_) {
            switch (type) {
                case 0: //一次函数
                    LinearFunction();
                    break;
                case 1: //2次函数
                    QuadraticFunction();
                    break;
                default:
                    break;
            }
        }
    }
    //一次函数分解
    void LinearFunction() {
        if (!funStr.empty()) {
            int len = funStr.length();
            int i = 0, j = 0;
            if (funStr.at(0) == 'y' && funStr.at(1) == '=') {
                i += 2;
                if (funStr.at(i) == 'x' || funStr.at(i) == 'X') {
                    a = 1;
                    i += 1;
                } else if (funStr.at(i) == '-' && funStr.at(i + 1) == 'x') {
                    a = -1;
                    i += 2;
                }
                while (i < len) {
                    if (FuncUtils::IsNumber(funStr.at(i))) { //如果遇到数字，要判断下一个字符是不是x
                        bool negative = false; //数字的正负
                        if (FuncUtils::IsNegative(funStr.at(i - 1))) {
                            negative = true;
                        }
                        j = i;
                        i += 1;
                        if (i < len) {
                            char tmp = funStr.at(i);
                            if (tmp == 'x' || tmp == 'X') { //遇到x说明是，ax
                                a = atof(funStr.substr(j, j + 1).c_str());
                                if (negative) {
                                    a = -a;
                                }
                            } else if (tmp == '+' || tmp == '-') { //遇到+ - 说明是b
                                float bb = atof(funStr.substr(j, j + 1).c_str());
                                if (negative) bb = -bb;
                                b += bb;
                            }
                        } else {
                            //根据前一个的的符号来判断b的正负
                            b = atof(funStr.substr(j, j + 1).c_str());
                            if (negative) b = -b;
                        }
                    }
                    i++;
                }
            }
        }
        INFO("a = %f, b = %f", a, b);
    }

    //2次函数
    void QuadraticFunction() {
        if (!funStr.empty()) {
            int len = funStr.length();
            int i = 0, j = 0;
            if (funStr.at(0) == 'y' && funStr.at(1) == '=') {
                i += 2;
                while (i < len) {
                    bool negative = false; //数字的正负
                    if (FuncUtils::IsNumber(funStr.at(i))) { //如果遇到数字，要判断下一个字符是不是x
                        if (FuncUtils::IsNegative(funStr.at(i - 1))) {
                            negative = true;
                        }
                        j = i;
                        i += 1;
                        if (i < len) {
                            char tmp = funStr.at(i);
                            if (tmp == 'x' || tmp == 'X') { //遇到x说明是，ax
                                i += 1;
                                if (funStr.at(i) == '^') {
                                    a = atof(funStr.substr(j, j + 1).c_str());
                                    if (negative) {
                                        a = -a;
                                    }
                                } else {
                                    b = atof(funStr.substr(j, j + 1).c_str());
                                    if (negative) {
                                        b = -b;
                                    }
                                }
                            } else if (tmp == '+' || tmp == '-') { //遇到+ - 说明是b
                                float bb = atof(funStr.substr(j, j + 1).c_str());
                                if (negative) bb = -bb;
                                b += bb;
                            }
                        } else {
                            //根据前一个的的符号来判断b的正负
                            c = atof(funStr.substr(j, j + 1).c_str());
                            if (negative) c = -c;
                        }
                    } else {
                        negative = FuncUtils::IsNegative(funStr.at(i - 1));
                        if (funStr.at(i) == 'x') {
                            if (i+1 < len && funStr.at(i+1) == '^') { //²
                                a = 1;
                                i += 1;
                                if (negative) a = -1;
                            } else {
                                b = 1;
                                if (negative) b = -1;
                            }
                        }
                    }
                    i++;
                }
            }
            //Log.i("cg", "a = " + a + " b = " + b + " c = " + c);
        }
    }

    float GetY(float x) {
        float y = 0;
        switch (type) {
            case 0: //一次函数
                y = a * x + b;
                break;
            case 1: //2次函数
                y = a * x * x + b * x + c;
                break;
            default:
                y = 0;
                break;
        }
        return y;
    }
};
#endif

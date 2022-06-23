//
// Created by iboard on 2022/6/18.
//

#ifndef ANNOTATEIMAGES_ACCELARATEDRAW_H
#define ANNOTATEIMAGES_ACCELARATEDRAW_H

#include <jni.h>
#include "FbAbstract.h"

class  AcclarateDraw{
public:
    static AcclarateDraw* GetInstance(); /*内部静态变量创建的单例模式，线程安全，代码量少*/
    static void DestroyInstance();
    void Init(int w, int h, int sys);
    void StartAccelerateDraw();
    void StopAccelerateDraw();
    void RefreshAccelerateDraw(int x, int y, int w, int h, uint8_t *pixel);
protected:
    static AcclarateDraw* ins_;
    int sys_;
    FbAbstractPtr fb_ = nullptr;
private:
    AcclarateDraw();
    ~AcclarateDraw();
};

#endif //ANNOTATEIMAGES_ACCELARATEDRAW_H

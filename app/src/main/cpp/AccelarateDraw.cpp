//
// Created by iboard on 2022/6/18.
//
#include "AccelarateDraw.h"
#include "SkiaLog.h"

AcclarateDraw*  AcclarateDraw::ins_ = nullptr;

AcclarateDraw* AcclarateDraw::GetInstance() {
    if (ins_ == nullptr) {
        ins_ = new AcclarateDraw();
    }
    return ins_;
}

void AcclarateDraw::DestroyInstance() {
    if (ins_ != nullptr) {
        delete ins_;
        ins_ = nullptr;
    }
}

AcclarateDraw::AcclarateDraw() {

}

AcclarateDraw::~AcclarateDraw() {
    fb_ = nullptr;
}

void AcclarateDraw::Init(int w, int h, int sys) {
    sys_ = sys;
    fb_ = FbFactory::CreateFb(sys);
    if (fb_ && fb_->InitFb(w, h)) {
        INFO("open fb successed!");
    }
}

void AcclarateDraw::StartAccelerateDraw() {
    if (fb_) fb_->EnableFb(1);
}

void AcclarateDraw::StopAccelerateDraw() {
    if (fb_) fb_->EnableFb(0);
}

void AcclarateDraw::RefreshAccelerateDraw(int x, int y, int w, int h, uint8_t *pixel) {
    if (fb_) fb_->RefreshAccelerateDraw(x, y, w, h, pixel);
}


//
// Created by iboard on 2021/12/15.
//

#ifndef MY_APPLICATION_SCREENCAPCLIENT_H
#define MY_APPLICATION_SCREENCAPCLIENT_H
#include <iostream>

typedef struct ImageInfo{
    int width;
    int height;
    int format;
    int stride;
    void *base;
    ImageInfo() {
        width = 0;
        height = 0;
        format = 0;
        stride = 0;
        base = nullptr;
    }
}ImageInfo;

class ScreenCapClient;
typedef std::shared_ptr<ScreenCapClient> ScreenCapClientPtr;
class ScreenCapClient{
public:
    virtual ~ScreenCapClient() = default;
    virtual int Capture(ImageInfo &image) = 0;
    virtual bool Wait(ImageInfo &image) = 0;
    virtual void ReleaseBuffer() = 0;
    virtual int GetState() = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
ScreenCapClient* GetObj();
#ifdef __cplusplus
}
#endif
#endif //MY_APPLICATION_SCREENCAPCLIENT_H

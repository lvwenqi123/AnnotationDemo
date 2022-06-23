/*
 * LogUtill.cpp
 *
 *  Created on: 2019年3月2日
 *      Author: chengang
 */
#include "LogUtil.h"
#include <android/log.h>




LogUtil::LogUtil()
{
	tag_ = "LogUtil";
}
LogUtil::LogUtil(const std::string &tag) {
	tag_ = tag;
}
LogUtil::~LogUtil() {
}

void LogUtil::T( const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_VERBOSE, tag_.c_str(), fmt, arg);
	va_end(arg);
}

void LogUtil::D( const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_DEBUG, tag_.c_str(), fmt, arg);
	va_end(arg);
}

void LogUtil::I( const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_INFO, tag_.c_str(), fmt, arg);
	va_end(arg);
}

void LogUtil::W(const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_WARN, tag_.c_str(), fmt, arg);
	va_end(arg);
}

void LogUtil::E( const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_ERROR, tag_.c_str(), fmt, arg);
	va_end(arg);
}

void LogUtil::TEST(const char *tag,  const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, arg);
	va_end(arg);
}
void LogUtil::SetTag(const std::string &tag)
{
	tag_ = tag;
}

void LogUtil::PrintF(const std::string &name, SkRect &r)
{
	E("name %s l = %f t = %f r = %f b = %f", name.c_str(), r.fLeft, r.fTop, r.fRight, r.fBottom);
}




void LOG_D(const char *tag,  const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_DEBUG, tag, fmt, arg);
	va_end(arg);
}

void LOG_I(const char *tag,  const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_INFO, tag, fmt, arg);
	va_end(arg);
}

void LOG_E(const char *tag,  const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, arg);
	va_end(arg);
}

void LOG_W(const char *tag,  const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	__android_log_vprint(ANDROID_LOG_WARN, tag, fmt, arg);
	va_end(arg);
}



/*
 * logUtil.hpp
 *
 *  Created on: 2017年4月11日
 *      Author: chengang
 */
#ifndef SKIALOG_HPP_
#define SKIALOG_HPP_
#include <android/log.h>
#define NDK_LOG 1
#define TAG "Skia"
#if NDK_LOG
#define __log_print_verbose(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define __log_print_debug(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define __log_print_info(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define __log_print_warn(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define __log_print_error(...)__android_log_print(ANDROID_LOG_ERROR, TAG , __VA_ARGS__)

#else
#define __log_print_verbose(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define __log_print_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define __log_print_info(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define __log_print_warn(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#define __log_print_error(fmt, ...) printf(fmt,  ##__VA_ARGS__)
#endif
//#define Trace(fmt, ...) __log_print_verbose(fmt, ##__VA_ARGS__)
#define DBG(fmt, ...) __log_print_debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) __log_print_info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) __log_print_warn(fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) __log_print_error(fmt, ##__VA_ARGS__)

#define ENTER_F DBG("Enter %s", __FUNCTION__);
#define EXIT_F DBG("Exit %s",__FUNCTION__);


#endif /* VLINKLOG_H_ */




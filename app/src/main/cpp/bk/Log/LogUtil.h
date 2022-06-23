/*
 * Log.h
 *
 *  Created on: 2019年3月2日
 *      Author: chengang
 */

#ifndef COMMON_LOG_LOGUTIL_H_
#define COMMON_LOG_LOGUTIL_H_
#include <iostream>
#include <stdarg.h>
#include <SkRect.h>
//#include <android/log.h>


class LogUtil{
public:
	LogUtil();
	LogUtil(const std::string &tag);
	virtual ~LogUtil();
	virtual void T( const char *fmt,...);
	virtual void D( const char *fmt,...);
	virtual void I( const char *fmt, ...);
	virtual void W( const char *fmt, ...);
	virtual void E( const char *fmt, ...);
	virtual void TEST(const char *tag,  const char *fmt, ...);
	virtual void SetTag(const std::string &tag);
	virtual const std::string& GetTag(){return tag_;}
	virtual void PrintF(const std::string &name, SkRect &r);
private:
	std::string tag_;
};


extern void LOG_D(const char *tag,  const char *fmt, ...);
extern void LOG_I(const char *tag,  const char *fmt, ...);
extern void LOG_E(const char *tag,  const char *fmt, ...);
extern void LOG_W(const char *tag,  const char *fmt, ...);

#endif /* COMMON_LOG_LOGUTIL_H_ */

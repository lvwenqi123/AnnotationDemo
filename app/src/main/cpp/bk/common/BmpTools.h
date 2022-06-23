/*
 * BmpTools.h
 *
 *  Created on: 2018��8��13��
 *      Author: chengang
 */

#ifndef BMPTOOLS_H_
#define BMPTOOLS_H_

#include <iostream>
#include <SkBitmap.h>
#include <DecodeFile.h>
#include <setjmp.h>
#include "jpeglib.h"
#include "cdjpeg.h"
#include "SkiaLog.h"
#include <SkCanvas.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  //error = (char*)myerr->pub.jpeg_message_table[myerr->pub.msg_code];
  ERR("jpeg_message_table[%d]:%s", myerr->pub.msg_code, myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
 // LOGE("addon_message_table:%s", myerr->pub.addon_message_table);
//  LOGE("SIZEOF:%d",myerr->pub.msg_parm.i[0]);
//  LOGE("sizeof:%d",myerr->pub.msg_parm.i[1]);
  longjmp(myerr->setjmp_buffer, 1);
}

struct BmpTools{

	static inline bool DecPic2Bmp(const std::string &path, SkBitmap *bmp)
	{
		return decode_file(path.c_str(), bmp);
	}

	static int generateJPEG(uint8_t* data, int w, int h, int quality,
			std::string &path, bool optimize) {
		//ENTER_F
		int nComponent = 3;
		const char *outfilename = path.c_str();
		struct jpeg_compress_struct jcs;

		my_error_mgr jem;

		jcs.err = jpeg_std_error(&jem.pub);
		jem.pub.error_exit = my_error_exit;
		if (setjmp(jem.setjmp_buffer)) {
			return 0;
		}
		jpeg_create_compress(&jcs);
		FILE* f = fopen(outfilename, "wb");
		if (f == NULL) {
			return 0;
		}
		jpeg_stdio_dest(&jcs, f);
		jcs.image_width = w;
		jcs.image_height = h;
		if (optimize) {
			//INFO("optimize==ture");
		} else {
			//INFO("optimize==false");
		}

		jcs.arith_code = false;
		jcs.input_components = nComponent;
		if (nComponent == 1)
			jcs.in_color_space = JCS_GRAYSCALE;
		else
			jcs.in_color_space = JCS_RGB;

		//ȫ������Ĭ�ϲ���/* Default parameter setup for compression */
		jpeg_set_defaults(&jcs);
		jcs.optimize_coding = optimize; //�Ƿ���ù����������ݼ��� Ʒ�����5-10��
		jpeg_set_quality(&jcs, quality, true);/*��������*/

		jpeg_start_compress(&jcs, TRUE); /*��ʼѹ�� �Ƿ�д��ȫ������*/

		JSAMPROW row_pointer[1];
		int row_stride;
		row_stride = jcs.image_width * nComponent;
		while (jcs.next_scanline < jcs.image_height) {
			row_pointer[0] = &data[jcs.next_scanline * row_stride];

			jpeg_write_scanlines(&jcs, row_pointer, 1);
		}

		if (jcs.optimize_coding) {
			//INFO("optimize==ture");
		} else {
			//INFO("optimize==false");
		}
		jpeg_finish_compress(&jcs);
		jpeg_destroy_compress(&jcs);
		fclose(f);
		//EXIT_F
		return 1;
	}

	static SkBitmap* ScaleBmp(SkBitmap *srcBmp, int newW, int newH) {
		if (srcBmp) {
			SkRect r = SkRect::MakeLTRB(0, 0, newW, newH);
			SkMatrix m1;
			m1.reset();
			SkBitmap *newBmp = new SkBitmap;
			SkImageInfo ii = SkImageInfo::Make(r.width(), r.height(), kRGBA_8888_SkColorType, kOpaque_SkAlphaType);
			newBmp->allocPixels(ii);
			float sx =  (float) r.width() / (float)srcBmp->width();
			float sy =  (float) r.height() / (float)srcBmp->height();
			m1.reset();
			m1.postScale(sx, sy, 0, 0);
			SkCanvas c(*newBmp);
			int saveCount0 = c.saveLayer(&r, nullptr); //
			if (saveCount0 >0) {
				c.setMatrix(m1);
				SkPaint paint;
				paint.setAntiAlias(true);
				paint.setFilterQuality(kLow_SkFilterQuality);
				c.drawBitmap(*srcBmp, 0, 0, &paint);
				c.restoreToCount(saveCount0);
			}
			return newBmp;
		}
		return nullptr;
	}
};




#endif /* BMPTOOLS_H_ */

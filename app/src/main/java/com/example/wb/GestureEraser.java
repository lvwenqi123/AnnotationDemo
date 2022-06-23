package com.example.wb;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Paint;

/**
 * Author: Wen Luo
 * Date: 2020/2/1211:04
 * Email: Wen.Luo@lango-tech.cn
 * 手势擦除模式，擦除面积随接触面积变化
 */

public class GestureEraser extends AEraser {
    private int preEraserWidth;
    private int preEraserHeight;
    private float w = 0;


    public GestureEraser(Paint paint){
        this.paint = new Paint(paint);
        this.paint.setStrokeWidth(1);
        this.paint.setStyle(Paint.Style.FILL);
        this.paint.setColor(Color.LTGRAY);
    }

    @Override
    protected void updateEraserIcon() {
        if(preEraserWidth == eraserWidth && preEraserHeight == eraserHeight) {
            return;
        }
        if(preEraserWidth != eraserWidth) {
            preEraserWidth = eraserWidth;
        }
        if(preEraserHeight != eraserHeight) {
            preEraserHeight = eraserHeight;
        }
    }
}

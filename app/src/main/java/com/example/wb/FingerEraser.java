package com.example.wb;

import android.content.Context;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Paint;

public class FingerEraser extends AEraser {
    public FingerEraser(Paint paint) {
        this.paint = new Paint(paint);
        this.paint.setStrokeWidth(1);
        this.paint.setStyle(Paint.Style.FILL);
        this.paint.setColor(Color.LTGRAY);
    }

    @Override
    protected void updateEraserIcon() {
    }
}

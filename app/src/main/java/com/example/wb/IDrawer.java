package com.example.wb;

import android.graphics.Rect;

/**
 * Author: Wen Luo
 * Date: 2020/2/1211:04
 * Email: Wen.Luo@lango-tech.cn
 */
public interface IDrawer {
    public Rect getCurrRect();
    public void touchDown(float x, float y);
    public boolean touchMove(float x, float y);
    public boolean touchUp(float x, float y);
    public void draw(WhiteBoard wb, boolean show);
}

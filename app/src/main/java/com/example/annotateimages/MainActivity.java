package com.example.annotateimages;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import com.example.sharecode.QRcodeshare;
import com.example.someutils.ShareUtils;
import com.example.view.AnnotateView;
import com.example.wb.Util;


public class MainActivity extends Activity implements View.OnClickListener{
    private AnnotateView draw;
    private ImageView side_pen,side_earse,side_clearscreen,side_save,side_share,side_exit;
    private ImageView side_pen1,side_earse1,side_clearscreen1,side_save1,side_share1,side_exit1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Init();
    }

    void Init() {
        draw = findViewById(R.id.image_canvas);
        side_pen = findViewById(R.id.side_pen);
        side_earse = findViewById(R.id.side_earse);
        side_clearscreen = findViewById(R.id.side_clearscreen);
        side_save = findViewById(R.id.side_save);
        side_share = findViewById(R.id.side_share);
        side_exit = findViewById(R.id.side_exit);
        side_pen1 = findViewById(R.id.side_pen1);
        side_earse1 = findViewById(R.id.side_earse1);
        side_clearscreen1 = findViewById(R.id.side_clearscreen1);
        side_save1 = findViewById(R.id.side_save1);
        side_share1 = findViewById(R.id.side_share1);
        side_exit1 = findViewById(R.id.side_exit1);

        side_pen.setOnClickListener(this);
        side_earse.setOnClickListener(this);
        side_clearscreen.setOnClickListener(this);
        side_save.setOnClickListener(this);
        side_share.setOnClickListener(this);
        side_exit.setOnClickListener(this);
        side_pen1.setOnClickListener(this);
        side_earse1.setOnClickListener(this);
        side_clearscreen1.setOnClickListener(this);
        side_save1.setOnClickListener(this);
        side_share1.setOnClickListener(this);
        side_exit1.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.side_pen:
                draw.GetWB().SetMode(Util.PEN);
                break;
            case R.id.side_earse:
                draw.GetWB().SetMode(Util.ERASER);
                break;
            case R.id.side_clearscreen:
                draw.GetWB().ClearScreen();
                break;
            case R.id.side_save:

                break;
            case R.id.side_share:

                break;
            case R.id.side_exit:

                break;
            case R.id.side_pen1:
                draw.GetWB().SetMode(Util.PEN);
                break;
            case R.id.side_earse1:
                draw.GetWB().SetMode(Util.ERASER);
                break;
            case R.id.side_clearscreen1:
                draw.GetWB().ClearScreen();
                break;
            case R.id.side_save1:

                break;
            case R.id.side_share1:
                ShareUtils.getInstance().getShowEmailDialog(MainActivity.this);
                break;
            case R.id.side_exit1:

                break;
        }
    }

    public void ChangeButtonAlpha() {
        float alpha = 1.0f;
        if (side_pen.getAlpha() != 0.98f) {
            alpha = 0.98f;
        }
        side_pen.setAlpha(alpha);
        side_earse.setAlpha(alpha);
        side_clearscreen.setAlpha(alpha);
    }



}
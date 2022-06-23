package com.example.annotateimages;

import androidx.annotation.IdRes;
import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.log.LogUtil;
import com.example.view.AnnotateView;
import com.example.wb.Util;

import java.util.ArrayList;

public class MainActivity extends Activity implements View.OnClickListener{
    private AnnotateView draw;
    private Button pen;
    private Button erase;
    private Button clearScreen;
    private ArrayList<Button>bts = new ArrayList<>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Init();
    }

    void InitButton(@IdRes int id) {
        Button btn = findViewById(id);
        btn.setOnClickListener(this);
        bts.add(btn);
    }

    void Init() {
        draw = findViewById(R.id.image_canvas);
        pen = findViewById(R.id.pen);
        pen.setOnClickListener(this);

        erase = findViewById(R.id.erase);
        erase.setOnClickListener(this);

        clearScreen = findViewById(R.id.clearScreen);
        clearScreen.setOnClickListener(this);
    }

    public void ChangeButtonAlpha() {
        float alpha = 1.0f;
        if (pen.getAlpha() != 0.98f) {
            alpha = 0.98f;
        }
        pen.setAlpha(alpha);
        erase.setAlpha(alpha);
        clearScreen.setAlpha(alpha);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.pen:
                draw.GetWB().SetMode(Util.PEN);
                break;
            case R.id.erase:
                draw.GetWB().SetMode(Util.ERASER);
                break;
            case R.id.clearScreen:
                draw.GetWB().ClearScreen();
                break;
        }

    }
}
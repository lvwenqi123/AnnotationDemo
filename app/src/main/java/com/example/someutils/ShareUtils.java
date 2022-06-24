package com.example.someutils;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;

import com.example.annotateimages.R;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class ShareUtils {
    private static volatile ShareUtils instance;
    private MyDialog myDialog;
    private LinearLayout linear_dialog_share_progress,linear_dialog_share_code;
    private ImageView image_share_pic;
    private TextView text_share_encrypt;

    public static ShareUtils getInstance() {
        if(instance == null) {
            synchronized (HttpUtils.class) {
                if (instance == null) {
                    instance = new ShareUtils();
                }
            }
        }
        return instance;
    }

    public void getShowEmailDialog(Activity activity){
        View view = activity.getLayoutInflater().inflate(R.layout.dialog_share_progress, null);
        myDialog = new MyDialog(activity, view, R.style.custom_dialog_style);
        myDialog.setCanceledOnTouchOutside(true);
        initShareViews(view);


        myDialog.show();
    }

    private void initShareViews(View shareView) {
        linear_dialog_share_progress = (LinearLayout) shareView.findViewById(R.id.linear_dialog_share_progress);
        linear_dialog_share_code = (LinearLayout) shareView.findViewById(R.id.linear_dialog_share_code);
        image_share_pic = (ImageView) shareView.findViewById(R.id.image_share_pic);
        text_share_encrypt = (TextView) shareView.findViewById(R.id.text_share_encrypt);
    }

    //广域网分享
    private void WANshare(String str) {
        /*File pic = new File(str);
        File[] fileArray = pic.listFiles();
        List<File> list = new ArrayList<File>();
        Log.e("HU", "str======" + str);
        ResultOssPicPathList = new ArrayList<String>();
        if (pic.isDirectory()) {
            if (null != fileArray && 0 != fileArray.length) {
                for (int i = 0; i < fileArray.length; i++) {
                    list.add(fileArray[i]);
                }
                uploadMettingPic(list, str, false, false, 0);
            }
        } else {
            list.add(pic);
            uploadMettingPic(list, str, false, false, 0);
        }*/
    }

}

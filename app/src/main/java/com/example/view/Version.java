package com.example.view;

import android.os.Environment;


import com.example.annotateimages.R;

import java.io.File;

public class Version {
    public static final char version_DW = 0;
    private static final String domain_PUBLIC  = "http://www.imagic-cloud.com";      //不包含公司名称的域名，中性版本
    private static int _softVersion = version_DW;
    public static String RootPath(){
        String root;
       root = Environment.getExternalStorageDirectory().getAbsolutePath() + "/BlackBoard"+"/";
        return root;
    }

    public static String GetDomain() {
        return domain_PUBLIC;
    }
}

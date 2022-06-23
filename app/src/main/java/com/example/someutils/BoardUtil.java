package com.example.someutils;

import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.net.ConnectivityManager;
import android.net.wifi.WifiManager;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.util.Xml;
import android.view.MotionEvent;
import android.view.View;

import com.example.view.Version;
import com.google.zxing.BarcodeFormat;
import com.google.zxing.EncodeHintType;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import com.google.zxing.qrcode.QRCodeWriter;

import org.json.JSONException;
import org.json.JSONObject;
import org.xmlpull.v1.XmlPullParser;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import okio.Buffer;
import okio.BufferedSource;

public class BoardUtil {





    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }
    public static boolean checkNetworkState(Context applicationContext)
    {
        boolean flag = false;
        //得到网络连接信息
        ConnectivityManager manager = (ConnectivityManager) applicationContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        //去进行判断网络是否连接
        if (manager.getActiveNetworkInfo() != null) {
            //flag = manager.getActiveNetworkInfo().isAvailable();//判断当前网络是否可用
            flag = manager.getActiveNetworkInfo().isConnected();//判断当前网络是否存在，并可用于数据传输

        }
        return flag;
    }

    public static File saveString(String str,String filename,String Path)
    {
        File wb =  null;
        if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
        {
            String path = Version.RootPath() + "/"+Path;
            File f = new File(path);
            BufferedOutputStream bos =null;
            if(!f.exists())
            {
                f.mkdirs();
            }
            wb = new File(f, filename);
            try {
                bos =new BufferedOutputStream(new FileOutputStream(wb));
                byte[] bs = str.getBytes();
                bos.write(bs,0,bs.length);
                bos.flush();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            catch (IOException e) {
                e.printStackTrace();
            }finally {
                if(bos != null)
                {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return wb;
    }


    public static void ossSetupload(final Context context, final String password, final String code)
            throws JSONException {
        Log.e("TAG=====", "ossSetupload: "+code+"==="+password);
        /*Message msg = new Message();
        msg.obj ="http://www.ouredustore.com:5000/ApiMeeting/SetupPassword?ExtractCode="+code+"&Password="+password;
        msg.what = 1;
        handler.sendMessage(msg);*/
        //获取后台数据
        String GET_GUIDE = "/ApiMeeting/SetupPassword";
        final JSONObject jsonObject = new JSONObject();
        jsonObject.put("ExtractCode", code);
        jsonObject.put("Password", password);
        HttpUtils.getInstance().postDataWithParame(GET_GUIDE, jsonObject, new HttpUtils.ResultCallback() {
            @Override
            public void onError(Request request, Exception e) {
                Log.e("TAG===","加密失败======请检查网络");
                //Toast.makeText(context,"加密失败",Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onResponse(Response response) throws IOException {
                Log.e("TAG===","扫码分享加密=====");
                ResponseBody responseBody = response.body();
                BufferedSource source = responseBody.source();
                try {
                    source.request(Long.MAX_VALUE); // request the entire body.
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Buffer buffer = source.buffer();
                final String guide = buffer.clone().readString(Charset.forName("UTF-8"));
                Log.e("TAG=====", "guide=====" + guide);
                try {
                    JSONObject guideJson = new JSONObject(guide);
                    boolean isSucceed = guideJson.getBoolean("isSucceed");
                    String message = guideJson.getString("message");
                    String isupgrade = guideJson.getString("entity");
                    Log.e("TAG=====", isSucceed + "===" + message + "===" + isupgrade);
                    /*Looper.prepare();
                    if(isSucceed){
                        Toast.makeText(context,"加密成功",Toast.LENGTH_SHORT).show();
                    }else {
                        Toast.makeText(context,"加密失败",Toast.LENGTH_SHORT).show();
                    }
                    Looper.loop();*/
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.e("TAG=====", "接口错误==e===" + e.toString());
                }
            }
        });
    }

    public static Bitmap generateBitmap(String content, int width, int height) {
        QRCodeWriter qrCodeWriter = new QRCodeWriter();
        Map<EncodeHintType, String> hints = new HashMap<EncodeHintType, String>();
        hints.put(EncodeHintType.CHARACTER_SET, "utf-8");
        try {
            BitMatrix encode = qrCodeWriter.encode(content, BarcodeFormat.QR_CODE, width, height, hints);
            encode = deleteWhite(encode);//删除白边
            width = encode.getWidth();
            height = encode.getHeight();
            int[] pixels = new int[width * height];
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (encode.get(j, i)) {
                        pixels[i * width + j] = 0x00000000;
                    } else {
                        pixels[i * width + j] = 0xFFF0F1F4;
                    }
                }
            }
            return Bitmap.createBitmap(pixels, 0, width, width, height, Bitmap.Config.RGB_565);
        } catch (WriterException e) {
            e.printStackTrace();
        }
        return null;
    }
    private static BitMatrix deleteWhite(BitMatrix matrix) {
        int[] rec = matrix.getEnclosingRectangle();
        int resWidth = rec[2] + 1;
        int resHeight = rec[3] + 1;

        BitMatrix resMatrix = new BitMatrix(resWidth, resHeight);
        resMatrix.clear();
        for (int i = 0; i < resWidth; i++) {
            for (int j = 0; j < resHeight; j++) {
                if (matrix.get(i + rec[0], j + rec[1]))
                    resMatrix.set(i, j);
            }
        }
        return resMatrix;
    }





    public static String packageCode(Context context) {
        PackageManager manager = context.getPackageManager();
        String code = "";
        try {
            PackageInfo info = manager.getPackageInfo(context.getPackageName(), 0);
            code = info.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return code;
    }

    public static void setSystemProperty(String key, String value) {
        try {
            Class<?> c = Class.forName("android.os.SystemProperties");
            Method set = c.getMethod("set", String.class, String.class);
            set.invoke(c, key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    public static String getSystemProperty(String property, String defaultValue) {
        try {
            Class clazz = Class.forName("android.os.SystemProperties");
            Method getter = clazz.getDeclaredMethod("get", String.class);
            String value = (String) getter.invoke(null, property);
            if (!TextUtils.isEmpty(value)) {
                return value;
            }
        } catch (Exception e) {

        }
        return defaultValue;
    }

    public static boolean isWifiApOpen(Context context) {
        try {
            WifiManager manager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            //通过放射获取 getWifiApState()方法
            Method method = manager.getClass().getDeclaredMethod("getWifiApState");
            //调用getWifiApState() ，获取返回值
            int state = (int) method.invoke(manager);
            //通过放射获取 WIFI_AP的开启状态属性
            Field field = manager.getClass().getDeclaredField("WIFI_AP_STATE_ENABLED");
            //获取属性值
            int value = (int) field.get(manager);
            //判断是否开启
            if (state == value) {
                return true;
            } else {
                return false;
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
        return false;
    }

    public  static int GetLocalServer() {
        Locale l = Locale.getDefault();
        String language = l.getCountry().toLowerCase();
        int local = 0;
        if (language != null){
            if (language.trim().equals("hk") || language.trim().equals("tw")
                    || language.trim().equals("ru")){
                local = 0;
            }else if(language.trim().equals("us") || language.trim().equals("ca")){
                local = 1;
            }else if(language.trim().equals("de") || language.trim().equals("gb")
                    || language.trim().equals("fr") || language.trim().equals("it")
                    || language.trim().equals("pl") || language.trim().equals("pt")
                    || language.trim().equals("es")){
                local = 2;
            }else if(language.trim().equals("au")){
                local = 3;
            }else if(language.trim().equals("jp")){
                local = 4;
            }else if(language.trim().equals("cn")){
                local = 5;
            }
        }
        return local;
    }


}
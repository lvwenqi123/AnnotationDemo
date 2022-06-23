package com.example.sharecode;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.provider.Settings;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;


import com.example.annotateimages.R;
import com.example.someutils.BoardUtil;
import com.example.someutils.HttpUtils;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import okio.Buffer;
import okio.BufferedSource;

public class QRcodeshare implements View.OnClickListener {

    private Context mContext;
    private PopupWindow QrWindow;
    private ImageView pen_btn;
    private View showView;
    private final String GetDomain = "http://www.imagic-cloud.com";
    private TextView number_0, number_1, number_2, number_3, number_4, number_5, number_6, number_7, number_8, number_9, number_cancel, number_ok, number_all;
    private LinearLayout back;
    private RelativeLayout qrcode;
    private ImageView zoom_big;
    private TextView show_share_status;
    private ImageView show_share_pic;
    private Switch pwd;
    private View view;
    private ProgressBar two_ball;
    private LinearLayout numberLayout;
    private Bitmap bigBmQRCode;
    private Bitmap smallBmQRCode;
    private String strNumber = "";
    private String codeString = "";
    private boolean isReUpload = false;
    private boolean isReCreatMetting = false;
    private int direction = 0;//左是0右是1
    //保存云端地址
    private final String SAVE_CLOUND_PATH = "/Picture/UploadPicture?region=";
    private final String CREATE_CLOUND_MEETING = "/ApiMeeting/CreateMeeting";
    private final String CLOUD_GET_MEETING = "/ApiMeeting/GetMeeting";
    private List<String> ResultOssPicPathList;
    private final int Time = 100;    //时间间隔，   单位 ms
    private Handler handler;


    public QRcodeshare(View showView, Context context, String str, int direction) {
        this.showView = showView;
        mContext = context;
        this.direction = direction;
        init();
        showShareCode(str);
    }

    private void init() {
        handler = new Handler() {

            @Override
            public void handleMessage(Message msg) {
                // TODO Auto-generated method stub
                super.handleMessage(msg);
                Log.e("progress:", "" + msg.what);

                switch (msg.what) {
                    case 1:
                        String url = msg.obj.toString();
                        smallBmQRCode = BoardUtil.generateBitmap(url, 180, 180);
                        bigBmQRCode = BoardUtil.generateBitmap(url, 500, 500);
                        show_share_pic.setImageBitmap(smallBmQRCode);
                        show_share_status.setVisibility(View.VISIBLE);
                        two_ball.setVisibility(View.GONE);
                        zoom_big.setVisibility(View.VISIBLE);
                        break;
                    case 2:
  /*if (loadialog != null && loadialog.isShowing()) {
                                loadialog.dismiss();
                            }*/
                        Toast.makeText(mContext,
                                mContext.getString(R.string.upload_error), Toast.LENGTH_SHORT).show();
                        if (QrWindow != null && QrWindow.isShowing()) {
                            two_ball.setVisibility(View.INVISIBLE);
                            show_share_status.setVisibility(View.VISIBLE);
                            show_share_status.setText(mContext.getString(R.string.unconnect));
                        }
                        break;
                    case 3:


                        break;
                }
            }
        };
    }

    private void showShareCode(String str) {
        View shareView = LayoutInflater.from(mContext).inflate(R.layout.qrcode_layout, null);
        QrWindow = new PopupWindow(shareView, BoardUtil.dip2px(mContext, 669), BoardUtil.dip2px(mContext, 282));

        setToolWindow(5);
        initShareViews(shareView);
        //������磨��ʾ��������߿����߳��ϴ���
        boolean isNetWork = BoardUtil.checkNetworkState(mContext.getApplicationContext());
        if (isNetWork) {//�п������磬�����߳�
            //generateQRCode1(msgHandler,str);
            //广域网分享
            WANshare(str);
        } else {
            Log.e("HU", "isNetWork=false");
            show_share_status.setVisibility(View.VISIBLE);
            show_share_status.setText(mContext.getResources().getString(R.string.unconnect));
        }
    }

    private void setToolWindow(int size) {

        QrWindow.setBackgroundDrawable(new BitmapDrawable());
        //QrWindow.setFocusable(false);
        //QrWindow.setOutsideTouchable(true);
        QrWindow.update();
        QrWindow.showAsDropDown(showView, BoardUtil.dip2px(mContext, size), 0);
    }

    private void initShareViews(View shareView) {
        qrcode = (RelativeLayout) shareView.findViewById(R.id.relativeLayout_right);
        back = (LinearLayout) shareView.findViewById(R.id.qrcode_back);
        zoom_big = (ImageView) shareView.findViewById(R.id.zoom_imageView);
        show_share_status = (TextView) shareView.findViewById(R.id.share_warn);
        show_share_pic = (ImageView) shareView.findViewById(R.id.share_pic);
        pwd = (Switch) shareView.findViewById(R.id.qrcode_pwd);
        two_ball = (ProgressBar) shareView.findViewById(R.id.two_ball);
        NumberPopInit(shareView);

    }

    private void NumberPopInit(View v) {
        numberLayout = (LinearLayout) v.findViewById(R.id.number_linearLayout);
        number_0 = (TextView) v.findViewById(R.id.number_0);
        number_1 = (TextView) v.findViewById(R.id.number_1);
        number_2 = (TextView) v.findViewById(R.id.number_2);
        number_3 = (TextView) v.findViewById(R.id.number_3);
        number_4 = (TextView) v.findViewById(R.id.number_4);
        number_5 = (TextView) v.findViewById(R.id.number_5);
        number_6 = (TextView) v.findViewById(R.id.number_6);
        number_7 = (TextView) v.findViewById(R.id.number_7);
        number_8 = (TextView) v.findViewById(R.id.number_8);
        number_9 = (TextView) v.findViewById(R.id.number_9);
        number_cancel = (TextView) v.findViewById(R.id.number_cancel);
        number_ok = (TextView) v.findViewById(R.id.number_ok);
        number_all = (TextView) v.findViewById(R.id.tx_number);
        number_0.setOnClickListener(new NumberClickLisenter());
        number_1.setOnClickListener(new NumberClickLisenter());
        number_2.setOnClickListener(new NumberClickLisenter());
        number_3.setOnClickListener(new NumberClickLisenter());
        number_4.setOnClickListener(new NumberClickLisenter());
        number_5.setOnClickListener(new NumberClickLisenter());
        number_6.setOnClickListener(new NumberClickLisenter());
        number_7.setOnClickListener(new NumberClickLisenter());
        number_8.setOnClickListener(new NumberClickLisenter());
        number_9.setOnClickListener(new NumberClickLisenter());
        number_ok.setOnClickListener(new NumberClickLisenter());
        back.setOnClickListener(this);
        zoom_big.setOnClickListener(this);
        number_cancel.setOnClickListener(new NumberClickLisenter());
        pwd.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                if (isChecked) {
                    numberLayout.setVisibility(View.VISIBLE);
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                        qrcode.setBackground(mContext.getDrawable(R.drawable.scan_qrcode_background));
                    }
                } else {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                        qrcode.setBackground(mContext.getDrawable(R.drawable.scan_qrcode_normal_background));
                    }
                    numberLayout.setVisibility(View.GONE);
                }
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.qrcode_back:
                QrWindow.dismiss();
                break;
            case R.id.zoom_imageView:
                AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                ImageView imageView = new ImageView(mContext);
                imageView.setScaleType(ImageView.ScaleType.FIT_XY);
                imageView.setImageBitmap((BoardUtil.generateBitmap(GetDomain + "/ApiMeeting/Meeting?code=" + codeString + "&region=" + BoardUtil.GetLocalServer(), 500, 500)));
                builder.setView(imageView);
                AlertDialog ad = builder.create();
                ad.getWindow().setType(WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY);
                ad.show();
                ad.getWindow().setLayout(500, 430);
                break;


        }
    }

    //广域网分享
    private void WANshare(String str) {
        //show_share_tips.setText(R.string.setting_qrcode_wan);
        File pic = new File(str);
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
        }
    }


    private String uploadMettingPic(final List<File> fileArray, final String meetingName, final boolean isExit, final boolean isShow, final int RoomType) {
        HttpUtils.getInstance().postPicWithParame(SAVE_CLOUND_PATH + BoardUtil.GetLocalServer(), fileArray.get(0), new HttpUtils.ResultCallback() {
            @Override
            public void onError(Request request, Exception e) {
                if (!isReUpload) {
                    isReUpload = true;
                    uploadMettingPic(fileArray, meetingName, isExit, isShow, RoomType);
                } else {
                    isReUpload = false;

                    Message msg = new Message();
                    msg.what = 2;
                    handler.sendMessage(msg);
                    Log.e("HU", "isReUpload =false");
                }
            }

            @Override
            public void onResponse(Response response) throws IOException {
                isReUpload = false;
                ResponseBody responseBody = response.body();
                BufferedSource source = responseBody.source();
                try {
                    source.request(Long.MAX_VALUE); // request the entire body.
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Buffer buffer = source.buffer();
                String result = buffer.clone().readString(Charset.forName("UTF-8"));
                try {
                    JSONObject resultJson = new JSONObject(result);
                    String statusCode = resultJson.getString("statusCode");
                    if (statusCode.equals("200")) {
                        ResultOssPicPathList.add(resultJson.getString("message"));
                        if (fileArray.size() != 1) {
                            fileArray.remove(0);
                            uploadMettingPic(fileArray, meetingName, isExit, isShow, RoomType);
                        } else {
                            JSONObject createMeetingJson = new JSONObject();
                            createMeetingJson.put("mac", Settings.System.getString(mContext.getContentResolver(), Settings.Secure.ANDROID_ID));
                            createMeetingJson.put("name", meetingName);
                            StringBuilder imagepath = new StringBuilder("");
                            for (int i = 0; i < ResultOssPicPathList.size(); i++) {
                                imagepath.append(ResultOssPicPathList.get(i));
                                if (i != ResultOssPicPathList.size() - 1) {
                                    imagepath.append(",");
                                }
                            }
                            createMeetingJson.put("imgpath", imagepath.toString());
                            createMeetingJson.put("filepath", "");
                            CreateMeeting(createMeetingJson, isShow, isExit, RoomType);
                        }


                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }

            }
        });
        return "";
    }


    class NumberClickLisenter implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            if (strNumber.length() > 4) {
                return;
            }
            switch (view.getId()) {
                case R.id.number_0:
                    number_all.setText(strNumber + number_0.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_1:
                    number_all.setText(strNumber + number_1.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_2:
                    number_all.setText(strNumber + number_2.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_3:
                    number_all.setText(strNumber + number_3.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_4:
                    number_all.setText(strNumber + number_4.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_5:
                    number_all.setText(strNumber + number_5.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_6:
                    number_all.setText(strNumber + number_6.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_7:
                    number_all.setText(strNumber + number_7.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_8:
                    number_all.setText(strNumber + number_8.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_9:
                    number_all.setText(strNumber + number_9.getText().toString());
                    strNumber = number_all.getText().toString();
                    break;
                case R.id.number_cancel:
                    strNumber = "";
                    number_all.setText("");
                    break;
                case R.id.number_ok:
                    if (strNumber.length() > 0) {
                        List<File> urls = new ArrayList<File>();
                        File fpwd = BoardUtil.saveString(strNumber, "key.txt", "imgs");
                        if (fpwd != null) {
                            urls.add(fpwd);
                        }
                        try {
                            BoardUtil.ossSetupload(mContext, strNumber, codeString);
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                        pwd.setChecked(false);
                        numberLayout.setVisibility(View.GONE);
                    }
                    break;
            }
        }
    }

    private void CreateMeeting(final JSONObject createMeetingJson, final boolean isShow, final boolean isExit, final int RoomType) {
        HttpUtils.getInstance().postDataWithParame(CREATE_CLOUND_MEETING, createMeetingJson, new HttpUtils.ResultCallback() {
            @Override
            public void onError(Request request, Exception e) {
                if (!isReCreatMetting) {
                    isReCreatMetting = true;
                    CreateMeeting(createMeetingJson, isShow, isExit, RoomType);
                    Message msg = new Message();
                    msg.what = 2;
                    handler.sendMessage(msg);
                    Log.e("HU", "isReCreatMetting =false");
                } else {
                    isReCreatMetting = false;
                }
            }

            @Override
            public void onResponse(Response response) throws IOException {
                isReCreatMetting = false;
                ResponseBody responseBody = response.body();
                BufferedSource source = responseBody.source();
                try {
                    source.request(Long.MAX_VALUE); // request the entire body.
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Buffer buffer = source.buffer();
                String result = buffer.clone().readString(Charset.forName("UTF-8"));
                try {
                    final JSONObject jsonObject = new JSONObject(result);
                    if (jsonObject.getBoolean("isSucceed")) {
                        codeString = jsonObject.getString("entity");
                        Message msg = new Message();
                        msg.obj = GetDomain + "/ApiMeeting/Meeting?code=" + codeString + "&region=" + BoardUtil.GetLocalServer();
                        msg.what = 1;
                        handler.sendMessage(msg);
                    } else {
                        Toast.makeText(mContext, mContext.getString(R.string.upload_error), Toast.LENGTH_SHORT).show();
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }

            }
        });
    }


}

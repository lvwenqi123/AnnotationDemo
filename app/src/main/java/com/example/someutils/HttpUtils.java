package com.example.someutils;

import android.os.Handler;
import android.util.Log;

import com.example.view.Version;

import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class HttpUtils {
    private static volatile HttpUtils instance;
    private static final String TAG = "HttpUtils";
    private String path;
    private OkHttpClient client;
    private Handler mHandler;
    private HttpUtils() {
        client = new OkHttpClient().newBuilder().connectTimeout(30, TimeUnit.SECONDS).
                readTimeout(60,TimeUnit.SECONDS).writeTimeout(60,TimeUnit.SECONDS).build();
        mHandler =new Handler();
    }


    public static HttpUtils getInstance() {
        if(instance == null) {
            synchronized (HttpUtils.class) {
                if (instance == null) {
                    instance = new HttpUtils();
                }
            }
        }
        return instance;
    }
    public void postDataWithParame(String local , JSONObject RequestJson, final ResultCallback callback) {
        MediaType JSON = MediaType.parse("application/json; charset=utf-8");//数据类型为json格式，
        Log.d(TAG," json 数据 " +RequestJson.toString());
        RequestBody body = RequestBody.create(JSON, RequestJson.toString());
        Request request = new Request.Builder()//创建Request 对象。
                .url(Version.GetDomain() + local)
                .post(body)//传递请求体
                .build();
        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {

                sendFailedCallback(call,e,callback);
            }
            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if(response.isSuccessful()){//回调的方法执行在子线程。
                    sendSuccessCallback(response,callback);

                }
            }
        });

    }
    public void postPicWithParame(String local , File file, final ResultCallback callback) {
        MediaType MEDIA_TYPE_PNG = MediaType.parse("image/png");
        MultipartBody.Builder builder = new MultipartBody.Builder().setType(MultipartBody.FORM);
        builder.addFormDataPart("img",file.getName(), RequestBody.create(MEDIA_TYPE_PNG, file));
        MultipartBody requestBody = builder.build();
        Request request = new Request.Builder()//创建Request 对象。
                .url(Version.GetDomain() + local)
                .post(requestBody)//传递请求体
                .build();
        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                sendFailedCallback(call, e, callback);
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {//回调的方法执行在子线程。
                    sendSuccessCallback(response, callback);

                }
            }
        });




    }
    public void DownloadImage(String url ,final ResultCallback callback){
        Request request = new Request.Builder()
                .url(url)
                .build();
        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                sendFailedCallback(call, e, callback);
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {//回调的方法执行在子线程。
                    sendSuccessCallback(response, callback);
                }
            }
        });

    }
    /**当请求失败时，都会调用这个方法
     * @param call
     * @param e
     * @param callback
     */
    private void sendFailedCallback(final Call call, final IOException e, final ResultCallback callback){
//        mHandler.post(new Runnable() {
//            @Override
//            public void run() {
//                Log.i(TAG,"当前线程："+Thread.currentThread().getName());
        if (callback != null){
            callback.onError(call.request(),e);
        }
//            }
//        });
    }

    /**请求成功调用该方法
     * @param response  返回的数据
     * @param callback 回调的接口
     */
    private void sendSuccessCallback(final Response response, final ResultCallback callback) {
//        mHandler.post(new Runnable() {
//            @Override
//            public void run() {
//                Log.i(TAG, "当前线程：" + Thread.currentThread().getName());
        if (callback != null) {
            try {
                callback.onResponse(response);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
//            }
//        });
    }
    //创建接口，回调给调用者
    public interface ResultCallback{
        void onError(Request request, Exception e);
        void onResponse(Response response) throws IOException;
    }

}

package com.example.runzhe.ticket;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.text.TextUtils;
import android.view.Gravity;
import android.widget.Adapter;
import android.widget.Toast;

import com.zaaach.citypicker.CityPicker;
import com.zaaach.citypicker.adapter.OnPickListener;
import com.zaaach.citypicker.model.City;
import com.zaaach.citypicker.model.HotCity;
import com.zaaach.citypicker.model.LocatedCity;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import es.dmoral.toasty.Toasty;

public class Tools {

    static String command(String command) throws Exception{

        // TODO : 使用 client
        return Client.main(command);
//        return new String("张三 zhangsan@sjtu.edu.cn 12345678901 2");
    }

    static String getSeatType(int pos){
        if(pos >= 11) return "未知席别";
        String[] seatType = {"商务座", "一等座", "二等座", "特等座", "硬座", "软座", "无座", "硬卧", "软卧", "动卧", "高级软卧"};
        return seatType[pos];
    }

    static String getTrainCatalogs(int pos){
        if(pos >= 7) return "未知席别";
        String[] trainType = {"T", "Z", "O", "G", "D", "K", "C"};
        return trainType[pos];
    }

    static  boolean isNonNegtiveInteger(String string){
        Pattern pattern = Pattern.compile("[0-9]*");
        Matcher isNum = pattern.matcher(string);
        return isNum.matches();
    }

    static boolean isEmpty(String string){
        return TextUtils.isEmpty(string);
    }

    static boolean isEmail(String string){
        if (string == null) return false;
        String regEx1 = "^([a-z0-9A-Z]+[-|\\.]?)+[a-z0-9A-Z]@([a-z0-9A-Z]+(-[a-z0-9A-Z]+)?\\.)+[a-zA-Z]{2,}$";
        Pattern p; Matcher m;
        p = Pattern.compile(regEx1);
        m = p.matcher(string);
        return m.matches();
    }

    static boolean isPhone(String string){
        return isNonNegtiveInteger(string) && string.length() == 11;
    }

    static int getRandomInteger(){
        Random random = new Random();
        return random.nextInt();
    }

    static String getNthSubstring(String string, String regex, int index){ // 0 base
        String[] strArray = string.split(" ");
        return strArray[index];
    }

    static void showMessage(Context context, String message, String type){

        switch (type){
            case "error" :
                Toasty.error(context, message, Toast.LENGTH_SHORT).show();
                break;
            case "success" :
                Toasty.success(context, message, Toast.LENGTH_SHORT).show();
                break;
            case "info" :
                Toasty.info(context, message, Toast.LENGTH_SHORT).show();
                break;
            case "warning" :
                Toasty.warning(context, message, Toast.LENGTH_SHORT).show();
                break;
        }
    }
    static void showMessage(final Context context, Activity ac, final String message, final String type){
        ac.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (type) {
                    case "error":
                        Toasty.error(context, message, Toast.LENGTH_SHORT).show();
                        break;
                    case "success":
                        Toasty.success(context, message, Toast.LENGTH_SHORT).show();
                        break;
                    case "info":
                        Toasty.info(context, message, Toast.LENGTH_SHORT).show();
                        break;
                    case "warning":
                        Toasty.warning(context, message, Toast.LENGTH_SHORT).show();
                        break;
                }
            }
        });
    }

    static String getStandardDate(String num){
        return num.length() == 2 ? num : "0" + num;
    }

}

package com.example.runzhe.ticket;

import android.content.Context;
import android.text.TextUtils;
import android.view.Gravity;
import android.widget.Adapter;
import android.widget.Toast;

import java.util.Random;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tools {

    static void toastMessage(Context context, String string){
        Toast toast = Toast.makeText(context, string, Toast.LENGTH_LONG);
        toast.setGravity(Gravity.BOTTOM, 0, 100);
        toast.show();
    }

    static  boolean isNonNegtiveInteger(String string){
        Pattern pattern = Pattern.compile("[0-9]*");
        Matcher isNum = pattern.matcher(string);
        if( !isNum.matches() ){
            return false;
        }
        return true;
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
        if (m.matches())  return true;
        else  return false;
    }

    static boolean isPhone(String string){
        return isNonNegtiveInteger(string) && string.length() == 11;
    }

    static int getRandomInteger(){
        Random random = new Random();
        return random.nextInt();
    }

    static String getNthSubstring(String string, String regex, int index){
        String[] strArray = string.split(" ");
        return strArray[index];
    }

}

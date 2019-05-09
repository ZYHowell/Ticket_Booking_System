package com.example.runzhe.ticket;

import android.content.Context;
import android.text.TextUtils;
import android.view.Gravity;
import android.widget.Toast;

import java.util.Random;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Tools {

    static public void toastMessage(Context context, String string){
        Toast toast = Toast.makeText(context, string, Toast.LENGTH_LONG);
        toast.setGravity(Gravity.BOTTOM, 0, 100);
        toast.show();
    }

    static  public boolean isNonNegtiveInteger(String string){
        Pattern pattern = Pattern.compile("[0-9]*");
        Matcher isNum = pattern.matcher(string);
        if( !isNum.matches() ){
            return false;
        }
        return true;
    }

    static public boolean isEmpty(String string){
        return TextUtils.isEmpty(string);
    }

    static public int getRandomInteger(){
        Random random = new Random();
        return random.nextInt();
    }

}

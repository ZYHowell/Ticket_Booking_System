package com.example.runzhe.ticket;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.app.LoaderManager.LoaderCallbacks;

import android.content.CursorLoader;
import android.content.Loader;
import android.database.Cursor;
import android.net.Uri;
import android.os.AsyncTask;

import android.os.Build;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import es.dmoral.toasty.Toasty;

public class LoginActivity extends AppCompatActivity {

    EditText userid_edit;
    EditText password_edit;
    ImageView userid_clear;
    ImageView password_clear;
    Button login_button;
    Button register_button;

    private ProgressbarFragment progressbarFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        findAllView();

        login_button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                attemptLogin();
            }
        });
        register_button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(LoginActivity.this, RegisterActivity.class);
                startActivity(intent);
            }
        });
        userid_clear.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                userid_edit.setText("");
            }
        });
        password_clear.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                password_edit.setText("");
            }
        });

    }

    private void attemptLogin() {

        String userid = userid_edit.getText().toString();
        String password = password_edit.getText().toString();

        // 用户名为空
        if (TextUtils.isEmpty(userid)) {
            Tools.showMessage(this, "ID不能为空！", "error");
            return;
        }
        // 密码为空
        if (TextUtils.isEmpty(password)) {
            Tools.showMessage(this, "密码不能为空！", "error");
            return;
        }

        try {
            progressbarFragment = new ProgressbarFragment();
            progressbarFragment.setCancelable(false);
            progressbarFragment.show(getSupportFragmentManager());
            sendRequest(userid, password);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    private void sendRequest(final String userid, final String password){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "login" + " " + userid + " " + password;
                    String result = Tools.command(command);
                    if(result.equals("1")){
                        Tools.showMessage(LoginActivity.this, LoginActivity.this, "登录成功", "success");
                        Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                        intent.putExtra("userid", userid);
                        progressbarFragment.dismiss();
                        startActivity(intent);
                        finish();
                    }
                    else{
                        Tools.showMessage(LoginActivity.this, LoginActivity.this, "ID不存在或密码错误！", "error");
                        progressbarFragment.dismiss();
                    }
                } catch (Exception e) {
                    Tools.showMessage(LoginActivity.this, LoginActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void findAllView(){
        userid_edit = (EditText) findViewById(R.id.loginUserid_Edit);
        password_edit = (EditText) findViewById(R.id.loginPassword_Edit);
        userid_clear = (ImageView) findViewById(R.id.loginUserid_Clear);
        password_clear = (ImageView) findViewById(R.id.loginPassword_Clear);
        login_button = (Button) findViewById(R.id.login_LoginButton);
        register_button = (Button) findViewById(R.id.login_RegisterButton);
    }
}


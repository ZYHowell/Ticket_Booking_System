package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import es.dmoral.toasty.Toasty;

public class RegisterActivity extends AppCompatActivity {

    private EditText username_edit;
    private EditText password_edit;
    private EditText password2_edit;
    private EditText email_edit;
    private EditText phone_edit;
    private Button register_btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        findAllView();

        register_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String username = username_edit.getText().toString();
                String email = email_edit.getText().toString();
                String phone = phone_edit.getText().toString();
                String password = password_edit.getText().toString();
                String password2 = password2_edit.getText().toString();

                // 用户名为空
                if (TextUtils.isEmpty(username)) {
                    Tools.showMessage(RegisterActivity.this, "用户名不能为空！", "error");
                    return;
                }
                // 电话为空
                if (TextUtils.isEmpty(phone)) {
                    Tools.showMessage(RegisterActivity.this, "手机号码不能为空！", "error");
                    return;
                }
                // 密码为空
                if (TextUtils.isEmpty(password)) {
                    Tools.showMessage(RegisterActivity.this, "密码不能为空！", "error");
                    return;
                }
                // 密码不一致
                if (!password.equals(password2)) {
                    Tools.showMessage(RegisterActivity.this, "两次输入密码不一致！", "error");
                    return;
                }
                // 邮箱格式不正确
                if (!Tools.isEmail(email)) {
                    Tools.showMessage(RegisterActivity.this, "邮箱格式有误！", "error");
                    return;
                }
                // 手机号码格式不正确
                if (!Tools.isPhone(phone)) {
                    Tools.showMessage(RegisterActivity.this, "手机号码有误！", "error");
                    return;
                }

                // TODO : 后端检查能否注册，并返回ID
                boolean success = true;
                int id = 2333;

                if(success){
                    Tools.showMessage(RegisterActivity.this, "注册成功，你的ID是" + id + "，请重新登录！", "success");
                    finish();
                }
                else
                    Tools.showMessage(RegisterActivity.this, "注册失败！", "error");

            }
        });

    }

    void findAllView(){
        username_edit = (EditText)findViewById(R.id.registerUsername_Edit);
        password_edit = (EditText)findViewById(R.id.registerPassword_Edit);
        password2_edit = (EditText)findViewById(R.id.registerConfirmPassword_Edit);
        email_edit = (EditText)findViewById(R.id.registerEmail_Edit);
        phone_edit = (EditText)findViewById(R.id.registerPhone_Edit);
        register_btn = (Button)findViewById(R.id.register_Button);
    }

}

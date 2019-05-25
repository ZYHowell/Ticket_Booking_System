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

    private ProgressbarFragment progressbarFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        findAllView();

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        register_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String username = username_edit.getText().toString();
                String email = email_edit.getText().toString();
                String phone = phone_edit.getText().toString();
                String password = password_edit.getText().toString();
                String password2 = password2_edit.getText().toString();

                // 用户名为空
                if (TextUtils.isEmpty(username) || username.contains(" ")) {
                    Tools.showMessage(RegisterActivity.this, "用户名不合法！", "error");
                    return;
                }
                // 电话为空
                if (TextUtils.isEmpty(phone) || phone.contains(" ")) {
                    Tools.showMessage(RegisterActivity.this, "手机号码不合法！", "error");
                    return;
                }
                // 密码为空
                if (TextUtils.isEmpty(password) || password.contains(" ")) {
                    Tools.showMessage(RegisterActivity.this, "密码不合法！", "error");
                    return;
                }
                // 密码不一致
                if (!password.equals(password2)) {
                    Tools.showMessage(RegisterActivity.this, "两次输入密码不一致！", "error");
                    return;
                }
                // 邮箱格式不正确
                if (!Tools.isEmail(email) || email.contains(" ")) {
                    Tools.showMessage(RegisterActivity.this, "邮箱不合法！", "error");
                    return;
                }
                // 手机号码格式不正确
                if (!Tools.isPhone(phone)) {
                    Tools.showMessage(RegisterActivity.this, "手机号码不合法！", "error");
                    return;
                }

                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getSupportFragmentManager());
                    sendRequest(username, password, email, phone);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

    }

    private void sendRequest(final String username, final String password, final String email, final String phone){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "register" + " " + username + " " + password + " " + email + " " + phone;
                    String result = Tools.command(command);
                    if(result.equals("-1")){
                        Tools.showMessage(RegisterActivity.this, RegisterActivity.this, "注册失败！", "error");
                        progressbarFragment.dismiss();
                    }
                    else{
                        Tools.showMessage(RegisterActivity.this, RegisterActivity.this,
                                "注册成功！\n你的ID是 " + result, "success");
                        progressbarFragment.dismiss();
                        finish();
                    }
                } catch (Exception e) {
                    Tools.showMessage(RegisterActivity.this, RegisterActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void findAllView(){
        username_edit = (EditText)findViewById(R.id.registerUsername_Edit);
        password_edit = (EditText)findViewById(R.id.registerPassword_Edit);
        password2_edit = (EditText)findViewById(R.id.registerConfirmPassword_Edit);
        email_edit = (EditText)findViewById(R.id.registerEmail_Edit);
        phone_edit = (EditText)findViewById(R.id.registerPhone_Edit);
        register_btn = (Button)findViewById(R.id.register_Button);
    }


    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

}

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

public class RegisterActivity extends AppCompatActivity {

    private EditText mUsernameView;
    private EditText mEmailView;
    private EditText mPhoneView;
    private EditText mPasswordView;
    private EditText mPasswordAgainView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        mUsernameView = (EditText)findViewById(R.id.r_username);
        mEmailView = (EditText)findViewById(R.id.r_email);
        mPhoneView = (EditText)findViewById(R.id.r_phone);
        mPasswordView = (EditText)findViewById(R.id.r_password);
        mPasswordAgainView = (EditText)findViewById(R.id.r_password_again);

        Button mRegisterButton = (Button)findViewById(R.id.r_username_confirm_button);
        mRegisterButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String username = mUsernameView.getText().toString();
                String email = mEmailView.getText().toString();
                String phone = mPhoneView.getText().toString();
                String password = mPasswordView.getText().toString();
                String password2 = mPasswordAgainView.getText().toString();

                // 用户名为空
                if (TextUtils.isEmpty(username)) {
                    mUsernameView.setError("用户名不能为空！");
                    mUsernameView.requestFocus();
                    return;
                }
                // 邮箱为空
                if (TextUtils.isEmpty(email)) {
                    mEmailView.setError("邮箱不能为空！");
                    mEmailView.requestFocus();
                    return;
                }
                // 电话为空
                if (TextUtils.isEmpty(phone)) {
                    mPhoneView.setError("手机号码不能为空！");
                    mPhoneView.requestFocus();
                    return;
                }
                // 密码为空
                if (TextUtils.isEmpty(password)) {
                    mPasswordView.setError("密码不能为空！");
                    mPasswordView.requestFocus();
                    return;
                }
                // 密码不一致
                if (!password.equals(password2)) {
                    mPasswordAgainView.setError("两次输入密码不一致！");
                    mPasswordAgainView.requestFocus();
                    return;
                }

                // TODO : 后端检查能否注册

                Toast toast = Toast.makeText(getApplicationContext(), "注册成功，请重新登陆！", Toast.LENGTH_LONG);
                toast.setGravity(Gravity.BOTTOM, 0, 100);
                toast.show();

                finish();

            }
        });

    }
}

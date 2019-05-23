package com.example.runzhe.ticket;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class ModifyUserInfoActivity extends AppCompatActivity {

    private EditText username_edit;
    private EditText password_edit;
    private EditText password2_edit;
    private EditText email_edit;
    private EditText phone_edit;
    private Spinner privilege_spinner;
    private Button save_button;
    private TextView a_hint;

    ArrayAdapter<String> arrayAdapter;
    List<String> privilege_list;

    private ProgressbarFragment progressbarFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_modify_user_info);
        findAllView();

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        privilege_list = new ArrayList<String>();
        privilege_list.add(getResources().getString(R.string.privilege_user));
        privilege_list.add(getResources().getString(R.string.privilege_admin));
        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, privilege_list);
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        privilege_spinner.setAdapter(arrayAdapter);

        Intent intent = getIntent();
        a_hint.setText("提示：您正在修改ID为" + intent.getStringExtra("id") + "的用户。");
        username_edit.setText(intent.getStringExtra("name"));
        email_edit.setText(intent.getStringExtra("email"));
        phone_edit.setText(intent.getStringExtra("phone"));
        privilege_spinner.setSelection(Integer.valueOf(intent.getStringExtra("privilege")) - 1);

        save_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String userid = getIntent().getStringExtra("id");
                String username = username_edit.getText().toString();
                String email = email_edit.getText().toString();
                String phone = phone_edit.getText().toString();
                String password = password_edit.getText().toString();
                String password2 = password2_edit.getText().toString();
                String privilege = getIntent().getStringExtra("privilege");

                // 用户名为空
                if (TextUtils.isEmpty(username)) {
                    Tools.showMessage(ModifyUserInfoActivity.this, "用户名不能为空！", "error");
                    return;
                }
                // 电话为空
                if (TextUtils.isEmpty(phone)) {
                    Tools.showMessage(ModifyUserInfoActivity.this, "手机号码不能为空！", "error");
                    return;
                }
                // 密码不一致
                if (!password.equals(password2)) {
                    Tools.showMessage(ModifyUserInfoActivity.this, "两次输入密码不一致！", "error");
                    return;
                }
                // 邮箱格式不正确
                if (!Tools.isEmail(email)) {
                    Tools.showMessage(ModifyUserInfoActivity.this, "邮箱格式有误！", "error");
                    return;
                }
                // 手机号码格式不正确
                if (!Tools.isPhone(phone)) {
                    Tools.showMessage(ModifyUserInfoActivity.this, "手机号码有误！", "error");
                    return;
                }

                progressbarFragment = new ProgressbarFragment();
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getSupportFragmentManager());
                    sendRequest(userid, username, password, email, phone, privilege);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                // TODO : 后端检查能否保存，注意这时候可以不改密码，记得加进度条

                boolean success = true;
                if(success)
                    Tools.showMessage(ModifyUserInfoActivity.this, "保存成功", "success");
                else
                    Tools.showMessage(ModifyUserInfoActivity.this, "保存失败！", "error");

            }
        });

    }

    private void sendRequest(final String userid, final String username, final String password, final String email, final String phone, final String privilege){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = !Tools.isEmpty(password)
                        ? "modify_profile" + " " + userid + " " + username + " " + password + " " + email + " " + phone + " " + privilege
                        : "modify_profile_without_password" + " " + userid + " " + username + " " + email + " " + phone + " " + privilege;
                    String result = Tools.command(command);
                    if(result.equals("0")){
                        progressbarFragment.dismiss();
                        Tools.showMessage(ModifyUserInfoActivity.this, ModifyUserInfoActivity.this, "修改失败！", "error");
                    }
                    else{
                        progressbarFragment.dismiss();
                        Tools.showMessage(ModifyUserInfoActivity.this, ModifyUserInfoActivity.this, "修改成功！", "success");
                    }
                } catch (Exception e) {
                    Tools.showMessage(ModifyUserInfoActivity.this, ModifyUserInfoActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void findAllView(){
        a_hint = findViewById(R.id.a_hint);
        username_edit = findViewById(R.id.registerUsername_Edit);
        password_edit = findViewById(R.id.registerPassword_Edit);
        password2_edit = findViewById(R.id.registerConfirmPassword_Edit);
        email_edit = findViewById(R.id.registerEmail_Edit);
        phone_edit = findViewById(R.id.registerPhone_Edit);
        privilege_spinner = findViewById(R.id.registerPrivilege_Spinner);
        save_button = findViewById(R.id.save_Button);
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }
}

package com.example.runzhe.ticket;

import android.content.Intent;
import android.media.Image;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import es.dmoral.toasty.Toasty;

public class Fragment3 extends Fragment {

    View view;

    TextView id_text;
    EditText username_edit;
    EditText email_edit;
    EditText phone_edit;
    EditText password_edit;
    EditText password_confirm_edit;
    TextView privilege_text;
    Button save_btn;

    String userid, username, email, phone, privilege;

    private ProgressbarFragment progressbarFragment;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_3, container, false);
        findAllView();

        userid = getActivity().getIntent().getStringExtra("userid");
        progressbarFragment = new ProgressbarFragment();
        try {
            progressbarFragment = new ProgressbarFragment();
            progressbarFragment.setCancelable(false);
            progressbarFragment.show(getActivity().getSupportFragmentManager());
            sendRequest(userid);
        } catch (Exception e) {
            e.printStackTrace();
        }

        id_text.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Tools.showMessage(getActivity(), "ID是不可以修改的噢！", "info");
            }
        });

        privilege_text.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Tools.showMessage(getActivity(), "如需修改权限，请进入管理模式！", "info");
            }
        });

        save_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String newUsername = username_edit.getText().toString();
                String newEmail = email_edit.getText().toString();
                String newPhone = phone_edit.getText().toString();
                String newPassword = password_edit.getText().toString();
                String newPassword2 = password_confirm_edit.getText().toString();

                if(Tools.isEmpty(userid)) {Tools.showMessage(getActivity(), "系统异常！", "error"); return;}
                if(Tools.isEmpty(newUsername)) {Tools.showMessage(getActivity(), "用户名不能为空！", "error"); return;}
                if(Tools.isEmpty(newEmail)) {Tools.showMessage(getActivity(), "邮箱不能为空！", "error"); return;}
                if(Tools.isEmpty(newPhone)) {Tools.showMessage(getActivity(), "手机号码不能为空！", "error"); return;}
                if(!Tools.isEmail(newEmail)) {Tools.showMessage(getActivity(), "邮箱格式有误！", "error"); return;}
                if(!Tools.isPhone(newPhone)) {Tools.showMessage(getActivity(), "手机号码有误！", "error"); return;}
                if(!Tools.isEmpty(newPassword) && !newPassword.equals(newPassword2))
                    {Tools.showMessage(getActivity(), "两次密码不一致！", "error"); return;}


                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getActivity().getSupportFragmentManager());
                    sendRequest2(userid, newUsername, newPassword, newEmail, newPhone);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                password_edit.setText("");
                password_confirm_edit.setText("");
            }
        });
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    private void sendRequest(final String userid){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "query_profile" + " " + userid;
                    String result = Tools.command(command);
                    if(result.equals("0")){
                        Tools.showMessage(getActivity(), getActivity(), "系统异常！", "error");
                        progressbarFragment.dismiss();
                    }
                    else{
                        username = Tools.getNthSubstring(result, " ", 0);
                        email = Tools.getNthSubstring(result, " ", 1);
                        phone = Tools.getNthSubstring(result, " ", 2);
                        privilege = Tools.getNthSubstring(result, " ", 3);

                        getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                id_text.setText(userid);
                                username_edit.setText(username);
                                email_edit.setText(email);
                                phone_edit.setText(phone);
                                privilege_text.setText(privilege.equals("1") ? "普通用户" : "管理员");
                                progressbarFragment.dismiss();
                            }
                        });
                    }
                } catch (Exception e) {
                    Tools.showMessage(getActivity(), getActivity(), "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private void sendRequest2(final String userid, final String username, final String password, final String email, final String phone){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = !Tools.isEmpty(password)
                        ? "modify_profile" + " " + userid + " " + username + " " + password + " " + email + " " + phone
                        : "modify_profile_without_password" + " " + userid + " " + username + " " + email + " " + phone;
                    String result = Tools.command(command);
                    if(result.equals("0")){
                        Tools.showMessage(getActivity(), getActivity(), "修改失败！", "error");
                        progressbarFragment.dismiss();
                    }
                    else{
                        Tools.showMessage(getActivity(), getActivity(), "修改成功！", "success");
                        progressbarFragment.dismiss();
                    }
                } catch (Exception e) {
                    Tools.showMessage(getActivity(), getActivity(), "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void findAllView(){
        id_text = view.findViewById(R.id.registerID_Text);
        username_edit = view.findViewById(R.id.registerUsername_Edit);
        password_edit = view.findViewById(R.id.registerPassword_Edit);
        password_confirm_edit = view.findViewById(R.id.registerConfirmPassword_Edit);
        email_edit = view.findViewById(R.id.registerEmail_Edit);
        phone_edit = view.findViewById(R.id.registerPhone_Edit);
        privilege_text = view.findViewById(R.id.registerPrivilege_Text);
        save_btn = view.findViewById(R.id.save_Button);
    }

}
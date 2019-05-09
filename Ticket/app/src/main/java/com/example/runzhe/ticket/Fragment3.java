package com.example.runzhe.ticket;

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

public class Fragment3 extends Fragment {

    View view;

    ImageView portrait;
    EditText username;
    EditText email;
    EditText phone;
    EditText new_password;
    TextView priv;
    Button btn_update;

    public int privilege;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_3, container, false);

        portrait = (ImageView)view.findViewById(R.id.s_portrait);
        priv = (TextView) view.findViewById(R.id.s_privilege);
        username = (EditText)view.findViewById(R.id.s_username);
        email = (EditText)view.findViewById(R.id.s_email);
        phone = (EditText)view.findViewById(R.id.s_phone);
        new_password = (EditText) view.findViewById(R.id.s_new_password);
        btn_update = (Button)view.findViewById(R.id.s_update_btn);

        privilege = 2; // TODO : 后端查询该用户权限

        // TODO : 向后端索要各种个人资料并展示，这里胡来一通
        priv.setText(privilege == 1 ? "普通用户" : "管理员");
        username.setText("小明");
        email.setText("xiaoming@sjtu.edu.cn");
        phone.setText("12345678901");
        new_password.setText("");

        portrait.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // TODO : 上传图片
                // TODO : 向后端传图片
                Toast toast = Toast.makeText(getActivity().getApplicationContext(), "头像功能还未开放！", Toast.LENGTH_LONG);
                toast.setGravity(Gravity.BOTTOM, 0, 100);
                toast.show();
            }
        });

        btn_update.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String newUsername = username.getText().toString();
                String newEmail = email.getText().toString();
                String newPhone = phone.getText().toString();
                String newPassword = new_password.getText().toString();

                if(Tools.isEmpty(newUsername)) {Tools.toastMessage(getActivity(), "用户名不能为空！"); return;}
                if(Tools.isEmpty(newEmail)) {Tools.toastMessage(getActivity(), "邮箱不能为空！"); return;}
                if(Tools.isEmpty(newPhone)) {Tools.toastMessage(getActivity(), "手机号码不能为空！"); return;}
                if(!Tools.isEmail(newEmail)) {Tools.toastMessage(getActivity(), "邮箱格式有误！"); return;}
                if(!Tools.isPhone(newPhone)) {Tools.toastMessage(getActivity(), "手机号码有误！"); return;}

                // TODO : 向后端传递修改数据
                boolean success = true;
                if(success) Tools.toastMessage(getActivity(), "修改成功！");
                else Tools.toastMessage(getActivity(), "修改失败！");

            }
        });

        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

    }


}
package com.example.runzhe.ticket;

import android.app.Dialog;
import android.app.TimePickerDialog;
import android.content.Intent;
import android.content.IntentFilter;
import android.provider.ContactsContract;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Html;
import android.text.TextUtils;
import android.util.Pair;
import android.view.Gravity;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import java.sql.Time;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class New2Activity extends AppCompatActivity {

    //extra
    String s_id, s_name, s_catalog;
    boolean tic1, tic2, tic3;

    ListView listView;
    Button newSta, submit;

    ArrayAdapter<String> arrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new2);

        listView = (ListView) findViewById(R.id.n_list);
        newSta = (Button) findViewById(R.id.n_new_sta);
        submit = (Button) findViewById(R.id.n_submit);

        s_id = getIntent().getStringExtra("id");
        s_name = getIntent().getStringExtra("name");
        s_catalog = getIntent().getStringExtra("catalog");
        tic1 = getIntent().getBooleanExtra("tic1", true);
        tic2 = getIntent().getBooleanExtra("tic2", true);
        tic3 = getIntent().getBooleanExtra("tic3", true);

        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, new ArrayList<String>());
        listView.setAdapter(arrayAdapter);

        submit.setOnClickListener(new View.OnClickListener() { // 提交新建车次
            @Override
            public void onClick(View v) { // 提交
                // TODO : 向后端提交数据并返回结果
                Tools.toastMessage(New2Activity.this, "新建成功！");
                finish();
            }
        });

        newSta.setOnClickListener(new View.OnClickListener() { // 新建站点 弹窗
            @Override
            public void onClick(View v) {

                AlertDialog.Builder builder = new AlertDialog.Builder(New2Activity.this);
                View view = View.inflate(New2Activity.this, R.layout.dialog_new, null);

                final AlertDialog alertDialog = builder.create();
                alertDialog.setView(view);
                alertDialog.show();

                final EditText name = (EditText) view.findViewById(R.id.n_sta_name);
                final TextView time_arriv_text = (TextView) view.findViewById(R.id.n_time_arriv);
                final TextView time_start_text = (TextView) view.findViewById(R.id.n_time_start);
                final TextView time_stop_text = (TextView) view.findViewById(R.id.n_time_stop);
                ImageView modify_time_arriv = (ImageView) view.findViewById(R.id.n_modify_time_arriv);
                ImageView modify_time_start = (ImageView) view.findViewById(R.id.n_modify_time_start);
                ImageView modify_time_stop = (ImageView) view.findViewById(R.id.n_modify_time_stop);
                final EditText price1_text = (EditText) view.findViewById(R.id.n_price1);
                final EditText price2_text = (EditText) view.findViewById(R.id.n_price2);
                final EditText price3_text = (EditText) view.findViewById(R.id.n_price3);
                Button submit_new_sta = (Button) view.findViewById(R.id.n_new_sta);

                if(!tic1) {price1_text.setText("N/A"); price1_text.setEnabled(false);}
                if(!tic2) {price2_text.setText("N/A"); price2_text.setEnabled(false);}
                if(!tic3) {price3_text.setText("N/A"); price3_text.setEnabled(false);}

                class ModifyTimeOnClickListener implements View.OnClickListener{ // 仅直接修改对应TextView的字符串
                    @Override
                    public void onClick(final View v) {
                        Calendar calendar = Calendar.getInstance();
                        Dialog dialog = new TimePickerDialog(New2Activity.this,
                                new TimePickerDialog.OnTimeSetListener(){
                                    @Override
                                    public void onTimeSet(TimePicker view, int h, int m) {
                                        switch (v.getId()){
                                            case R.id.n_modify_time_arriv:
                                                time_arriv_text.setText((h >= 10 ? h : "0" + h) + ":" + (m >= 10 ? m : "0" + m));
                                                break;
                                            case R.id.n_modify_time_start:
                                                time_start_text.setText((h >= 10 ? h : "0" + h) + ":" + (m >= 10 ? m : "0" + m));
                                                break;
                                            case R.id.n_modify_time_stop:
                                                time_stop_text.setText((h >= 10 ? h : "0" + h) + ":" + (m >= 10 ? m : "0" + m));
                                                break;
                                        }

                                    }
                                }, 0, 0, true);
                        dialog.show();
                    }
                }
                modify_time_arriv.setOnClickListener(new ModifyTimeOnClickListener());
                modify_time_start.setOnClickListener(new ModifyTimeOnClickListener());
                modify_time_stop.setOnClickListener(new ModifyTimeOnClickListener());

                submit_new_sta.setOnClickListener(new View.OnClickListener() { // 提交新建站点
                    @Override
                    public void onClick(View v) {

                        // 获取控件上的数据
                        String sta_name = name.getText().toString();

                        DateFormat df = new SimpleDateFormat("hh:mm");
                        Date time_arriv, time_start, time_stop;
                        try {
                            time_arriv = df.parse(time_arriv_text.getText().toString());
                            time_start = df.parse(time_start_text.getText().toString());
                            time_stop = df.parse(time_stop_text.getText().toString());
                        } catch (ParseException e){ return; }

                        String price1_s = price1_text.getText().toString();
                        String price2_s = price2_text.getText().toString();
                        String price3_s = price3_text.getText().toString();

                        // 前端检测
                        if(Tools.isEmpty(sta_name)) {Tools.toastMessage(New2Activity.this, "站名不能为空！"); return;}
                        if((tic1 && Tools.isEmpty(price1_s)) || (tic2 && Tools.isEmpty(price2_s)) || (tic3 && Tools.isEmpty(price3_s)))
                           {Tools.toastMessage(New2Activity.this, "票价不能为空！"); return;}
                        if((tic1 && !Tools.isNonNegtiveInteger(price1_s)) || (tic2 && !Tools.isNonNegtiveInteger(price2_s)) || (tic3 && !Tools.isNonNegtiveInteger(price3_s)))
                            {Tools.toastMessage(New2Activity.this, "票价必须是非负整数！"); return;}

                        // 通过检测
                        int price1 = tic1 ? Integer.valueOf(price1_s) : -1;
                        int price2 = tic2 ? Integer.valueOf(price2_s) : -1;
                        int price3 = tic3 ? Integer.valueOf(price3_s) : -1;

                        // TODO : 生成新站点的指令字符串，加载在原字符串后面，提交时传给后端

                        arrayAdapter.add(sta_name);
                        alertDialog.dismiss();

                    }
                });

            }
        });

    }

}
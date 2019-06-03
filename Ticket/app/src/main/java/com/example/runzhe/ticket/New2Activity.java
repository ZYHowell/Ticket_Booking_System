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

import es.dmoral.toasty.Toasty;

public class New2Activity extends AppCompatActivity {

    //extra
    String s_id, s_name, s_catalog;
    boolean[] ticket_type;

    TextView name_in_dialog;
    ListView listView;
    Button newSta, submit;

    ArrayAdapter<String> arrayAdapter;

    String command, stations;
    int price_cnt, station_cnt;

    private ProgressbarFragment progressbarFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new2);

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        progressbarFragment = new ProgressbarFragment();
        command = new String();
        stations = new String();

        findAllView();
        getAllExtra();

        // 列表
        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, new ArrayList<String>());
        listView.setAdapter(arrayAdapter);

        submit.setOnClickListener(new View.OnClickListener() { // 提交新建车次
            @Override
            public void onClick(View v) { // 提交
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getSupportFragmentManager());

                    if(station_cnt < 2){
                        Tools.showMessage(New2Activity.this, New2Activity.this, "请至少添加2个站点！", "error");
                        progressbarFragment.dismiss();
                        return;
                    }

                    command = " " + getIntent().getStringExtra("id")
                             + " " + getIntent().getStringExtra("name")
                             + " " + getIntent().getStringExtra("catalog")
                             + " " + station_cnt
                             + " " + price_cnt;
                    for(int i = 0; i < 11; i++)
                        if(getIntent().getBooleanExtra("ticket_type_"+i, false))
                            command += " " + i;
                    command += "\n" + stations;

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                if(Tools.command("query_train " + getIntent().getStringExtra("id")).equals("0"))
                                    command = "add_train" + command;
                                else
                                    command = "modify_train" + command;
                                String result = Tools.command(command);
                                if(result.equals("1")){
                                    Tools.showMessage(New2Activity.this, New2Activity.this, "新建/修改成功！", "success");
                                    progressbarFragment.dismiss();
                                    finish();
                                }
                                else{
                                    Tools.showMessage(New2Activity.this, New2Activity.this, "新建/修改失败！", "error");
                                    progressbarFragment.dismiss();
                                }
                            } catch (Exception e) {
                                Tools.showMessage(New2Activity.this, New2Activity.this, "请检查网络连接！", "warning");
                                progressbarFragment.dismiss();
                                e.printStackTrace();
                            }
                        }
                    }).start();
                } catch (Exception e) {
                    e.printStackTrace();
                }
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

                // find all view
                final TextView name = (TextView) view.findViewById(R.id.n_sta_name);
                name_in_dialog = name;
                final TextView time_arriv_text = (TextView) view.findViewById(R.id.n_time_arriv);
                final TextView time_start_text = (TextView) view.findViewById(R.id.n_time_start);
                final TextView time_stop_text = (TextView) view.findViewById(R.id.n_time_stop);
                ImageView modify_time_arriv = (ImageView) view.findViewById(R.id.n_modify_time_arriv);
                ImageView modify_time_start = (ImageView) view.findViewById(R.id.n_modify_time_start);
                ImageView modify_time_stop = (ImageView) view.findViewById(R.id.n_modify_time_stop);
                Button submit_new_sta = (Button) view.findViewById(R.id.n_new_sta);
                final EditText[] ticket_type_edit = new EditText[11];
                ticket_type_edit[0] = view.findViewById(R.id.price_0);                ticket_type_edit[1] = view.findViewById(R.id.price_1);
                ticket_type_edit[2] = view.findViewById(R.id.price_2);                ticket_type_edit[3] = view.findViewById(R.id.price_3);
                ticket_type_edit[4] = view.findViewById(R.id.price_4);                ticket_type_edit[5] = view.findViewById(R.id.price_5);
                ticket_type_edit[6] = view.findViewById(R.id.price_6);                ticket_type_edit[7] = view.findViewById(R.id.price_7);
                ticket_type_edit[8] = view.findViewById(R.id.price_8);                ticket_type_edit[9] = view.findViewById(R.id.price_9);
                ticket_type_edit[10] = view.findViewById(R.id.price_10);
                final TextView[] ticket_type_text = new TextView[11];
                ticket_type_text[0] = view.findViewById(R.id.textview_0);                ticket_type_text[1] = view.findViewById(R.id.textview_1);
                ticket_type_text[2] = view.findViewById(R.id.textview_2);                ticket_type_text[3] = view.findViewById(R.id.textview_3);
                ticket_type_text[4] = view.findViewById(R.id.textview_4);                ticket_type_text[5] = view.findViewById(R.id.textview_5);
                ticket_type_text[6] = view.findViewById(R.id.textview_6);                ticket_type_text[7] = view.findViewById(R.id.textview_7);
                ticket_type_text[8] = view.findViewById(R.id.textview_8);                ticket_type_text[9] = view.findViewById(R.id.textview_9);
                ticket_type_text[10] = view.findViewById(R.id.textview_10);

                // init ticket_type_edit
                price_cnt = 0;
                for(int i = 0; i < 11; i++) {
                    ticket_type_text[i].setText(Tools.getSeatType(i));
                    boolean enable = getIntent().getBooleanExtra("ticket_type_"+i, false);
                    ticket_type_edit[i].setEnabled(enable);
                    if(!enable) ticket_type_edit[i].setText("N/A（未启用）");
                    else price_cnt++;
                }

                name.setOnClickListener(new changeStationClickListener());
                class ModifyTimeOnClickListener implements View.OnClickListener{ // 仅直接修改对应TextVie                      w的字符串
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
                    public void onClick(View v) { // 新建一个车站
                        // 获取控件上的数据
                        String sta_name = name.getText().toString();
                        DateFormat df = new SimpleDateFormat("hh:mm");
                        Date time_arriv, time_start, time_stop;
                        try {
                            time_arriv = df.parse(time_arriv_text.getText().toString());
                            time_start = df.parse(time_start_text.getText().toString());
                            time_stop = df.parse(time_stop_text.getText().toString());
                        } catch (ParseException e){ return; }

                        String[] price_s = new String[11];
                        for(int i = 0; i < 11; i++) price_s[i] = ticket_type_edit[i].getText().toString();

                        // 前端检测
                        if(sta_name.equals("请选择")) {
                            Tools.showMessage(New2Activity.this, "站名不能为空！", "error"); return;}
                        for(int i = 0; i < 11; i++){
                            if(getIntent().getBooleanExtra("ticket_type_"+i, false)
                                    && (Tools.isEmpty(price_s[i]) || !Tools.isNonNegtiveInteger(price_s[i]))){
                                Tools.showMessage(New2Activity.this, Tools.getSeatType(i) + "价格不合法！", "error");
                                return;
                            }
                        }

                        int[] price = new int[11];
                        for(int i = 0; i < 11; i++) price[i] = getIntent().getBooleanExtra("ticket_type_"+i, false)
                                                                ? Integer.valueOf(price_s[i]) : -1;
                        /* 生成指令*/
                        station_cnt++;
                        stations += sta_name
                                + " " + time_arriv_text.getText().toString()
                                + " " + time_start_text.getText().toString()
                                + " " + time_stop_text.getText().toString();
                        for(int i = 0; i < 11; i++) if(price[i] != -1)
                            stations += " ￥" + price[i];
                        stations += "\n";

                        arrayAdapter.add(sta_name);
                        alertDialog.dismiss();
                    }
                });

            }
        });

    }


    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (resultCode){
            case 1:
                name_in_dialog.setText(data.getStringExtra("station"));
                break;
        }
    }

    class changeStationClickListener implements View.OnClickListener{
        public void onClick(View v) {
            Intent intent = new Intent(New2Activity.this, SelectStation.class);
            startActivityForResult(intent, 2333);
        }
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

    void getAllExtra(){
        s_id = getIntent().getStringExtra("id");
        s_name = getIntent().getStringExtra("name");
        s_catalog = getIntent().getStringExtra("catalog");
        ticket_type = new boolean[11];
        for(int i = 0; i < 11; i++) ticket_type[i] = getIntent().getBooleanExtra("ticket_type_" + i, false);
    }

    void findAllView(){
        listView = (ListView) findViewById(R.id.n_list);
        newSta = (Button) findViewById(R.id.n_new_sta);
        submit = (Button) findViewById(R.id.n_submit);
    }
}
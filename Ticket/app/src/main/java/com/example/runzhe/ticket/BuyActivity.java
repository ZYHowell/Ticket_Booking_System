package com.example.runzhe.ticket;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import es.dmoral.toasty.Toasty;

import static android.widget.Toast.*;

public class BuyActivity extends AppCompatActivity {

    TextView train_id_text;
    TextView date_text;
    TextView departure_text;
    TextView destination_text;
    TextView depart_time_text;
    TextView destination_time_text;
    ListView ticket_list;

    private ProgressbarFragment progressbarFragment;

    private List<String> list;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_buy);

        findAllView();
        setAllInfo();

        progressbarFragment = new ProgressbarFragment();

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        refresh();

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View vi, final int position, long id) { // 购票
                final android.support.v7.app.AlertDialog.Builder builder = new android.support.v7.app.AlertDialog.Builder(BuyActivity.this);
                final View view = View.inflate(BuyActivity.this, R.layout.dialog_buy_ticket, null);
                builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
//                        dialog.dismiss();
                    }
                });
                builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        EditText num_edit = view.findViewById(R.id.num_edit);
                        String num = num_edit.getText().toString();
                        if(Tools.isEmpty(num) || !Tools.isNonNegtiveInteger(num)){
                            Tools.showMessage(BuyActivity.this, "数量有误！", "error");
                            return;
                        }
                        try {
                            progressbarFragment.setCancelable(false);
                            progressbarFragment.show(getSupportFragmentManager());
                            sendRequest(getIntent().getStringExtra("userid"), num,
                                    train_id_text.getText().toString(),
                                    departure_text.getText().toString(),
                                    destination_text.getText().toString(),
                                    date_text.getText().toString(),
                                    position);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                });
                final android.support.v7.app.AlertDialog alertDialog = builder.create();
                alertDialog.setView(view);
                alertDialog.show();
            }
        });

    }

    void sendRequest(final String userid, final String num, final String train_id, final String departure, final String destination,
                     final String date, final int ticket_type){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "buy_ticket" + " " + userid + " " + num + " " + train_id + " "
                            + departure + " " + destination + " " + date + " " + ticket_type;
                    String result = Tools.command(command);
                    if(result.equals("1")){
                        Tools.showMessage(BuyActivity.this, BuyActivity.this, "购票成功！\n（"
                                + Integer.valueOf(num) + "×" + Tools.getSeatType(ticket_type) + "）", "success");
                        progressbarFragment.dismiss();
                        refresh();
                    }
                    else{
                        Tools.showMessage(BuyActivity.this, BuyActivity.this, "购票失败！", "error");
                        progressbarFragment.dismiss();
                    }
                } catch (Exception e) {
                    Tools.showMessage(BuyActivity.this, BuyActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    void findAllView(){
        depart_time_text = findViewById(R.id.depart_time);
        destination_time_text = findViewById(R.id.destination_time);
        train_id_text = findViewById(R.id.train_id);
        date_text = findViewById(R.id.textView_date);
        departure_text = findViewById(R.id.departure);
        destination_text = findViewById(R.id.destination);
        ticket_list = findViewById(R.id.ticket_list);
    }
    void setAllInfo(){
        Intent intent = getIntent();
        train_id_text.setText(intent.getStringExtra("train_id"));
        date_text.setText(intent.getStringExtra("date"));
        departure_text.setText(intent.getStringExtra("departure"));
        destination_text.setText(intent.getStringExtra("destination"));
        depart_time_text.setText(intent.getStringExtra("depart_time"));
        destination_time_text.setText(intent.getStringExtra("destination_time"));
    }

    void refresh(){
        // 从后端获得座数量与价格
        progressbarFragment = new ProgressbarFragment();
        progressbarFragment.setCancelable(false);
        progressbarFragment.show(getSupportFragmentManager());
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "query_available_ticket"
                            + " " + getIntent().getStringExtra("train_id")
                            + " " + getIntent().getStringExtra("date")
                            + " " + getIntent().getStringExtra("departure")
                            + " " + getIntent().getStringExtra("destination");
                    // 返回2*11个数据 每一个二元组是 价格+剩余数量     查不到返回0
                    String result = Tools.command(command);

                    if(!result.equals("0")){
                        final String[] left = new String[11];
                        final String[] price = new String[11];
                        for (int i = 0; i < 11; i++) {
                            price[i] = Tools.getNthSubstring(result, " ", i * 2 );
                            left[i] = Tools.getNthSubstring(result, " ", i * 2 + 1);
                        }
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                list = new ArrayList<>();
                                for (int i = 0; i < 11; i++) {
                                    if (left[i].equals("-1"))
                                        list.add(Tools.getSeatType(i) + " ： 无");
                                    else
                                        list.add(Tools.getSeatType(i) + " ： ￥ " + price[i] + "     " + "剩余 " + left[i] + " 张");
                                }
                                ArrayAdapter<String> adapter = new ArrayAdapter<String>(BuyActivity.this, android.R.layout.simple_list_item_1, list);
                                ticket_list.setAdapter(adapter);
                            }
                        });
                    }
                    else {
                        Tools.showMessage(BuyActivity.this, BuyActivity.this, "查询失败！", "error");
                        finish();
                    }
                    progressbarFragment.dismiss();
                } catch (Exception e) {
                    Tools.showMessage(BuyActivity.this, BuyActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

}

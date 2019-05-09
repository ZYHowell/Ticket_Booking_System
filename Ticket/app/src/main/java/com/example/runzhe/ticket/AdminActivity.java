package com.example.runzhe.ticket;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ListView;

public class AdminActivity extends AppCompatActivity {

    ListView listView;
    EditText id;
    Button search;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_admin);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // 开始新建车次
                Intent intent = new Intent(AdminActivity.this, NewActivity.class);
                startActivity(intent);
            }
        });

        listView = (ListView)findViewById(R.id.a_ticket_list);
        id = (EditText) findViewById(R.id.a_id);
        search = (Button) findViewById(R.id.a_search);

        search.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // TODO : 以id向后端查询车次信息

                boolean found = false; // 是否找到

                if(found){
                    // 显示车次信息
                }
                else{
                    Tools.toastMessage(AdminActivity.this, "未找到对应的车次！");
                }

            }
        });

        // TODO : 从后端获得车次信息并放进listView

    }

    @Override
    protected void onResume() {
        super.onResume();
        // TODO : 因为可能已经修改，所以重新从后端获得车次信息并放进listView
    }
}

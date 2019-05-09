package com.example.runzhe.ticket;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.List;

public class InformationActivity extends AppCompatActivity {

    ListView ticket_list;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_information);

        ticket_list = (ListView) findViewById(R.id.ticket_list);

        String[] tickets = getIntent().getStringArrayExtra("tickets");
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, tickets);
        ticket_list.setAdapter(adapter);

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                String id_s = Tools.getNthSubstring((String)parent.getAdapter().getItem(position), " ", 0);

                // TODO : 从后端获得id_s对应车票的信息并putExtra方式传给BuyActivity

                Intent intent = new Intent(InformationActivity.this, BuyActivity.class);
                intent.putExtra("station_1", "北京");
                intent.putExtra("station_2", "上海");
                intent.putExtra("time_1", "13:00");
                intent.putExtra("time_2", "14:00");
                intent.putExtra("price_1", 100);
                intent.putExtra("price_2", 200);
                intent.putExtra("price_3", 300);
                intent.putExtra("left_1", 1000);
                intent.putExtra("left_2", 2000);
                intent.putExtra("left_3", 3000);
                startActivity(intent);
            }
        });

    }
}

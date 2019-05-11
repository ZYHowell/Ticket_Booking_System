package com.example.runzhe.ticket;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;

import java.util.List;

public class InformationActivity extends AppCompatActivity {

    ListView ticket_list;
    Button sort_by_consume, sort_by_start;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_information);

        ticket_list = (ListView) findViewById(R.id.i_ticket_list);
        sort_by_consume = (Button) findViewById(R.id.i_sort_by_time_consume);
        sort_by_start = (Button) findViewById(R.id.i_sort_by_time_start);

        // TODO : 分别获得按两种方式排序的车次信息
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

        sort_by_consume.setOnClickListener(new OnClickListener());
        sort_by_start.setOnClickListener(new OnClickListener());

    }

    void refresh(){
        // TODO : 更改排序次序后重新显示车票
    }

    class OnClickListener implements View.OnClickListener{
        @Override
        public void onClick(View v) {
            switch (v.getId()){
                case R.id.i_sort_by_time_consume:
                    // TODO : 按耗时排序
                    break;
                case R.id.i_sort_by_time_start:
                    // TODO : 按发时排序
                    break;
            }
            refresh();
        }
    }
}

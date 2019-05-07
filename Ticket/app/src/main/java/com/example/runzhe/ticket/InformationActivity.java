package com.example.runzhe.ticket;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
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

        String[] test = {"D233    北京 → 上海    07:34 → 13:54", "D234    北京 → 上海    08:34 → 14:54"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, test);
        ticket_list.setAdapter(adapter);

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent(InformationActivity.this, BuyActivity.class);
                intent.putExtra("station_1", "北京");
                intent.putExtra("station_2", "上海");
                intent.putExtra("time_1", "13:00");
                intent.putExtra("time_2", "14:00");
                startActivity(intent);
            }
        });

    }
}

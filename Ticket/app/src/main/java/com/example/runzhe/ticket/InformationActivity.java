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

    String[] tickets;
    String[] tickets_tmp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_information);

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        ticket_list = (ListView) findViewById(R.id.i_ticket_list);
        sort_by_consume = (Button) findViewById(R.id.i_sort_by_time_consume);
        sort_by_start = (Button) findViewById(R.id.i_sort_by_time_start);

        // TODO : 分别获得按两种方式排序的车次信息
        /*******************/
        tickets = getIntent().getStringArrayExtra("tickets");
        tickets_tmp = new String[tickets.length];
        for(int i = 0; i < tickets.length; i++){ // 整理以便展示 注意这里是0
            String[] tmp = tickets[i].split(" ");
            tickets_tmp[i] = tmp[0] + " " + tmp[1] + " → " + tmp[4] + " " + tmp[3] + " → " + tmp[6];
        }
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, tickets_tmp);
        ticket_list.setAdapter(adapter);
        /*******************/

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String[] tmp = tickets[position].split(" "); // 此车票标准信息的分隔
                Intent intent = new Intent(InformationActivity.this, BuyActivity.class);
                intent.putExtra("userid", getIntent().getStringExtra("userid"));
                intent.putExtra("train_id", tmp[0]);
                intent.putExtra("departure", tmp[1]);
                intent.putExtra("date", tmp[2]);
                intent.putExtra("depart_time", tmp[3]);
                intent.putExtra("destination", tmp[4]);
                intent.putExtra("destination_time", tmp[6]);
                for(int i = 0; i < 11; i++){
                    intent.putExtra("left_" + i, tmp[7 + i * 2]); // -1 if no available
                    intent.putExtra("price_" + i, tmp[7 + i * 2 + 1]);
                }
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

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }
}

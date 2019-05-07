package com.example.runzhe.ticket;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class BuyActivity extends AppCompatActivity {

    String sta_1, sta_2, time_1, time_2;
    TextView station_1, station_2;
    Button btn_tic_1,btn_tic_2,btn_tic_3;
    Button btn_submit;

    int curType;
    int buttonColor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_buy);

        sta_1 = getIntent().getStringExtra("station_1");
        sta_2 = getIntent().getStringExtra("station_2");
        time_1 = getIntent().getStringExtra("time_1");
        time_2 = getIntent().getStringExtra("time_2");

        station_1 = (TextView) findViewById(R.id.y_station_1);
        station_2 = (TextView) findViewById(R.id.y_station_2);
        btn_tic_1 = (Button) findViewById(R.id.y_btn_tic_1);
        btn_tic_2 = (Button) findViewById(R.id.y_btn_tic_2);
        btn_tic_3 = (Button) findViewById(R.id.y_btn_tic_3);
        btn_submit = (Button) findViewById(R.id.y_submit);

        station_1.setText(sta_1 + "\n" + time_1);
        station_2.setText(sta_2 + "\n" + time_2);

        buttonColor = 0xFFFFFF;
        curType = 1;
        btn_tic_1.setBackgroundResource(R.color.colorPrimary);
        btn_tic_2.setBackgroundColor(buttonColor);
        btn_tic_3.setBackgroundColor(buttonColor);

        btn_submit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // TODO
            }
        });

        btn_tic_1.setOnClickListener(new ChooseTicketType());
        btn_tic_2.setOnClickListener(new ChooseTicketType());
        btn_tic_3.setOnClickListener(new ChooseTicketType());

    }

    class ChooseTicketType implements View.OnClickListener{
        @Override
        public void onClick(View v) {

            btn_tic_1.setBackgroundColor(buttonColor);
            btn_tic_2.setBackgroundColor(buttonColor);
            btn_tic_3.setBackgroundColor(buttonColor);

            switch (v.getId()){
                case R.id.y_btn_tic_1:
                    curType = 1;
                    btn_tic_1.setBackgroundResource(R.color.colorPrimary);
                    break;
                case R.id.y_btn_tic_2:
                    curType = 2;
                    btn_tic_2.setBackgroundResource(R.color.colorPrimary);
                    break;
                case R.id.y_btn_tic_3:
                    curType = 3;
                    btn_tic_3.setBackgroundResource(R.color.colorPrimary);
                    break;
            }
        }
    }

}

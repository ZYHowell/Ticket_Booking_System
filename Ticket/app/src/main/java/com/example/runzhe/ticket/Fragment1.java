package com.example.runzhe.ticket;

import android.app.DatePickerDialog;
import android.content.Intent;
import android.media.Image;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.zaaach.citypicker.CityPicker;

import org.w3c.dom.Text;

import java.util.Calendar;

import es.dmoral.toasty.Toasty;

import static java.util.Collections.swap;

public class Fragment1 extends Fragment {

    View view;

    TextView station_1;
    TextView station_2;
    TextView date;
    ImageView double_arrow;
    ImageView modify_date;

    Calendar calendar;
    DatePickerDialog dialog;

    Button btn_query;

    int cur_year, cur_month, cur_day; // 车次日期
    int whichStationToChange;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_1, container, false);

        station_1 = (TextView) view.findViewById(R.id.b_station_1);
        station_2 = (TextView) view.findViewById(R.id.b_station_2);
        date = (TextView) view.findViewById(R.id.b_date);
        double_arrow = (ImageView) view.findViewById(R.id.b_double_arrow);
        modify_date = (ImageView) view.findViewById(R.id.b_modify_date);
        btn_query = (Button) view.findViewById(R.id.b_query);

        class changeStationClickListener implements View.OnClickListener{
            public void onClick(View v) {
                Intent intent = new Intent(getActivity(), CityPickerActivity.class);
                startActivityForResult(intent, 1);
                whichStationToChange = v.getId() == R.id.b_station_1 ? 1: 2; // 判断是哪一个框
            }
        }
        station_1.setOnClickListener(new changeStationClickListener());
        station_2.setOnClickListener(new changeStationClickListener());

        double_arrow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String s1 = station_1.getText().toString();
                String s2 = station_2.getText().toString();
                station_1.setText(s2);
                station_2.setText(s1);
            }
        });

        btn_query.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Tools.isEmpty(station_1.getText().toString()) || Tools.isEmpty(station_2.getText().toString())){
                    Toasty.error(getActivity(), "站点不能为空！", Toast.LENGTH_LONG, true). show();
                    return;
                }

                // TODO : 向后端发送信息，接收到的信息使用putExtra方式以string[]参数传给InformationActivity
                String[] tickets = new String[]{"D233 北京 → 上海 07:34 → 13:54", "D234 北京 → 上海 08:34 → 14:54"};

                Intent intent = new Intent(getActivity(), InformationActivity.class);
                intent.putExtra("tickets", tickets);
                startActivity(intent);
            }
        });

        setDate();

        return view;
    }

    public void setDate(){
        calendar = Calendar.getInstance();
        cur_year = calendar.get(Calendar.YEAR);
        cur_month = calendar.get(Calendar.MONTH) + 1;
        cur_day = calendar.get(Calendar.DAY_OF_MONTH);
        date.setText(cur_year + "/" + cur_month + "/" + cur_day);
        modify_date.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                calendar = Calendar.getInstance();
                dialog = new DatePickerDialog(getActivity(),
                        new DatePickerDialog.OnDateSetListener() {

                            @Override
                            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
                                cur_year = year; cur_month = monthOfYear + 1; cur_day = dayOfMonth;
                                date.setText(cur_year + "/" + cur_month + "/" + cur_day);
                            }
                        }, calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH));
                dialog.show();
            }
        });
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (resultCode){
            case 1:
                (whichStationToChange == 1 ? station_1 : station_2).setText(data.getStringExtra("station"));
                break;
        }
    }
}
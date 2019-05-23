package com.example.runzhe.ticket;

import android.app.AlertDialog;
import android.app.DatePickerDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import es.dmoral.toasty.Toasty;

import static android.widget.Toast.LENGTH_SHORT;

public class Fragment2 extends Fragment {

    View view;

    TextView date;
    Spinner train_catalog_spinner;
    Button query_btn;
    ImageView modify_date;
    ListView ticket_list;

    ArrayAdapter<String> arrayAdapter;
    List<String> train_catalog_list;

    Calendar calendar;
    DatePickerDialog dialog;
    int cur_year, cur_month, cur_day; // 车次日期

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_2, container, false);

        findAllView();
        setDate();

        train_catalog_list = new ArrayList<String>();
        addAllCatalog(train_catalog_list);
        arrayAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, train_catalog_list);
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        train_catalog_spinner.setAdapter(arrayAdapter);

        query_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                refresh();
            }
        });

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String id_s = Tools.getNthSubstring((String)parent.getAdapter().getItem(position), " ", 0);
                Intent intent = new Intent(getActivity(), ReturnTicketActivity.class);
                // TODO : 后端要数据
                intent.putExtra("id", id_s);
                intent.putExtra("data", "2019/06/06");
                intent.putExtra("departure", "北京");
                intent.putExtra("destination", "南京");
                intent.putExtra("departure_time", "12:34");
                intent.putExtra("destination_time", "56:12");
                startActivity(intent);
                refresh(); // ONRESUME ?
            }
        });

        return view;
    }

    void refresh(){
        // TODO : 向后端要所有车票
        String[] tickets = {"D233    北京 → 上海    07:34 → 13:54    一等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D234    北京 → 上海    08:34 → 14:54    二等", "D236    北京 → 上海    08:34 → 14:54    二等"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_list_item_1, tickets);
        ticket_list.setAdapter(adapter);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
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

    void findAllView(){
        date = view.findViewById(R.id.b_date);
        train_catalog_spinner = view.findViewById(R.id.trainCatalog_Spinner);
        query_btn = view.findViewById(R.id.query_Button);
        ticket_list = view.findViewById(R.id.ticket_list);
        modify_date = view.findViewById(R.id.b_modify_date);
    }

    void addAllCatalog(List<String> list){
        list.add("T(特快)");
        list.add("Z(直达)");
        list.add("O(普通)");
        list.add("G(高铁)");
        list.add("D(动车)");
        list.add("K(快车)");
        list.add("C(城际)");
    }

}
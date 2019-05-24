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
import android.widget.CheckBox;
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
    Button btn_query;

    CheckBox[] cb;
    CheckBox cb_All, cb_transfer;

    Calendar calendar;
    DatePickerDialog dialog;
    int cur_year, cur_month, cur_day; // 车次日期

    private ProgressbarFragment progressbarFragment;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_1, container, false);

        findAllView(view);
        initCheckBox();
        setDate();

        progressbarFragment = new ProgressbarFragment();

        class changeStationClickListener implements View.OnClickListener{
            public void onClick(View v) {
                Intent intent = new Intent(getActivity(), SelectStation.class);
                intent.putExtra("type", v.getId() == R.id.b_station_1 ? "1" : "2");
                startActivityForResult(intent, 2333);
            }
        }
        station_1.setOnClickListener(new changeStationClickListener());
        station_2.setOnClickListener(new changeStationClickListener());

        double_arrow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { // 交换站点
                String s1 = station_1.getText().toString();
                String s2 = station_2.getText().toString();
                station_1.setText(s2);
                station_2.setText(s1);
            }
        });

        btn_query.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { // 查询车票
                if(Tools.isEmpty(station_1.getText().toString()) || Tools.isEmpty(station_2.getText().toString())){
                    Tools.showMessage(getActivity(), "站点不能为空！", "error");
                    return;
                }
                String catalogs = getAllCatalogs();
                if(Tools.isEmpty(catalogs)){
                    Tools.showMessage(getActivity(), "车类未选择！", "error");
                    return;
                }
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getActivity().getSupportFragmentManager());
                    sendRequest(station_1.getText().toString(), station_2.getText().toString(), date.getText().toString(), catalogs, cb_transfer.isChecked());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
        return view;
    }

    private void sendRequest(final String sta1, final String sta2, final String date, final String catalogs, final boolean transfer){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = (transfer ? "query_transfer" : "query_ticket") + " " + sta1 + " " + sta2 + " " + date + " " + catalogs;
                    String result = Tools.command(command);
                    if(result.equals("-1") || result.equals("0")){
                        Tools.showMessage(getActivity(), getActivity(), "无查询结果！", "error");
                        progressbarFragment.dismiss();
                    }
                    else{
                        // 向后端发送信息，接收到的信息使用putExtra方式以string[]参数传给InformationActivity
                        String[] ticket_arr = result.split("\n");
                        Intent intent = new Intent(getActivity(), InformationActivity.class);
                        if(transfer)
                            intent.putExtra("tickets", ticket_arr);
                        else{
                            String[] tickets = new String[ticket_arr.length - 1];
                            for(int i = 1; i < ticket_arr.length; i++){
                                tickets[i-1] = ticket_arr[i];
                            }
                            intent.putExtra("userid", getActivity().getIntent().getStringExtra("userid"));
                            intent.putExtra("tickets", tickets);
                        }
                        progressbarFragment.dismiss();
                        startActivity(intent);
                    }
                } catch (Exception e) {
                    Tools.showMessage(getActivity(), getActivity(), "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public void setDate(){
        calendar = Calendar.getInstance();
        cur_year = calendar.get(Calendar.YEAR);
        cur_month = calendar.get(Calendar.MONTH) + 1;
        cur_day = calendar.get(Calendar.DAY_OF_MONTH);
        if(cur_month == 5) {cur_month = 6; cur_day = 1;}
        date.setText(cur_year + "-" + Tools.getStandardDate("" + cur_month) + "-" + Tools.getStandardDate("" + cur_day));
        modify_date.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                calendar = Calendar.getInstance();
                dialog = new DatePickerDialog(getActivity(),
                        new DatePickerDialog.OnDateSetListener() {

                            @Override
                            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
                                cur_year = year; cur_month = monthOfYear + 1; cur_day = dayOfMonth;
                                date.setText(cur_year + "-" + Tools.getStandardDate("" + cur_month) + "-" + Tools.getStandardDate("" + cur_day));
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
                (data.getStringExtra("type").equals("1") ? station_1 : station_2).setText(data.getStringExtra("station"));
                break;
        }
    }

    void findAllView(View view){
        station_1 = (TextView) view.findViewById(R.id.b_station_1);
        station_2 = (TextView) view.findViewById(R.id.b_station_2);
        date = (TextView) view.findViewById(R.id.b_date);
        double_arrow = (ImageView) view.findViewById(R.id.b_double_arrow);
        modify_date = (ImageView) view.findViewById(R.id.b_modify_date);
        btn_query = (Button) view.findViewById(R.id.b_query);

        cb = new CheckBox[7];
        cb[0] = view.findViewById(R.id.train_checkbox_T);
        cb[1] = view.findViewById(R.id.train_checkbox_Z);
        cb[2] = view.findViewById(R.id.train_checkbox_O);
        cb[3] = view.findViewById(R.id.train_checkbox_G);
        cb[4] = view.findViewById(R.id.train_checkbox_D);
        cb[5] = view.findViewById(R.id.train_checkbox_K);
        cb[6] = view.findViewById(R.id.train_checkbox_C);
        cb_All = view.findViewById(R.id.train_checkbox_All);
        cb_transfer = view.findViewById(R.id.train_checkbox_transfer);
    }
    String getAllCatalogs(){
        String catalog = "";
        for(int i = 0; i < 7; i++) if(cb[i].isChecked())
            catalog += Tools.getTrainCatalogs(i);
        return catalog;
    }
    void initCheckBox(){
        cb_All.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean state = cb_All.isChecked();
                setAllCheckBoxState(state);
            }
        });
    }
    void setAllCheckBoxState(boolean state){
        for(int i = 0; i <= 6; i++) cb[i].setChecked(state);
    }
}
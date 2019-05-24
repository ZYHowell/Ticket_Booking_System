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

    String[] tickets;

    private ProgressbarFragment progressbarFragment;

    Calendar calendar;
    DatePickerDialog dialog;
    int cur_year, cur_month, cur_day; // 车次日期

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_2, container, false);

        findAllView();
        setDate();

        progressbarFragment = new ProgressbarFragment();

        train_catalog_list = new ArrayList<String>();
        addAllCatalog(train_catalog_list);
        arrayAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, train_catalog_list);
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        train_catalog_spinner.setAdapter(arrayAdapter);

        query_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getActivity().getSupportFragmentManager());
                    sendRequest(getActivity().getIntent().getStringExtra("userid"), date.getText().toString(),
                            train_catalog_spinner.getSelectedItemPosition());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String[] tmp = tickets[position].split(" "); // 此车票标准信息的分隔
                Intent intent = new Intent(getActivity(), ReturnTicketActivity.class);
                intent.putExtra("userid", getActivity().getIntent().getStringExtra("userid"));
                intent.putExtra("train_id", tmp[0]);
                intent.putExtra("departure", tmp[1]);
                intent.putExtra("date", tmp[2]);
                intent.putExtra("depart_time", tmp[3]);
                intent.putExtra("destination", tmp[4]);
                intent.putExtra("destination_time", tmp[6]);
                for(int i = 0; i < 11; i++){
                    intent.putExtra("left_" + i, tmp[7 + i * 2]); // -1 if no available
                }
                startActivity(intent);
            }
        });

        return view;
    }

    void sendRequest(final String userid, final String date, final int catalog) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "query_order" + " " + userid + " " + date + " " + Tools.getTrainCatalogs(catalog);
                    String result = Tools.command(command);
                    if(result.equals("-1") || result.equals("0")){
System.out.println("fuckyou2");
                        Tools.showMessage(getActivity(), getActivity(), "无查询结果！", "error");
                        progressbarFragment.dismiss();
                    }
                    else{
                        /*******************/
System.out.println("fuckyou");
                        tickets = result.split("\n");
                        String[] tickets_tmp = new String[tickets.length];
                        for(int i = 1; i < tickets.length; i++){ // 整理以便展示 注意这里是1
                            String[] tmp = tickets[i].split(" ");
                            tickets_tmp[i] = tmp[0] + " " + tmp[1] + " → " + tmp[4] + " " + tmp[3] + " → " + tmp[6];
                        }
                        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_list_item_1, tickets_tmp);
                        ticket_list.setAdapter(adapter);
                        /*******************/
                        progressbarFragment.dismiss();
                    }
                } catch (Exception e) {
                    Tools.showMessage(getActivity(), getActivity(), "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
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
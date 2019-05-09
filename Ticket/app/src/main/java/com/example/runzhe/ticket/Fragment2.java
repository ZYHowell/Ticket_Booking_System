package com.example.runzhe.ticket;

import android.app.AlertDialog;
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
import android.widget.ListView;

public class Fragment2 extends Fragment {

    View view;
    ListView ticket_list;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_2, container, false);

        ticket_list = (ListView) view.findViewById(R.id.ticket_list);

        refresh();

        ticket_list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                String id_s = Tools.getNthSubstring((String)parent.getAdapter().getItem(position), " ", 0);

                final AlertDialog.Builder mBuilder = new AlertDialog.Builder(getActivity());
                mBuilder.setTitle("退票");
                mBuilder.setMessage("确认退票 " + id_s + " ？");
                mBuilder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // TODO : 后端发送退票请求，id为id_s
                        boolean success = true;
                        if(success) Tools.toastMessage(getActivity(), "退票成功！");
                        else Tools.toastMessage(getActivity(), "退票失败！");
                        refresh();
                        dialog.dismiss();
                    }
                });
                mBuilder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
                AlertDialog mDialog = mBuilder.create();
                mDialog.show();
            }
        });

        return view;
    }

    void refresh(){
        // TODO : 向后端要数据以刷新
        String[] tickets = {"D233    北京 → 上海    07:34 → 13:54    一等", "D234    北京 → 上海    08:34 → 14:54    二等"};
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_list_item_1, tickets);
        ticket_list.setAdapter(adapter);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

    }


}
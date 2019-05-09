package com.example.runzhe.ticket;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.util.ArrayList;

public class AdminActivity extends AppCompatActivity {

    ListView listView;
    EditText idText;
    Button search;

    ArrayAdapter<String> arrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_admin);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // 开始新建车次
                Intent intent = new Intent(AdminActivity.this, NewActivity.class);
                startActivity(intent);
            }
        });

        listView = (ListView)findViewById(R.id.a_ticket_list);
        idText = (EditText) findViewById(R.id.a_id);
        search = (Button) findViewById(R.id.a_search);

        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, new ArrayList<String>());
        listView.setAdapter(arrayAdapter);

        // TODO : 从后端获得所有车次信息，按照一定规则做成字符串，并放进listView


        refresh(); // 刷新列表

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Adapter adapter = parent.getAdapter();
                String str = (String)adapter.getItem(position);
                String[] strArray = str.split(" ");
                idText.setText(strArray[0]);
            }
        });

        search.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) { // 查询

                String id = idText.getText().toString();
                if(Tools.isEmpty(id)) {Tools.toastMessage(AdminActivity.this, "ID不能为空！"); return;}

                // TODO : 以id向后端查询车次信息

                boolean found = true; // 是否找到

                if(found){ // 弹窗显示车次信息以便操作
                    AlertDialog.Builder builder = new AlertDialog.Builder(AdminActivity.this);
                    View view = View.inflate(AdminActivity.this, R.layout.activity_modify_ticket, null);
                    final AlertDialog alertDialog = builder.create();
                    alertDialog.setView(view);
                    alertDialog.show();

                    // 获取控件
                    TextView idText = (TextView) view.findViewById(R.id.m_id);
                    TextView nameText = (TextView) view.findViewById(R.id.m_name);
                    TextView catalogText = (TextView) view.findViewById(R.id.m_catalog);
                    TextView statusText = (TextView) view.findViewById(R.id.m_status);
                    Button btn_sell = (Button) view.findViewById(R.id.m_sell);
                    Button btn_delete = (Button) view.findViewById(R.id.m_delete);
                    Button btn_close = (Button) view.findViewById(R.id.m_close);

                    // TODO : 从后端获取数据
                    String name = "小火车";
                    String catalog = "D";
                    final boolean status = Tools.getRandomInteger() % 2 == 0 ? true : false; // 是否已发售


                    idText.setText(id);
                    nameText.setText(name);
                    catalogText.setText(catalog);
                    statusText.setText(status ? "已发售" : "未发售");

                    class ModifyOnClickListener implements View.OnClickListener{
                        @Override
                        public void onClick(View v) {
                            switch (v.getId()){
                                case R.id.m_sell:
                                    if(status) Tools.toastMessage(AdminActivity.this, "请勿重复发售！");
                                    else{
                                        // TODO : 传后端发售，注意多人操作，后端可能返回false
                                        Tools.toastMessage(AdminActivity.this, "发售成功！");
                                        alertDialog.dismiss();
                                        refresh();
                                    }
                                    break;
                                case R.id.m_delete:
                                    if(status) Tools.toastMessage(AdminActivity.this, "该车票已发售，无法删除！");
                                    else{
                                        // TODO : 传后端删除
                                        Tools.toastMessage(AdminActivity.this, "删除成功！");
                                        alertDialog.dismiss();
                                        refresh();
                                    }
                                    break;
                                case R.id.m_close:
                                    alertDialog.dismiss();
                                    break;
                            }
                        }
                    }
                    btn_sell.setOnClickListener(new ModifyOnClickListener());
                    btn_delete.setOnClickListener(new ModifyOnClickListener());
                    btn_close.setOnClickListener(new ModifyOnClickListener());
                }
                else{
                    Tools.toastMessage(AdminActivity.this, "未找到对应的车次！");
                }

            }
        });

        // TODO : 从后端获得车次信息并放进listView

    }

    void refresh(){
        // TODO : 后端传数据
        // 测试用数据
        arrayAdapter.clear();
        arrayAdapter.add("c100 上海 → 北京");
        arrayAdapter.add("d100 上海交通大学 → 世界一流大学");
    }

    @Override
    protected void onResume() {
        super.onResume();
        // TODO : 因为可能已经修改，所以重新从后端获得车次信息并放进listView
    }
}

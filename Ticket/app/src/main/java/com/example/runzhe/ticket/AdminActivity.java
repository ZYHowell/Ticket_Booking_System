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
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.ArrayList;

import es.dmoral.toasty.Toasty;

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

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

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

                final String id = idText.getText().toString();
                if(Tools.isEmpty(id)) {Toasty.error(AdminActivity.this, "ID不能为空！", Toast.LENGTH_SHORT, true).show();return;}

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
                    Button btn_modify = (Button) view.findViewById(R.id.m_modify);
                    Button btn_close = (Button) view.findViewById(R.id.m_close);

                    // TODO : 从后端获取数据，以下乱搞
                    final String name = "小火车";
                    final String catalog = "D";
                    final boolean status = Tools.getRandomInteger() % 2 == 0; // 是否已发售

                    idText.setText(id);
                    nameText.setText(name);
                    catalogText.setText(catalog);
                    statusText.setText(status ? "已发售" : "未发售");

                    class ModifyOnClickListener implements View.OnClickListener{
                        @Override
                        public void onClick(View v) {
                            switch (v.getId()){
                                case R.id.m_sell:
                                    if(status)
                                        Toasty.error(AdminActivity.this, "该车票已发售，无法再次发售！", Toast.LENGTH_SHORT, true).show();
                                    else{
                                        // TODO : 请求后端判断是否能发售（多人）
                                        Toasty.success(AdminActivity.this, "发售成功！", Toast.LENGTH_SHORT, true).show();
                                        refresh();
                                        alertDialog.dismiss();
                                    }
                                    break;
                                case R.id.m_delete:
                                    if(status)
                                        Toasty.error(AdminActivity.this, "该车票已发售，无法删除！", Toast.LENGTH_SHORT, true).show();
                                    else{
                                        // TODO : 请求后端判断是否能删除（多人）
                                        Toasty.success(AdminActivity.this, "删除成功！", Toast.LENGTH_SHORT, true).show();
                                        refresh();
                                        alertDialog.dismiss();
                                    }
                                    break;
                                case R.id.m_modify:
                                    if(status)
                                        Toasty.error(AdminActivity.this, "该车票已发售，无法修改！", Toast.LENGTH_SHORT, true).show();
                                    else {
                                        // TODO : 请求后端判断是否能修改（多人）
                                        // TODO : 传后端删除

                                        Intent intent = new Intent(AdminActivity.this, NewActivity.class);
                                        intent.putExtra("id", id);
                                        intent.putExtra("name", name);
                                        intent.putExtra("catalog", catalog);
                                        startActivity(intent);

                                        Toasty.success(AdminActivity.this, "已将原车次删除，请建立新的车次！", Toast.LENGTH_SHORT, true).show();
                                        refresh();
                                        alertDialog.dismiss();
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
                    btn_modify.setOnClickListener(new ModifyOnClickListener());
                    btn_close.setOnClickListener(new ModifyOnClickListener());
                }
                else{
                    Toasty.error(AdminActivity.this, "未找到对应的车次！", Toast.LENGTH_SHORT, true).show();
                }

            }
        });

        // TODO : 从后端获得车次信息并放进listView

    }

    void refresh(){
        // TODO : 后端传数据，按照一定规则做成字符串，并放进listView
        // 测试用数据
        arrayAdapter.clear();
        arrayAdapter.add("c100 上海 → 北京");
        arrayAdapter.add("d100 上海交通大学 → 世界一流大学");
    }

    @Override
    protected void onResume() {
        super.onResume();
        refresh();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_top_admin, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.clean:
                final AlertDialog.Builder mBuilder = new AlertDialog.Builder(this);
                mBuilder.setTitle("警告");
                mBuilder.setMessage("数据一经删除将无法恢复！你确定要删库吗？");
                mBuilder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // TODO : 传后端删库
                        Toasty.success(AdminActivity.this, "删库成功！", Toast.LENGTH_SHORT, true).show();
                        dialog.dismiss();
                        refresh();
                    }
                });
                mBuilder.setNegativeButton("算了", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
                AlertDialog mDialog = mBuilder.create();
                mDialog.show();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

}

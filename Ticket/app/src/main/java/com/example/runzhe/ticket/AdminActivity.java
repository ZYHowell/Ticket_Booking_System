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
import android.widget.ImageView;
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
    ImageView trainID_Image;

    ArrayAdapter<String> arrayAdapter;

    private ProgressbarFragment progressbarFragment;

    String[] result_arr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_admin);

        progressbarFragment = new ProgressbarFragment();

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
        trainID_Image = findViewById(R.id.trainID_Clear);

        arrayAdapter = new ArrayAdapter<String>(AdminActivity.this, android.R.layout.simple_list_item_1, new ArrayList<String>());
        listView.setAdapter(arrayAdapter);
        refresh(); // 刷新所有车次列表

        trainID_Image.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                idText.setText("");
            }
        });

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
                if(Tools.isEmpty(id) || id.contains(" ")) {Toasty.error(AdminActivity.this, "ID不合法！", Toast.LENGTH_SHORT, true).show();return;}
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getSupportFragmentManager());
                    sendRequest(id);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    void refresh(){
        try {
            progressbarFragment = new ProgressbarFragment();
            progressbarFragment.setCancelable(false);
            progressbarFragment.show(getSupportFragmentManager());
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        String command = "display";
                        final String result = Tools.command(command);
                        result_arr = result.split("\n");
                        progressbarFragment.dismiss();
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                arrayAdapter.clear();
                                if (!Tools.isEmpty(result)) {
                                    String[] tmp;
                                    for (int i = 0; i < result_arr.length; i++) {
                                        tmp = result_arr[i].split(" ");
                                        arrayAdapter.add(tmp[0] + " : " + tmp[1] + " → " + tmp[2]);
                                    }
                                }
                            }
                        });
                    } catch (Exception e) {
                        Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                        progressbarFragment.dismiss();
                        e.printStackTrace();
                    }
                }
            }).start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_top_admin, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    /* 菜单项 */
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.modify_user_info:
                showDialogToGetIdAndEnter();
                break;
            case R.id.clean: {
                final AlertDialog.Builder mBuilder = new AlertDialog.Builder(this);
                mBuilder.setTitle("警告");
                mBuilder.setMessage("数据一经删除将无法恢复！你确定要删库吗？");
                mBuilder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        try {
                            progressbarFragment = new ProgressbarFragment();
                            progressbarFragment.setCancelable(false);
                            progressbarFragment.show(getSupportFragmentManager());
                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    try {
                                        String command = "clean";
                                        String result = Tools.command(command);
                                        if (result.equals("1"))
                                            Tools.showMessage(AdminActivity.this, AdminActivity.this, "删库成功！", "success");
                                        else
                                            Tools.showMessage(AdminActivity.this, AdminActivity.this, "删库失败！", "error");
                                        progressbarFragment.dismiss();
                                    } catch (Exception e) {
                                        Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                                        progressbarFragment.dismiss();
                                        e.printStackTrace();
                                    }
                                }
                            }).start();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
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
                break;
            }
            case R.id.exit: {
                final AlertDialog.Builder mBuilder = new AlertDialog.Builder(this);
                mBuilder.setTitle("警告");
                mBuilder.setMessage("你确定要结束后端吗？");
                mBuilder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        try {
                            progressbarFragment = new ProgressbarFragment();
                            progressbarFragment.setCancelable(false);
                            progressbarFragment.show(getSupportFragmentManager());
                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    try {
                                        String command = "exit";
                                        String result = Tools.command(command);
                                        if (result.equals("BYE"))
                                            Tools.showMessage(AdminActivity.this, AdminActivity.this, "后端已结束！", "success");
                                        else
                                            Tools.showMessage(AdminActivity.this, AdminActivity.this, "后端无法结束！", "error");
                                        progressbarFragment.dismiss();
                                    } catch (Exception e) {
                                        Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                                        progressbarFragment.dismiss();
                                        e.printStackTrace();
                                    }
                                }
                            }).start();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
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
                break;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    /*修改用户资料对话框*/
    void showDialogToGetIdAndEnter(){
        android.support.v7.app.AlertDialog.Builder builder = new android.support.v7.app.AlertDialog.Builder(AdminActivity.this);
        View view = View.inflate(AdminActivity.this, R.layout.dialog_find_user, null);
        final android.support.v7.app.AlertDialog alertDialog = builder.create();
        alertDialog.setView(view);
        alertDialog.show();

        final EditText userid_edit = view.findViewById(R.id.userid_query_Edit);
        Button submit_btn = view.findViewById(R.id.contain_userid_query_button);
        Button exit_btn = view.findViewById(R.id.contain_userid_exit_button);

        submit_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final String userid = userid_edit.getText().toString();
                if(Tools.isEmpty(userid) || !Tools.isNonNegtiveInteger(userid))
                    Tools.showMessage(AdminActivity.this, "ID不正确", "error");
                else{
                    try {
                        progressbarFragment.setCancelable(false);
                        progressbarFragment.show(getSupportFragmentManager());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                String command = "query_profile" + " " + userid;
                                String result = Tools.command(command);
                                if(result.equals("0")){
                                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "找不到ID对应的用户！", "error");
                                    progressbarFragment.dismiss();
                                }
                                else{
                                    String username = Tools.getNthSubstring(result, " ", 0);
                                    String email = Tools.getNthSubstring(result, " ", 1);
                                    String phone = Tools.getNthSubstring(result, " ", 2);
                                    String privilege = Tools.getNthSubstring(result, " ", 3);

                                    Intent intent = new Intent(AdminActivity.this, ModifyUserInfoActivity.class);
                                    intent.putExtra("id", userid);
                                    intent.putExtra("name", username);
                                    intent.putExtra("email", email);
                                    intent.putExtra("phone", phone);
                                    intent.putExtra("privilege", privilege);
                                    startActivity(intent);
                                    progressbarFragment.dismiss();
                                }
                            } catch (Exception e) {
                                Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                                progressbarFragment.dismiss();
                                e.printStackTrace();
                            }
                        }
                    }).start();
                }
            }
        });
        exit_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
            }
        });
    }

    /*查询车次*/
    public void sendRequest(final String id){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String command = "query_train" + " " + id;
                    final String result = Tools.command(command);
                    if (!result.equals("0")) { // 弹窗显示车次信息以便操作
                        AdminActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {

                                AlertDialog.Builder builder = new AlertDialog.Builder(AdminActivity.this);
                                View view = View.inflate(AdminActivity.this, R.layout.activity_modify_ticket, null);
                                final AlertDialog alertDialog = builder.create();
                                alertDialog.setView(view);
                                alertDialog.show();

                                // 获取控件
                                TextView idText = (TextView) view.findViewById(R.id.m_id);
                                TextView nameText = (TextView) view.findViewById(R.id.m_name);
                                TextView catalogText = (TextView) view.findViewById(R.id.m_catalog);
                                TextView time_table = view.findViewById(R.id.time_table);
                                Button btn_sell = (Button) view.findViewById(R.id.m_sell);
                                Button btn_delete = (Button) view.findViewById(R.id.m_delete);
                                Button btn_close = (Button) view.findViewById(R.id.m_close);

                                final String name = Tools.getNthSubstring(result, " ", 1);
                                final String catalog = Tools.getNthSubstring(result, " ", 2);

                                idText.setText(id);
                                nameText.setText(name);
                                catalogText.setText(catalog);
                                time_table.setText(result);

                                class ModifyOnClickListener implements View.OnClickListener {
                                    @Override
                                    public void onClick(View v) {
                                        switch (v.getId()) {
                                            case R.id.m_sell:
                                                try {
                                                    progressbarFragment.setCancelable(false);
                                                    progressbarFragment.show(getSupportFragmentManager());
                                                    new Thread(new Runnable() {
                                                        @Override
                                                        public void run() {
                                                            try {
                                                                String command = "sale_train" + " " + id;
                                                                String result = Tools.command(command);
                                                                if (result.equals("0"))
                                                                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "发售失败！", "error");
                                                                else
                                                                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "发售成功！", "success");
                                                                progressbarFragment.dismiss();
                                                            } catch (Exception e) {
                                                                Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                                                                progressbarFragment.dismiss();
                                                                e.printStackTrace();
                                                            }
                                                        }
                                                    }).start();
                                                } catch (Exception e) {
                                                    e.printStackTrace();
                                                }
                                                break;
                                            case R.id.m_delete:
                                                try {
                                                    progressbarFragment.setCancelable(false);
                                                    progressbarFragment.show(getSupportFragmentManager());
                                                    new Thread(new Runnable() {
                                                        @Override
                                                        public void run() {
                                                            try {
                                                                String command = "delete_train" + " " + id;
                                                                String result = Tools.command(command);
                                                                if (result.equals("0"))
                                                                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "删除失败！", "error");
                                                                else
                                                                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "删除成功！", "success");
                                                                progressbarFragment.dismiss();
                                                            } catch (Exception e) {
                                                                Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                                                                progressbarFragment.dismiss();
                                                                e.printStackTrace();
                                                            }
                                                        }
                                                    }).start();
                                                } catch (Exception e) {
                                                    e.printStackTrace();
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
                        });
                    } else {
                        Tools.showMessage(AdminActivity.this, AdminActivity.this, "未找到对应的车次！", "error");
                    }
                    progressbarFragment.dismiss();
                } catch (Exception e) {
                    Tools.showMessage(AdminActivity.this, AdminActivity.this, "请检查网络连接！", "warning");
                    progressbarFragment.dismiss();
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

}

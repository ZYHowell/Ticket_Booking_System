package com.example.runzhe.ticket;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private Fragment1 fragment1;
    private Fragment2 fragment2;
    private Fragment3 fragment3;

    String userid;

    private ProgressbarFragment progressbarFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        userid = getIntent().getStringExtra("userid");

        fragment1 = new Fragment1();
        fragment2 = new Fragment2();
        fragment3 = new Fragment3();

        getSupportFragmentManager().beginTransaction().add(R.id.layout_center, fragment1).commit();

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);
    }

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
                    getSupportFragmentManager().beginTransaction().replace(R.id.layout_center, fragment1).commit();
                    return true;
                case R.id.navigation_dashboard:
                    getSupportFragmentManager().beginTransaction().replace(R.id.layout_center, fragment2).commit();
                    return true;
                case R.id.navigation_notifications:
                    getSupportFragmentManager().beginTransaction().replace(R.id.layout_center, fragment3).commit();
                    return true;
            }
            return false;
        }
    };

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_top, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId())
        {
            case R.id.about_item: {
                final AlertDialog.Builder mBuilder = new AlertDialog.Builder(this);
                mBuilder.setTitle("关于");
                mBuilder.setMessage("我是订票系统V2.0，我有一段精妙的介绍，但是这里放不下了。");
                mBuilder.setPositiveButton("好吧", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
                AlertDialog mDialog = mBuilder.create();
                mDialog.show();
                break;
            }
            case R.id.admin_mode: {
                progressbarFragment = new ProgressbarFragment();
                try {
                    progressbarFragment = new ProgressbarFragment();
                    progressbarFragment.setCancelable(false);
                    progressbarFragment.show(getSupportFragmentManager());

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                String command = "query_profile" + " " + userid;
                                String result = Tools.command(command);
                                if (result.equals("0") || Tools.getNthSubstring(result, " ", 3).equals("1")) {
                                    Tools.showMessage(MainActivity.this, "无访问权限！", "error");
                                } else {
                                    progressbarFragment.dismiss();
                                    Intent intent = new Intent(MainActivity.this, AdminActivity.class);
                                    startActivity(intent);
                                }
                            } catch (Exception e) {
                                Tools.showMessage(MainActivity.this, MainActivity.this, "请检查网络连接！", "warning");
                                progressbarFragment.dismiss();
                                e.printStackTrace();
                            }
                        }
                    }).start();

                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            }
            case R.id.logout:{
                Intent intent = new Intent(MainActivity.this, LoginActivity.class);
                startActivity(intent);
                finish();
            }
        }
        return super.onOptionsItemSelected(item);
    }

}

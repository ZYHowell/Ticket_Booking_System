package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class SelectStation extends AppCompatActivity implements SortAdapter.Callback{

    private RecyclerView mRecyclerView;
    private WaveSideBar mSideBar;
    private SortAdapter mAdapter;
    private ClearEditText mClearEditText;
    private LinearLayoutManager manager;
    private String type;
    private List<SortModel> mDateList;
    private TitleItemDecoration mDecoration;
    private List<String> list;
    private ProgressbarFragment progressbarFragment;

    /**
     * 根据拼音来排列RecyclerView里面的数据类
     */
    private PinyinComparator mComparator;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select_station);

        Intent intent = getIntent();
        type = intent.getStringExtra("type");

        list = new ArrayList<>();
        progressbarFragment = new ProgressbarFragment();

        mSideBar = findViewById(R.id.sideBar);

        try {
            progressbarFragment.setCancelable(false);
            progressbarFragment.show(getSupportFragmentManager());
        } catch (Exception e) {
            e.printStackTrace();
        }
        sendRequest();
    }

    private void initViews() {
        mComparator = new PinyinComparator();
        //设置右侧SideBar触摸监听
        mSideBar.setOnTouchLetterChangeListener(new WaveSideBar.OnTouchLetterChangeListener() {
            @Override
            public void onLetterChange(String letter) {
                //该字母首次出现的位置
                int position = mAdapter.getPositionForSection(letter.charAt(0));
                if (position != -1) {
                    manager.scrollToPositionWithOffset(position, 0);
                }
            }
        });

        mRecyclerView = findViewById(R.id.rv);
        mDateList = filledData(list);

        // 根据a-z进行排序源数据
        Collections.sort(mDateList, mComparator);

        //RecyclerView设置manager
        manager = new LinearLayoutManager(this);
        mAdapter = new SortAdapter(this, mDateList);
        mDecoration = new TitleItemDecoration(this, mDateList);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                manager.setOrientation(LinearLayoutManager.VERTICAL);
                mRecyclerView.setLayoutManager(manager);
                mRecyclerView.setAdapter(mAdapter);
                //如果add两个，那么按照先后顺序，依次渲染。
                mRecyclerView.addItemDecoration(mDecoration);
                mRecyclerView.addItemDecoration(new DividerItemDecoration(SelectStation.this, DividerItemDecoration.VERTICAL));


                mClearEditText = (ClearEditText) findViewById(R.id.filter_edit);

                //根据输入框输入值的改变来过滤搜索
                mClearEditText.addTextChangedListener(new TextWatcher() {

                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {
                        //当输入框里面的值为空，更新为原来的列表，否则为过滤数据列表
                        filterData(s.toString());
                    }

                    @Override
                    public void beforeTextChanged(CharSequence s, int start, int count,
                                                  int after) {

                    }

                    @Override
                    public void afterTextChanged(Editable s) {
                    }
                });
                try{
                    progressbarFragment.dismiss(); // 关闭加载界面
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onClick(String station){  // 选好车票了，返回
        Intent intent = new Intent();
        intent.putExtra("station", station);
        intent.putExtra("type", type);
//        intent.putStringArrayListExtra("list", (ArrayList<String>)list);
        setResult(1, intent);
        finish();
    }

    /**
     * 为RecyclerView填充数据
     */

    private List<SortModel> filledData(List<String> date) {
        List<SortModel> mSortList = new ArrayList<>();

        for (int i = 0; i < date.size(); i++) {
            SortModel sortModel = new SortModel();
            sortModel.setName(date.get(i));
            //汉字转换成拼音
            String pinyin = PinyinUtils.getPingYin(date.get(i));
            String sortString = pinyin.substring(0, 1).toUpperCase();

            // 正则表达式，判断首字母是否是英文字母
            if (sortString.matches("[A-Z]")) {
                sortModel.setLetters(sortString.toUpperCase());
            } else {
                sortModel.setLetters("#");
            }

            mSortList.add(sortModel);
        }
        return mSortList;

    }

    /**
     * 根据输入框中的值来过滤数据并更新RecyclerView
     */

    private void filterData(String filterStr) {
        List<SortModel> filterDateList = new ArrayList<>();

        if (TextUtils.isEmpty(filterStr)) {
            filterDateList = filledData(list);
        } else {
            filterDateList.clear();
            for (SortModel sortModel : mDateList) {
                String name = sortModel.getName();
                if (name.indexOf(filterStr.toString()) != -1 ||
                        PinyinUtils.getFirstSpell(name).startsWith(filterStr.toString())
                        //不区分大小写
                        || PinyinUtils.getFirstSpell(name).toLowerCase().startsWith(filterStr.toString())
                        || PinyinUtils.getFirstSpell(name).toUpperCase().startsWith(filterStr.toString())
                        ) {
                    filterDateList.add(sortModel);
                }
            }
        }

        // 根据a-z进行排序
        Collections.sort(filterDateList, mComparator);
        mDateList.clear();
        mDateList.addAll(filterDateList);
        mAdapter.notifyDataSetChanged();
    }

    private void sendRequest(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {

                    // TODO : 后端传来所有站点名称，并add进去

                    list.add("StationA");
                    list.add("StationB");
                    for(int i = 1; i <= 100; i++) list.add("fuckyou" + i);
                    for(int i = 1; i <= 100; i++) list.add("dfasdg" + i);

                    initViews();

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
}

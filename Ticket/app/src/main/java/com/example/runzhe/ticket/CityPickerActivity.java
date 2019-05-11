package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

import com.zaaach.citypicker.CityPicker;
import com.zaaach.citypicker.adapter.OnPickListener;
import com.zaaach.citypicker.model.City;
import com.zaaach.citypicker.model.HotCity;
import com.zaaach.citypicker.model.LocateState;
import com.zaaach.citypicker.model.LocatedCity;

import java.util.ArrayList;
import java.util.List;

public class CityPickerActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        List<HotCity> hotCities = new ArrayList<>();
        hotCities.add(new HotCity("北京", "北京", "101010100")); //code为城市代码
        hotCities.add(new HotCity("上海", "上海", "101020100"));
        hotCities.add(new HotCity("广州", "广东", "101280101"));
        hotCities.add(new HotCity("深圳", "广东", "101280601"));
        hotCities.add(new HotCity("杭州", "浙江", "101210101"));

        CityPicker.from(CityPickerActivity.this) //activity或者fragment
                //.enableAnimation(true)	//启用动画效果，默认无
                //.setAnimationStyle(anim)	//自定义动画
                .setLocatedCity(new LocatedCity("上海", "上海", "101020100"))  //APP自身已定位的城市，传null会自动定位（默认）
                .setHotCities(hotCities)	//指定热门城市
                .setOnPickListener(new OnPickListener() {
                    @Override
                    public void onPick(int position, City data) {
                        //Toast.makeText(getApplicationContext(), data.getName(), Toast.LENGTH_SHORT).show();
                        Intent intent = getIntent();
                        intent.putExtra("station", data.getName());
                        setResult(1, intent);
                        finish();
                    }

                    @Override
                    public void onCancel(){
                        finish();
                    }

                    @Override
                    public void onLocate() {
                        //定位接口，需要APP自身实现，这里模拟一下定位
                        new Handler().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                // TODO : 不会定位，所以空着
                                //定位完成之后更新数据
                                /*
                                CityPicker.getInstance()
                                        .locateComplete(new LocatedCity("深圳", "广东", "101280601"), LocateState.SUCCESS);
                                */
                            }
                        }, 3000);
                    }
                })
                .show();

    }
}

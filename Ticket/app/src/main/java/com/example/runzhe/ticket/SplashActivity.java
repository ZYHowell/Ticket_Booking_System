package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.widget.ImageView;

import com.bumptech.glide.Glide;

import java.net.ServerSocket;
import java.time.chrono.MinguoChronology;

public class SplashActivity extends AppCompatActivity {

    ImageView splashPicture;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        splashPicture = (ImageView) findViewById(R.id.splashPicture);
        Glide.with(SplashActivity.this)
                .load(R.drawable.cxk)
                .centerCrop()
                .into(splashPicture);

        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(SplashActivity.this, LoginActivity.class);
//                intent.putExtra("userid", "23333333"); // XXX
                startActivity(intent);
                SplashActivity.this.finish();
            }
        }, 0);
//        }, 4500);

    }
}

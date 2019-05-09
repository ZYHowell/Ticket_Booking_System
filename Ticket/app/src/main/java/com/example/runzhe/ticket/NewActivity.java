package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;

public class NewActivity extends AppCompatActivity {

    EditText id, name, catalog;
    CheckBox cb1, cb2, cb3;
    Button new_train;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new);

        id = (EditText) findViewById(R.id.n_id);
        name = (EditText) findViewById(R.id.n_name);
        catalog = (EditText) findViewById(R.id.n_catalog);
        cb1 = (CheckBox) findViewById(R.id.n_type_1);
        cb2 = (CheckBox) findViewById(R.id.n_type_2);
        cb3 = (CheckBox) findViewById(R.id.n_type_3);
        new_train = (Button) findViewById(R.id.n_new_train);

        id.setText(getIntent().getStringExtra("id"));
        name.setText(getIntent().getStringExtra("name"));
        catalog.setText(getIntent().getStringExtra("catalog"));

        new_train.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                // TODO : 判断id等是否非重复  判断各项非空

                Intent intent = new Intent(NewActivity.this, New2Activity.class);

                intent.putExtra("id", id.getText());
                intent.putExtra("name", name.getText());
                intent.putExtra("catalog", catalog.getText());
                intent.putExtra("tic1", cb1.isChecked());
                intent.putExtra("tic2", cb2.isChecked());
                intent.putExtra("tic3", cb3.isChecked());

                startActivity(intent);
                finish();
            }
        });


    }

}

package com.example.runzhe.ticket;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;

import java.util.ArrayList;
import java.util.List;

public class NewActivity extends AppCompatActivity {

    EditText id, name;
    Spinner catalog;
    Button new_train;

    CheckBox[] cb;
    List<String> catalog_list;
    ArrayAdapter<String> arrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new);

        android.support.v7.app.ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        findAllView();
        initCatalogSpinner();
        initCheckBox();

//        id.setText(getIntent().getStringExtra("id"));
//        name.setText(getIntent().getStringExtra("name"));
//        catalog.setText(getIntent().getStringExtra("catalog"));

        new_train.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Tools.isEmpty(id.getText().toString()) || Tools.isEmpty(name.getText().toString())
                        || id.getText().toString().contains(" ") || name.getText().toString().contains(" ")){
                    Tools.showMessage(NewActivity.this, "车次信息填写有误！", "error");
                    return;
                }

                Intent intent = new Intent(NewActivity.this, New2Activity.class);

                intent.putExtra("id", id.getText().toString());
                intent.putExtra("name", name.getText().toString());
                intent.putExtra("catalog", Tools.getTrainCatalogs(catalog.getSelectedItemPosition()));
                for(int i = 0; i < 11; i++)
                    intent.putExtra("ticket_type_" + i, cb[i].isChecked());

                startActivity(intent);
                finish();
            }
        });
    }

    void findAllView(){
        id =  findViewById(R.id.n_id);
        name =  findViewById(R.id.n_name);
        catalog =  findViewById(R.id.n_catalog);
        new_train =  findViewById(R.id.n_new_train);

        cb = new CheckBox[11];
        cb[0] = findViewById(R.id.train_checkbox_1);        cb[1] = findViewById(R.id.train_checkbox_2);
        cb[2] = findViewById(R.id.train_checkbox_3);        cb[3] = findViewById(R.id.train_checkbox_4);
        cb[4] = findViewById(R.id.train_checkbox_5);        cb[5] = findViewById(R.id.train_checkbox_6);
        cb[6] = findViewById(R.id.train_checkbox_7);        cb[7] = findViewById(R.id.train_checkbox_8);
        cb[8] = findViewById(R.id.train_checkbox_9);        cb[9] = findViewById(R.id.train_checkbox_10);
        cb[10] = findViewById(R.id.train_checkbox_11);
    }
    void initCatalogSpinner(){
        catalog_list = new ArrayList<String>();
        for(int i = 0; i < 7; i++)
            catalog_list.add(Tools.getTrainCatalogs(i));
        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, catalog_list);
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        catalog.setAdapter(arrayAdapter);
        catalog.setSelection(0);
    }
    void initCheckBox(){
        for(int i = 0; i < 11; i++) cb[i].setText(Tools.getSeatType(i));
    }

    @Override
    public boolean onSupportNavigateUp() {
        finish();
        return super.onSupportNavigateUp();
    }

}

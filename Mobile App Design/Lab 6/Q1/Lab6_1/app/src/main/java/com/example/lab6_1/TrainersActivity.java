package com.example.lab6_1;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class TrainersActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_trainers);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("Trainers");
        }
    }
}
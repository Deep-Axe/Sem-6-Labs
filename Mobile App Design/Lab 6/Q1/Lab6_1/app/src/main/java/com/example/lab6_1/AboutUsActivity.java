package com.example.lab6_1;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class AboutUsActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about_us);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("About Us");
        }
    }
}
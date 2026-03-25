package com.example.lab6_1;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class ContactUsActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_contact_us);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("Contact Us");
        }
    }
}
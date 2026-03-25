package com.example.lab6_1;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class MembershipActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_membership);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("Membership");
        }
    }
}
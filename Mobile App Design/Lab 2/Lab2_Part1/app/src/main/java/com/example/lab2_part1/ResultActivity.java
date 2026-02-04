package com.example.lab2_part1;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

public class ResultActivity extends AppCompatActivity {

    private static final String TAG = "LifecycleResult";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_result);
        
        showToast("ResultActivity: onCreate()");
        Log.d(TAG, "onCreate() called");

        TextView textViewResult = findViewById(R.id.textViewResult);
        Button buttonBack = findViewById(R.id.buttonBack);

        String result = getIntent().getStringExtra("result");
        textViewResult.setText(result);

        buttonBack.setOnClickListener(v -> finish());
    }

    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onStart() {
        super.onStart();
        showToast("ResultActivity: onStart()");
        Log.d(TAG, "onStart() called");
    }

    @Override
    protected void onResume() {
        super.onResume();
        showToast("ResultActivity: onResume()");
        Log.d(TAG, "onResume() called");
    }

    @Override
    protected void onPause() {
        super.onPause();
        showToast("ResultActivity: onPause()");
        Log.d(TAG, "onPause() called");
    }

    @Override
    protected void onStop() {
        super.onStop();
        showToast("ResultActivity: onStop()");
        Log.d(TAG, "onStop() called");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        showToast("ResultActivity: onDestroy()");
        Log.d(TAG, "onDestroy() called");
    }
}
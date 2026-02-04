package com.example.lab2_part1;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import com.example.lab2_part1.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "LifecycleMain";
    private ActivityMainBinding binding;
    private EditText editTextNumber1, editTextNumber2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        setSupportActionBar(binding.toolbar);

        showToast("MainActivity: onCreate()");
        Log.d(TAG, "onCreate() called");

        editTextNumber1 = findViewById(R.id.editTextNumber1);
        editTextNumber2 = findViewById(R.id.editTextNumber2);

        findViewById(R.id.buttonAdd).setOnClickListener(v -> calculate('+'));
        findViewById(R.id.buttonSubtract).setOnClickListener(v -> calculate('-'));
        findViewById(R.id.buttonMultiply).setOnClickListener(v -> calculate('*'));
        findViewById(R.id.buttonDivide).setOnClickListener(v -> calculate('/'));
    }

    private void calculate(char operator) {
        String num1Str = editTextNumber1.getText().toString();
        String num2Str = editTextNumber2.getText().toString();

        if (num1Str.isEmpty() || num2Str.isEmpty()) {
            Toast.makeText(this, "Please enter both numbers", Toast.LENGTH_SHORT).show();
            return;
        }

        double num1 = Double.parseDouble(num1Str);
        double num2 = Double.parseDouble(num2Str);
        double result = 0;

        if (operator == '/' && num2 == 0) {
            new AlertDialog.Builder(this)
                    .setTitle("Error")
                    .setMessage("Division by zero is not allowed.")
                    .setPositiveButton("OK", null)
                    .show();
            return;
        }

        switch (operator) {
            case '+': result = num1 + num2; break;
            case '-': result = num1 - num2; break;
            case '*': result = num1 * num2; break;
            case '/': result = num1 / num2; break;
        }

        String resultText = num1 + " " + operator + " " + num2 + " = " + result;
        Intent intent = new Intent(this, ResultActivity.class);
        intent.putExtra("result", resultText);
        startActivity(intent);
    }

    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onStart() {
        super.onStart();
        showToast("MainActivity: onStart()");
        Log.d(TAG, "onStart() called");
    }

    @Override
    protected void onResume() {
        super.onResume();
        showToast("MainActivity: onResume()");
        Log.d(TAG, "onResume() called");
    }

    @Override
    protected void onPause() {
        super.onPause();
        showToast("MainActivity: onPause()");
        Log.d(TAG, "onPause() called");
    }

    @Override
    protected void onStop() {
        super.onStop();
        showToast("MainActivity: onStop()");
        Log.d(TAG, "onStop() called");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        showToast("MainActivity: onDestroy()");
        Log.d(TAG, "onDestroy() called");
    }
}
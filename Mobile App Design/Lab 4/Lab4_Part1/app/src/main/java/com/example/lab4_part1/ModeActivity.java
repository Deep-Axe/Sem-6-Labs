package com.example.lab4_part1;

import android.os.Bundle;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.appcompat.app.AppCompatActivity;

public class ModeActivity extends AppCompatActivity {

    private ImageView imgMode;
    private ToggleButton toggleMode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mode);

        imgMode = findViewById(R.id.imgMode);
        toggleMode = findViewById(R.id.toggleMode);
        Button btnChangeMode = findViewById(R.id.btnChangeMode);
        Button btnBack = findViewById(R.id.btnBackFromMode);

        // Set initial state
        updateModeUI();

        toggleMode.setOnClickListener(v -> {
            updateModeUI();
            showModeToast();
        });

        btnChangeMode.setOnClickListener(v -> {
            toggleMode.setChecked(!toggleMode.isChecked());
            updateModeUI();
            showModeToast();
        });

        btnBack.setOnClickListener(v -> finish());
    }

    private void updateModeUI() {
        if (toggleMode.isChecked()) {
            // Wi-Fi Mode
            imgMode.setImageResource(R.drawable.ic_launcher_foreground);
        } else {
            // Mobile Data Mode
            imgMode.setImageResource(R.drawable.ic_launcher_background);
        }
    }

    private void showModeToast() {
        String mode = toggleMode.isChecked() ? "Wi-Fi" : "Mobile Data";
        Toast.makeText(this, "Current Mode: " + mode, Toast.LENGTH_SHORT).show();
    }
}

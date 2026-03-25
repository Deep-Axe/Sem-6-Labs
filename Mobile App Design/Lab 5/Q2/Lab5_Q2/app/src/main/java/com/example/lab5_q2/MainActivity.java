package com.example.lab5_q2;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.Spinner;
import android.widget.ToggleButton;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Calendar;

public class MainActivity extends AppCompatActivity {

    private Spinner spinnerSource, spinnerDestination;
    private DatePicker datePicker;
    private ToggleButton toggleTripType;
    private Button btnSubmit, btnReset;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize UI components
        spinnerSource = findViewById(R.id.spinnerSource);
        spinnerDestination = findViewById(R.id.spinnerDestination);
        datePicker = findViewById(R.id.datePicker);
        toggleTripType = findViewById(R.id.toggleTripType);
        btnSubmit = findViewById(R.id.btnSubmit);
        btnReset = findViewById(R.id.btnReset);

        // Set minimum date to today to prevent booking in the past
        datePicker.setMinDate(System.currentTimeMillis());

        // Submit Button Click Listener
        btnSubmit.setOnClickListener(v -> {
            String source = spinnerSource.getSelectedItem().toString();
            String destination = spinnerDestination.getSelectedItem().toString();
            
            int day = datePicker.getDayOfMonth();
            int month = datePicker.getMonth() + 1; // Months are 0-indexed
            int year = datePicker.getYear();
            String date = day + "/" + month + "/" + year;

            String tripType = toggleTripType.isChecked() ? "Round-trip" : "One-way";

            if (source.equals(destination)) {
                Toast.makeText(this, "Source and destination cannot be the same", Toast.LENGTH_SHORT).show();
                return;
            }

            String details = "Source: " + source + "\n" +
                             "Destination: " + destination + "\n" +
                             "Date: " + date + "\n" +
                             "Trip Type: " + tripType;

            Intent intent = new Intent(MainActivity.this, DetailsActivity.class);
            intent.putExtra("DETAILS", details);
            startActivity(intent);
        });

        // Reset Button Click Listener
        btnReset.setOnClickListener(v -> {
            resetFields();
        });
    }

    private void resetFields() {
        spinnerSource.setSelection(0);
        spinnerDestination.setSelection(0);
        
        Calendar calendar = Calendar.getInstance();
        datePicker.updateDate(calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH));
        
        toggleTripType.setChecked(false);
        
        Toast.makeText(this, "Inputs Reset", Toast.LENGTH_SHORT).show();
    }
}
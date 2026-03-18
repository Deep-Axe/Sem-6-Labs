package com.example.labmidsem_practise2;

import android.content.DialogInterface;
import android.content.Intent;
import android.app.AlertDialog;
import android.view.View;
import android.os.Bundle;
import android.widget.DatePicker;
import android.widget.ToggleButton;
import android.widget.Spinner;
import android.widget.Button;
import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        DatePicker datePicker = findViewById(R.id.datePicker);
        ToggleButton statusToggle = findViewById(R.id.statusToggle);
        Spinner zoneSpinner = findViewById(R.id.spinner);
        Button submitButton = findViewById(R.id.button);

        long today = System.currentTimeMillis() - 1000;
        long twentyDaysinMillis = 20L * 24 * 60 * 60 * 1000L;
        datePicker.setMinDate(today);
        datePicker.setMaxDate(today + twentyDaysinMillis);

        submitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new AlertDialog.Builder(MainActivity.this)
                        .setTitle(("Confirm Submission"))
                        .setMessage(("Are you sure you want to submit?"))
                        .setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                int day = datePicker.getDayOfMonth();
                                int month = datePicker.getMonth() + 1;
                                int year = datePicker.getYear();
                                String SelectedDate = day + "/" + month + "/" + year;

                                String selectedZone = zoneSpinner.getSelectedItem().toString();
                                String systemStatus = statusToggle.isChecked() ? "Active" : "Inactive";

                                Intent intent = new Intent(MainActivity.this, MainActivity2.class);
                                intent.putExtra("Log_Date", SelectedDate);
                                intent.putExtra("Log_Zone", selectedZone);
                                intent.putExtra("Log_Status", systemStatus);
                                startActivity(intent);
                            }
                        })
                        .setNegativeButton("Canel", null)
                        .show();
            }
        });
    }
}
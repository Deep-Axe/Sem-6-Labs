package com.example.lab5q3;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.Spinner;
import android.widget.TimePicker;
import android.widget.ToggleButton;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Calendar;

public class MainActivity extends AppCompatActivity {

    private Spinner spinnerMovie, spinnerTheatre;
    private DatePicker datePicker;
    private TimePicker timePicker;
    private ToggleButton togglePremium;
    private Button btnBookNow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        spinnerMovie = findViewById(R.id.spinnerMovie);
        spinnerTheatre = findViewById(R.id.spinnerTheatre);
        datePicker = findViewById(R.id.datePicker);
        timePicker = findViewById(R.id.timePicker);
        togglePremium = findViewById(R.id.togglePremium);
        btnBookNow = findViewById(R.id.btnBookNow);
        Button btnReset = findViewById(R.id.btnReset);

        // Setup Spinners
        String[] movies = {"Inception", "Interstellar", "The Dark Knight", "Oppenheimer"};
        String[] theatres = {"PVR Cinemas", "Cinepolis", "INOX", "IMAX"};

        ArrayAdapter<String> movieAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, movies);
        movieAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerMovie.setAdapter(movieAdapter);

        ArrayAdapter<String> theatreAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, theatres);
        theatreAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerTheatre.setAdapter(theatreAdapter);

        // Set Minimum Date to Today
        datePicker.setMinDate(System.currentTimeMillis() - 1000);

        // Listeners for real-time validation
        togglePremium.setOnCheckedChangeListener((buttonView, isChecked) -> updateBookNowButtonState());
        
        Calendar calendar = Calendar.getInstance();
        datePicker.init(calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH), 
            (view, year, monthOfYear, dayOfMonth) -> updateBookNowButtonState());

        timePicker.setOnTimeChangedListener((view, hourOfDay, minute) -> updateBookNowButtonState());

        btnBookNow.setOnClickListener(v -> bookTicket());
        btnReset.setOnClickListener(v -> resetFields());

        updateBookNowButtonState();
    }

    private void updateBookNowButtonState() {
        boolean isValid = true;

        Calendar current = Calendar.getInstance();
        Calendar selected = Calendar.getInstance();
        selected.set(datePicker.getYear(), datePicker.getMonth(), datePicker.getDayOfMonth(), 
                     timePicker.getHour(), timePicker.getMinute());

        // 1. Prevent Past Date/Time
        if (selected.before(current)) {
            isValid = false;
        }

        // 2. Premium Requirement: Show time must be 12:00 PM or later
        if (togglePremium.isChecked()) {
            if (timePicker.getHour() < 12) {
                isValid = false;
            }
        }

        btnBookNow.setEnabled(isValid);
    }

    private void bookTicket() {
        String movie = spinnerMovie.getSelectedItem().toString();
        String theatre = spinnerTheatre.getSelectedItem().toString();
        String date = datePicker.getDayOfMonth() + "/" + (datePicker.getMonth() + 1) + "/" + datePicker.getYear();
        String time = String.format("%02d:%02d", timePicker.getHour(), timePicker.getMinute());
        String ticketType = togglePremium.isChecked() ? "Premium" : "Standard";

        Intent intent = new Intent(MainActivity.this, BookingDetailsActivity.class);
        intent.putExtra("MOVIE", movie);
        intent.putExtra("THEATRE", theatre);
        intent.putExtra("DATE", date);
        intent.putExtra("TIME", time);
        intent.putExtra("TYPE", ticketType);
        startActivity(intent);
    }

    private void resetFields() {
        spinnerMovie.setSelection(0);
        spinnerTheatre.setSelection(0);
        
        Calendar calendar = Calendar.getInstance();
        datePicker.updateDate(calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH));
        
        timePicker.setHour(calendar.get(Calendar.HOUR_OF_DAY));
        timePicker.setMinute(calendar.get(Calendar.MINUTE));
        
        togglePremium.setChecked(false);
        updateBookNowButtonState();
        
        Toast.makeText(this, "Fields Reset", Toast.LENGTH_SHORT).show();
    }
}

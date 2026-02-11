package com.example.q2;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class DetailsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_details);

        TextView tvSource = findViewById(R.id.tvSource);
        TextView tvDestination = findViewById(R.id.tvDestination);
        TextView tvDate = findViewById(R.id.tvDate);
        TextView tvTripType = findViewById(R.id.tvTripType);
        Button btnBack = findViewById(R.id.btnBack);

        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            tvSource.setText("Source: " + extras.getString("SOURCE"));
            tvDestination.setText("Destination: " + extras.getString("DESTINATION"));
            tvDate.setText("Date of Travel: " + extras.getString("DATE"));
            tvTripType.setText("Trip Type: " + extras.getString("TRIP_TYPE"));
        }

        btnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }
}
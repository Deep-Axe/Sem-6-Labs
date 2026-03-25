package com.example.lab5q3;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class BookingDetailsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_booking_details);

        TextView tvMovieDetail = findViewById(R.id.tvMovieDetail);
        TextView tvTheatreDetail = findViewById(R.id.tvTheatreDetail);
        TextView tvDateDetail = findViewById(R.id.tvDateDetail);
        TextView tvTimeDetail = findViewById(R.id.tvTimeDetail);
        TextView tvTypeDetail = findViewById(R.id.tvTypeDetail);
        Button btnBack = findViewById(R.id.btnBack);

        Intent intent = getIntent();
        if (intent != null) {
            tvMovieDetail.setText(intent.getStringExtra("MOVIE"));
            tvTheatreDetail.setText(intent.getStringExtra("THEATRE"));
            tvDateDetail.setText(intent.getStringExtra("DATE"));
            tvTimeDetail.setText(intent.getStringExtra("TIME"));
            tvTypeDetail.setText(intent.getStringExtra("TYPE"));
        }

        btnBack.setOnClickListener(v -> finish());
    }
}

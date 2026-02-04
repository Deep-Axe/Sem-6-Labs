package com.example.lab4_food_ordering;

import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class OrderSummaryActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_order_summary);

        TextView tvOrderDetails = findViewById(R.id.tvOrderDetails);
        TextView tvTotalCost = findViewById(R.id.tvTotalCost);
        Button btnBack = findViewById(R.id.btnBack);

        String details = getIntent().getStringExtra("ORDER_DETAILS");
        int total = getIntent().getIntExtra("TOTAL_COST", 0);

        tvOrderDetails.setText(details);
        tvTotalCost.setText("TOTAL CREDITS: $" + total);

        btnBack.setOnClickListener(v -> finish());
    }
}
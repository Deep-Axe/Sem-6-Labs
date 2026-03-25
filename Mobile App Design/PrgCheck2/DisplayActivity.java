package com.example.prgcheck;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class DisplayActivity extends AppCompatActivity {

    TextView tvName, tvItem, tvQuantity, tvPrice;
    Button btnBack;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display);

        tvName = findViewById(R.id.tvDisplayName);
        tvItem = findViewById(R.id.tvDisplayItem);
        tvQuantity = findViewById(R.id.tvDisplayQuantity);
        tvPrice = findViewById(R.id.tvDisplayPrice);
        btnBack = findViewById(R.id.btnBackToStart);

        String name = getIntent().getStringExtra("customerName");
        String item = getIntent().getStringExtra("itemName");
        int quantity = getIntent().getIntExtra("quantity", 0);
        double price = getIntent().getDoubleExtra("price", 0.0);

        tvName.setText("Customer: " + name);
        tvItem.setText("Item: " + item);
        tvQuantity.setText("Quantity: " + quantity);
        tvPrice.setText("Total Price: " + (price * quantity));

        btnBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(DisplayActivity.this, MainActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            }
        });
    }
}

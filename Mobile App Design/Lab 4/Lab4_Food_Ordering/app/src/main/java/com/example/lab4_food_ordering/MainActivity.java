package com.example.lab4_food_ordering;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.CheckBox;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class MainActivity extends AppCompatActivity {

    private CheckBox cbPizza, cbBurger, cbCoffee, cbPasta;
    private Button btnSubmit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        cbPizza = findViewById(R.id.cbPizza);
        cbBurger = findViewById(R.id.cbBurger);
        cbCoffee = findViewById(R.id.cbCoffee);
        cbPasta = findViewById(R.id.cbPasta);
        btnSubmit = findViewById(R.id.btnSubmit);

        btnSubmit.setOnClickListener(v -> {
            int total = 0;
            StringBuilder result = new StringBuilder();

            if (cbPizza.isChecked()) {
                result.append("Cyber Pizza: $15\n");
                total += 15;
            }
            if (cbBurger.isChecked()) {
                result.append("Neon Burger: $10\n");
                total += 10;
            }
            if (cbCoffee.isChecked()) {
                result.append("Quantum Coffee: $5\n");
                total += 5;
            }
            if (cbPasta.isChecked()) {
                result.append("Plasma Pasta: $12\n");
                total += 12;
            }

            if (result.length() == 0) {
                result.append("No items selected.\n");
            }

            // Lock the order as per requirement
            cbPizza.setEnabled(false);
            cbBurger.setEnabled(false);
            cbCoffee.setEnabled(false);
            cbPasta.setEnabled(false);
            btnSubmit.setEnabled(false);
            btnSubmit.setText("ORDER LOCKED");

            Intent intent = new Intent(MainActivity.this, OrderSummaryActivity.class);
            intent.putExtra("ORDER_DETAILS", result.toString());
            intent.putExtra("TOTAL_COST", total);
            startActivity(intent);
        });
    }
}
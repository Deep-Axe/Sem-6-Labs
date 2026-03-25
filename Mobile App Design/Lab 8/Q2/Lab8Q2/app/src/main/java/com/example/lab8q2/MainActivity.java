package com.example.lab8q2;

import android.database.Cursor;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private EditText etItemName, etItemCost;
    private Button btnAddItem, btnAddToCart;
    private Spinner spinnerItems;
    private TextView tvTotalCost;
    private DatabaseHelper dbHelper;
    private List<String> itemDisplayList;
    private List<Double> itemCosts;
    private double totalCost = 0.0;
    private static final String TAG = "GroceryApp";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        etItemName = findViewById(R.id.etItemName);
        etItemCost = findViewById(R.id.etItemCost);
        btnAddItem = findViewById(R.id.btnAddItem);
        btnAddToCart = findViewById(R.id.btnAddToCart);
        spinnerItems = findViewById(R.id.spinnerItems);
        tvTotalCost = findViewById(R.id.tvTotalCost);

        dbHelper = new DatabaseHelper(this);
        loadSpinnerData();

        btnAddItem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String name = etItemName.getText().toString().trim();
                String costStr = etItemCost.getText().toString().trim();

                if (!name.isEmpty() && !costStr.isEmpty()) {
                    try {
                        double cost = Double.parseDouble(costStr);
                        dbHelper.addItem(name, cost);
                        Log.d(TAG, "Added item: " + name + " with cost: " + cost);
                        
                        etItemName.setText("");
                        etItemCost.setText("");
                        loadSpinnerData();
                        Toast.makeText(MainActivity.this, "Item Added Successfully", Toast.LENGTH_SHORT).show();
                    } catch (NumberFormatException e) {
                        Toast.makeText(MainActivity.this, "Invalid cost value", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    Toast.makeText(MainActivity.this, "Please enter both name and cost", Toast.LENGTH_SHORT).show();
                }
            }
        });

        btnAddToCart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int position = spinnerItems.getSelectedItemPosition();
                if (position != Spinner.INVALID_POSITION && !itemCosts.isEmpty()) {
                    double selectedCost = itemCosts.get(position);
                    totalCost += selectedCost;
                    tvTotalCost.setText("Total Cost: " + String.format("%.2f", totalCost));
                    Log.d(TAG, "Added " + itemDisplayList.get(position) + " to cart. Current Total: " + totalCost);
                } else {
                    Toast.makeText(MainActivity.this, "No item selected", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void loadSpinnerData() {
        itemDisplayList = new ArrayList<>();
        itemCosts = new ArrayList<>();
        Cursor cursor = dbHelper.getAllItems();

        if (cursor != null && cursor.moveToFirst()) {
            do {
                String name = cursor.getString(cursor.getColumnIndexOrThrow("name"));
                double cost = cursor.getDouble(cursor.getColumnIndexOrThrow("cost"));
                itemDisplayList.add(name + " - Rs." + cost);
                itemCosts.add(cost);
            } while (cursor.moveToNext());
            cursor.close();
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, itemDisplayList);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerItems.setAdapter(adapter);
    }
}

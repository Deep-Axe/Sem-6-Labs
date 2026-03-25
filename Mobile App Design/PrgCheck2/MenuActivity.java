package com.example.prgcheck;

import android.content.ContentValues;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MenuActivity extends AppCompatActivity {

    Spinner spinnerItem, spinnerPrice;
    Button btnAdd, btnNext;
    DatabaseHelper dbHelper;

    String[] items = {"Burger", "Pizza", "Pasta", "Salad", "Soda"};
    String[] prices = {"5.99", "8.99", "7.49", "4.99", "1.99"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_menu);

        spinnerItem = findViewById(R.id.spinnerItem);
        spinnerPrice = findViewById(R.id.spinnerPrice);
        btnAdd = findViewById(R.id.btnAddMenu);
        btnNext = findViewById(R.id.btnNextToTransaction);
        dbHelper = new DatabaseHelper(this);

        ArrayAdapter<String> itemAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, items);
        itemAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerItem.setAdapter(itemAdapter);

        ArrayAdapter<String> priceAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, prices);
        priceAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerPrice.setAdapter(priceAdapter);

        btnAdd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String item = spinnerItem.getSelectedItem().toString();
                String priceStr = spinnerPrice.getSelectedItem().toString();

                SQLiteDatabase db = dbHelper.getWritableDatabase();
                ContentValues values = new ContentValues();
                values.put("item_name", item);
                values.put("price", Double.parseDouble(priceStr));

                long id = db.insert("Menu", null, values);
                if (id != -1) {
                    Toast.makeText(MenuActivity.this, "Item Added to Menu", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(MenuActivity.this, "Error adding item", Toast.LENGTH_SHORT).show();
                }
            }
        });

        btnNext.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MenuActivity.this, TransactionActivity.class);
                startActivity(intent);
            }
        });
    }
}

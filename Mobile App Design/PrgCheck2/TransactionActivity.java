package com.example.prgcheck;

import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;
import java.util.List;

public class TransactionActivity extends AppCompatActivity {

    Spinner spinnerCustomer, spinnerItem, spinnerPrice;
    AutoCompleteTextView autoQuantity;
    Button btnCreate;
    DatabaseHelper dbHelper;
    List<String> customerNames = new ArrayList<>();
    List<Integer> customerIds = new ArrayList<>();
    List<String> itemNames = new ArrayList<>();
    List<Double> itemPrices = new ArrayList<>();
    List<Integer> itemIds = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_transaction);

        spinnerCustomer = findViewById(R.id.spinnerCustomer);
        spinnerItem = findViewById(R.id.spinnerTransactionItem);
        spinnerPrice = findViewById(R.id.spinnerTransactionPrice);
        autoQuantity = findViewById(R.id.autoQuantity);
        btnCreate = findViewById(R.id.btnCreateTransaction);
        dbHelper = new DatabaseHelper(this);

        loadData();

        String[] quantities = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
        ArrayAdapter<String> qAdapter = new ArrayAdapter<>(this, android.R.layout.simple_dropdown_item_1line, quantities);
        autoQuantity.setAdapter(qAdapter);
        
        // Show dropdown suggestions when the field is clicked
        autoQuantity.setOnClickListener(v -> autoQuantity.showDropDown());

        btnCreate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String qStr = autoQuantity.getText().toString();
                if (customerIds.isEmpty() || itemIds.isEmpty() || qStr.isEmpty()) {
                    Toast.makeText(TransactionActivity.this, "Please fill all fields", Toast.LENGTH_SHORT).show();
                    return;
                }

                int quantity;
                try {
                    quantity = Integer.parseInt(qStr);
                } catch (NumberFormatException e) {
                    Toast.makeText(TransactionActivity.this, "Invalid quantity", Toast.LENGTH_SHORT).show();
                    return;
                }

                int customerPos = spinnerCustomer.getSelectedItemPosition();
                int itemPos = spinnerItem.getSelectedItemPosition();

                int customerId = customerIds.get(customerPos);
                int itemId = itemIds.get(itemPos);

                SQLiteDatabase db = dbHelper.getWritableDatabase();
                ContentValues values = new ContentValues();
                values.put("customer_id", customerId);
                values.put("menu_id", itemId);
                values.put("quantity", quantity);

                long id = db.insert("Transactions", null, values);
                if (id != -1) {
                    Intent intent = new Intent(TransactionActivity.this, DisplayActivity.class);
                    intent.putExtra("customerName", customerNames.get(customerPos));
                    intent.putExtra("itemName", itemNames.get(itemPos));
                    intent.putExtra("quantity", quantity);
                    intent.putExtra("price", itemPrices.get(itemPos));
                    startActivity(intent);
                } else {
                    Toast.makeText(TransactionActivity.this, "Error creating transaction", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void loadData() {
        SQLiteDatabase db = dbHelper.getReadableDatabase();

        // Load Customers
        Cursor cursor = db.rawQuery("SELECT id, name FROM Customers", null);
        while (cursor.moveToNext()) {
            customerIds.add(cursor.getInt(0));
            customerNames.add(cursor.getString(1));
        }
        cursor.close();
        ArrayAdapter<String> cAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, customerNames);
        spinnerCustomer.setAdapter(cAdapter);

        // Load Menu Items and Prices
        cursor = db.rawQuery("SELECT id, item_name, price FROM Menu", null);
        while (cursor.moveToNext()) {
            itemIds.add(cursor.getInt(0));
            itemNames.add(cursor.getString(1));
            itemPrices.add(cursor.getDouble(2));
        }
        cursor.close();
        ArrayAdapter<String> iAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, itemNames);
        spinnerItem.setAdapter(iAdapter);

        List<String> prices = new ArrayList<>();
        for (Double p : itemPrices) prices.add(String.valueOf(p));
        ArrayAdapter<String> pAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, prices);
        spinnerPrice.setAdapter(pAdapter);
    }
}

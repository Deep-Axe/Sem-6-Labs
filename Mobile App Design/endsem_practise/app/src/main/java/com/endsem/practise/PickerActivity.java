package com.endsem.practise;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

public class PickerActivity extends AppCompatActivity {

    DatabaseHelper db;
    Spinner spinnerPapers;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_picker);

        db = new DatabaseHelper(this);
        spinnerPapers = findViewById(R.id.spinnerPapers);
        Button btnEdit = findViewById(R.id.btnEdit);
        Button btnDelete = findViewById(R.id.btnDelete);

        loadPapers();

        btnEdit.setOnClickListener(v -> {
            if (spinnerPapers.getSelectedItem() != null) {
                String title = spinnerPapers.getSelectedItem().toString();
                Intent intent = new Intent(PickerActivity.this, ReviewActivity.class);
                intent.putExtra("PAPER_TITLE", title);
                startActivity(intent);
            }
        });

        btnDelete.setOnClickListener(v -> {
            if (spinnerPapers.getSelectedItem() != null) {
                String title = spinnerPapers.getSelectedItem().toString();
                showDeleteConfirmDialog(title);
            }
        });
    }

    private void loadPapers() {
        Cursor cursor = db.getAllPapers();
        ArrayList<String> titles = new ArrayList<>();
        if (cursor.moveToFirst()) {
            do {
                titles.add(cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_TITLE)));
            } while (cursor.moveToNext());
        }
        cursor.close();

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, titles);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerPapers.setAdapter(adapter);

        if (titles.isEmpty()) {
            Toast.makeText(this, "No submissions found.", Toast.LENGTH_SHORT).show();
        }
    }

    private void showDeleteConfirmDialog(String title) {
        new AlertDialog.Builder(this)
                .setTitle("Confirm Delete")
                .setMessage("Are you sure you want to delete '" + title + "'?")
                .setPositiveButton("Confirm", (dialog, which) -> {
                    db.deletePaper(title);
                    Toast.makeText(PickerActivity.this, "Submission deleted.", Toast.LENGTH_SHORT).show();
                    loadPapers(); // Refresh
                })
                .setNegativeButton("Cancel", null)
                .show();
    }
}

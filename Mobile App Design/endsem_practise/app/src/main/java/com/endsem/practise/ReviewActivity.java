package com.endsem.practise;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import java.util.ArrayList;

public class ReviewActivity extends AppCompatActivity {

    DatabaseHelper db;
    String paperTitle;
    TableLayout tableDetails;
    ListView lvSameDomain;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_review);

        db = new DatabaseHelper(this);
        paperTitle = getIntent().getStringExtra("PAPER_TITLE");

        tableDetails = findViewById(R.id.tableDetails);
        lvSameDomain = findViewById(R.id.lvSameDomain);

        loadPaperDetails();
        
        findViewById(R.id.btnRunStats).setOnClickListener(v -> runStats());
        findViewById(R.id.btnDelete).setOnClickListener(v -> confirmDelete());
        findViewById(R.id.btnFinalize).setOnClickListener(v -> {
            Intent intent = new Intent(ReviewActivity.this, ReviewerAssignmentActivity.class);
            intent.putExtra("PAPER_TITLE", paperTitle);
            startActivity(intent);
        });
    }

    private void loadPaperDetails() {
        Cursor cursor = db.getPaperByTitle(paperTitle);
        if (cursor.moveToFirst()) {
            String domain = "";
            for (int i = 0; i < cursor.getColumnCount(); i++) {
                String colName = cursor.getColumnName(i);
                String value = cursor.getString(i);
                if (colName.equals(DatabaseHelper.COL_DOMAIN)) domain = value;
                addTableRow(colName, value);
            }
            loadSameDomainPapers(domain);
        }
        cursor.close();
    }

    private void addTableRow(String field, String value) {
        TableRow row = new TableRow(this);
        TextView tvField = new TextView(this);
        tvField.setText(field);
        tvField.setPadding(10, 10, 10, 10);
        TextView tvValue = new TextView(this);
        tvValue.setText(value);
        tvValue.setPadding(10, 10, 10, 10);
        row.addView(tvField);
        row.addView(tvValue);
        tableDetails.addView(row);
    }

    private void loadSameDomainPapers(String domain) {
        Cursor cursor = db.getPapersInDomain(domain);
        ArrayList<String> list = new ArrayList<>();
        if (cursor.moveToFirst()) {
            do {
                String title = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_TITLE));
                String author = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_AUTHOR));
                list.add(title + " (by " + author + ")");
            } while (cursor.moveToNext());
        }
        cursor.close();
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, list);
        lvSameDomain.setAdapter(adapter);
    }

    private void runStats() {
        Cursor cursor = db.getStats();
        StringBuilder stats = new StringBuilder("Papers per Domain:\n");
        if (cursor.moveToFirst()) {
            do {
                stats.append(cursor.getString(1)).append(": ").append(cursor.getInt(0)).append("\n");
            } while (cursor.moveToNext());
        }
        cursor.close();
        new AlertDialog.Builder(this).setTitle("Domain Statistics").setMessage(stats.toString()).setPositiveButton("OK", null).show();
    }

    private void confirmDelete() {
        new AlertDialog.Builder(this)
                .setTitle("Delete Submission")
                .setMessage("Are you sure?")
                .setPositiveButton("Confirm", (dialog, which) -> {
                    db.deletePaper(paperTitle);
                    Toast.makeText(this, "Submission deleted.", Toast.LENGTH_SHORT).show();
                    Intent intent = new Intent(this, MainActivity.class);
                    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    startActivity(intent);
                })
                .setNegativeButton("Cancel", null)
                .show();
    }
}

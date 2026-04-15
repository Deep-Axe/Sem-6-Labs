package com.endsem.practise;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

public class FinalSummaryActivity extends AppCompatActivity {

    DatabaseHelper db;
    String paperTitle;
    TextView tvTitle, tvAuthor, tvReviewer, tvScore;
    TableLayout tableFinal;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_final_summary);

        db = new DatabaseHelper(this);
        paperTitle = getIntent().getStringExtra("PAPER_TITLE");

        tvTitle = findViewById(R.id.tvSummaryTitle);
        tvAuthor = findViewById(R.id.tvSummaryAuthor);
        tvReviewer = findViewById(R.id.tvSummaryReviewer);
        tvScore = findViewById(R.id.tvSummaryScore);
        tableFinal = findViewById(R.id.tableFinal);

        loadData();

        findViewById(R.id.btnExport).setOnClickListener(v -> exportSummary());
        findViewById(R.id.btnHome).setOnClickListener(v -> {
            Intent intent = new Intent(this, MainActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);
        });
    }

    private void loadData() {
        Cursor cursor = db.getPaperByTitle(paperTitle);
        if (cursor.moveToFirst()) {
            String author = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_AUTHOR));
            String reviewer = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_REVIEWER));
            int score = cursor.getInt(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_CONFIDENCE));

            tvTitle.setText("Title: " + paperTitle);
            tvAuthor.setText("Author: " + author);
            tvReviewer.setText("Reviewer: " + reviewer);
            tvScore.setText("Confidence Score: " + score);

            // Add DB specific fields to Table
            addTableRow("Email", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_EMAIL)));
            addTableRow("Type", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_TYPE)));
            addTableRow("Domain", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_DOMAIN)));
            addTableRow("Deadline", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_DEADLINE)));
            addTableRow("Track", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_TRACK)));
            addTableRow("Keywords", cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_KEYWORDS)));
        }
        cursor.close();
    }

    private void addTableRow(String key, String value) {
        TableRow row = new TableRow(this);
        TextView tvK = new TextView(this);
        tvK.setText(key);
        tvK.setPadding(10, 10, 10, 10);
        TextView tvV = new TextView(this);
        tvV.setText(value);
        tvV.setPadding(10, 10, 10, 10);
        row.addView(tvK);
        row.addView(tvV);
        tableFinal.addView(row);
    }

    private void exportSummary() {
        StringBuilder sb = new StringBuilder();
        sb.append("CONFERENCE SUBMISSION EXPORT\n");
        sb.append("============================\n");
        sb.append("Title: ").append(paperTitle).append("\n");
        sb.append(tvAuthor.getText()).append("\n");
        sb.append(tvReviewer.getText()).append("\n");
        sb.append(tvScore.getText()).append("\n");
        
        // Add others from table or re-fetch
        Cursor cursor = db.getPaperByTitle(paperTitle);
        if (cursor.moveToFirst()) {
            sb.append("Email: ").append(cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_EMAIL))).append("\n");
            sb.append("Track: ").append(cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COL_TRACK))).append("\n");
        }
        cursor.close();

        new AlertDialog.Builder(this)
                .setTitle("Export Preview")
                .setMessage(sb.toString())
                .setPositiveButton("Close", null)
                .show();
    }
}

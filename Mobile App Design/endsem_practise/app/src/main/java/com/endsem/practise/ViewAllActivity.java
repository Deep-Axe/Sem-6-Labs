package com.endsem.practise;

import android.database.Cursor;
import android.os.Bundle;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class ViewAllActivity extends AppCompatActivity {

    DatabaseHelper db;
    TableLayout tableAll;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_all);

        db = new DatabaseHelper(this);
        tableAll = findViewById(R.id.tableAll);

        loadAllData();
    }

    private void loadAllData() {
        Cursor cursor = db.getAllPapers();
        if (cursor.moveToFirst()) {
            // Add Header Row
            TableRow header = new TableRow(this);
            for (int i = 0; i < cursor.getColumnCount(); i++) {
                header.addView(createTextView(cursor.getColumnName(i), true));
            }
            tableAll.addView(header);

            // Add Data Rows
            do {
                TableRow row = new TableRow(this);
                for (int i = 0; i < cursor.getColumnCount(); i++) {
                    row.addView(createTextView(cursor.getString(i), false));
                }
                tableAll.addView(row);
            } while (cursor.moveToNext());
        }
        cursor.close();
    }

    private TextView createTextView(String text, boolean isHeader) {
        TextView tv = new TextView(this);
        tv.setText(text != null ? text : "");
        tv.setPadding(15, 10, 15, 10);
        if (isHeader) tv.setTypeface(null, android.graphics.Typeface.BOLD);
        return tv;
    }
}

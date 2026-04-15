package com.endsem.practise;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import androidx.appcompat.widget.PopupMenu;
import android.widget.DatePicker;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import java.util.ArrayList;
import java.util.Calendar;

public class SchedulingActivity extends AppCompatActivity {

    TextView tvHeader, tvKeywords;
    DatePicker datePicker;
    TimePicker timePicker;
    Spinner spinnerTrack;
    DatabaseHelper db;
    String paperTitle, authorName;
    ArrayList<String> selectedKeywords = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scheduling);

        db = new DatabaseHelper(this);
        paperTitle = getIntent().getStringExtra("PAPER_TITLE");
        authorName = getIntent().getStringExtra("AUTHOR_NAME");

        tvHeader = findViewById(R.id.tvHeader);
        tvHeader.setText("Title: " + paperTitle + "\nAuthor: " + authorName);

        datePicker = findViewById(R.id.datePicker);
        timePicker = findViewById(R.id.timePicker);
        spinnerTrack = findViewById(R.id.spinnerTrack);
        tvKeywords = findViewById(R.id.tvSelectedKeywords);

        findViewById(R.id.btnSelectKeywords).setOnClickListener(v -> showKeywordMenu(v));
        findViewById(R.id.btnBack).setOnClickListener(v -> finish());
        findViewById(R.id.btnSubmit).setOnClickListener(v -> submit());
    }

    private void showKeywordMenu(View v) {
        PopupMenu popup = new PopupMenu(this, v);
        String[] keywords = {"Deep Learning", "Federated Learning", "Edge Computing", "Network Security", "Data Privacy", "Computer Vision"};
        
        for (String kw : keywords) {
            String title = kw;
            if (selectedKeywords.contains(kw)) title = "✓ " + kw;
            popup.getMenu().add(title);
        }

        popup.setOnMenuItemClickListener(item -> {
            String title = item.getTitle().toString().replace("✓ ", "");
            if (selectedKeywords.contains(title)) {
                selectedKeywords.remove(title);
            } else {
                if (selectedKeywords.size() < 2) {
                    selectedKeywords.add(title);
                } else {
                    Toast.makeText(this, "Select up to 2 only", Toast.LENGTH_SHORT).show();
                }
            }
            tvKeywords.setText("Keywords: " + TextUtils.join(", ", selectedKeywords));
            return true;
        });
        popup.show();
    }

    private void submit() {
        Calendar selectedDate = Calendar.getInstance();
        selectedDate.set(datePicker.getYear(), datePicker.getMonth(), datePicker.getDayOfMonth(), 0, 0, 0);
        selectedDate.set(Calendar.MILLISECOND, 0);

        Calendar today = Calendar.getInstance();
        today.set(Calendar.HOUR_OF_DAY, 0);
        today.set(Calendar.MINUTE, 0);
        today.set(Calendar.SECOND, 0);
        today.set(Calendar.MILLISECOND, 0);

        if (selectedDate.before(today)) {
            Toast.makeText(this, "Deadline cannot be in the past", Toast.LENGTH_SHORT).show();
            return;
        }

        String dateStr = datePicker.getDayOfMonth() + "/" + (datePicker.getMonth() + 1) + "/" + datePicker.getYear();
        String timeStr = timePicker.getHour() + ":" + String.format("%02d", timePicker.getMinute());
        String track = spinnerTrack.getSelectedItem().toString();
        String kws = TextUtils.join(", ", selectedKeywords);

        db.updateScheduling(paperTitle, dateStr, timeStr, track, kws);
        
        Intent intent = new Intent(SchedulingActivity.this, ReviewActivity.class);
        intent.putExtra("PAPER_TITLE", paperTitle);
        intent.putExtra("AUTHOR_NAME", authorName);
        startActivity(intent);
    }
}

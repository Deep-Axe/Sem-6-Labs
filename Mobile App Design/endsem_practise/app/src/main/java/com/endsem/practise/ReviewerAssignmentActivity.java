package com.endsem.practise;

import android.content.Intent;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

public class ReviewerAssignmentActivity extends AppCompatActivity {

    DatabaseHelper db;
    String paperTitle;
    TextView tvTitle, tvConfidence;
    Spinner spinnerReviewer;
    SeekBar seekBarConfidence;
    ListView lvSimilar;
    String[] dummyPapers = {"Survey on AI in Medicine", "Security in Smart Grids", "Efficient Routing in IoT", "ML for Bio-sequences"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_reviewer_assignment);

        db = new DatabaseHelper(this);
        paperTitle = getIntent().getStringExtra("PAPER_TITLE");

        tvTitle = findViewById(R.id.tvTitle);
        tvTitle.setText("Paper: " + paperTitle);

        spinnerReviewer = findViewById(R.id.spinnerReviewer);
        tvConfidence = findViewById(R.id.tvConfidence);
        seekBarConfidence = findViewById(R.id.seekBarConfidence);
        lvSimilar = findViewById(R.id.lvSimilarPapers);

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, dummyPapers);
        lvSimilar.setAdapter(adapter);
        registerForContextMenu(lvSimilar);

        seekBarConfidence.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                tvConfidence.setText("Confidence Score: " + progress);
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        findViewById(R.id.btnAssignFinalize).setOnClickListener(v -> finalizeAssignment());
    }

    private void finalizeAssignment() {
        String reviewer = spinnerReviewer.getSelectedItem().toString();
        int score = seekBarConfidence.getProgress();
        db.updateReviewer(paperTitle, reviewer, score);

        new AlertDialog.Builder(this)
                .setTitle("Assignment Finalized")
                .setMessage("Reviewer: " + reviewer + "\nScore: " + score)
                .setPositiveButton("Go to Summary", (dialog, which) -> {
                    Intent intent = new Intent(ReviewerAssignmentActivity.this, FinalSummaryActivity.class);
                    intent.putExtra("PAPER_TITLE", paperTitle);
                    startActivity(intent);
                })
                .setNegativeButton("Cancel", null)
                .show();
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);
        getMenuInflater().inflate(R.menu.context_menu, menu);
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
        String paper = dummyPapers[info.position];
        Toast.makeText(this, item.getTitle() + ": " + paper, Toast.LENGTH_SHORT).show();
        return super.onContextItemSelected(item);
    }
}

package com.endsem.practise;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class MainActivity extends AppCompatActivity {

    private DatabaseHelper db;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        
        db = new DatabaseHelper(this);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        Button btnNewSubmission = findViewById(R.id.btnNewSubmission);
        Button btnReviewExisting = findViewById(R.id.btnReviewExisting);

        btnNewSubmission.setOnClickListener(v -> {
            startActivity(new Intent(MainActivity.this, AuthorDetailsActivity.class));
        });

        btnReviewExisting.setOnClickListener(v -> {
            startActivity(new Intent(MainActivity.this, PickerActivity.class));
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.view_all) {
            startActivity(new Intent(this, ViewAllActivity.class));
            return true;
        } else if (id == R.id.about) {
            new AlertDialog.Builder(this)
                    .setTitle("About Conference")
                    .setMessage("International Conference on Modern Trends 2024\n\nManage your research papers effectively.")
                    .setPositiveButton("OK", null)
                    .show();
            return true;
        } else if (id == R.id.clear_data) {
            db.clearAll();
            Toast.makeText(this, "All data cleared.", Toast.LENGTH_SHORT).show();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}

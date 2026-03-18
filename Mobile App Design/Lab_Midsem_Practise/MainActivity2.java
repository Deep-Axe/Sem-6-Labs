package com.example.labmidsem_practise2;

import android.os.Bundle;
import com.google.android.material.snackbar.Snackbar;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.content.Intent;
import android.widget.TextView;
import android.widget.Toast;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import androidx.appcompat.widget.Toolbar;
import com.example.labmidsem_practise2.databinding.ActivityMain2Binding;

public class MainActivity2 extends AppCompatActivity {

    private TextView displayDataText;
    private String passedZone, passedDate, passedStatus;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        Intent intent = getIntent();
        passedDate = intent.getStringExtra("Log_Date");
        passedZone = intent.getStringExtra("Log_Zone");
        passedStatus = intent.getStringExtra("Log_Status");

        displayDataText = findViewById(R.id.textView);
        if (passedZone != null) {
            String formattedText = "Log Details:\n\n" +
                    "Date: " + passedDate + "\n" +
                    "Zone: " + passedZone + "\n" +
                    "Status: " + passedStatus;
            displayDataText.setText(formattedText);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_second, menu);
        return true;
    }

    // 5. Handle Menu Clicks
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == android.R.id.home) {
            finish();
            return true;
        } else if (id == R.id.action_reset) {
            displayDataText.setText("Data Cleared.\nPlease return to the main screen.");
            Toast.makeText(this, "Form Reset", Toast.LENGTH_SHORT).show();
            return true;
        } else if (id == R.id.action_view_static) {
            String staticInfo = "STATIC STATION INFO\n\n" +
                    "Zone Alpha: 5 Extinguishers\n" +
                    "Zone Beta: 3 Extinguishers\n" +
                    "Zone Gamma: 8 Extinguishers";
            displayDataText.setText(staticInfo);
            Toast.makeText(this, "Viewing Static Data", Toast.LENGTH_SHORT).show();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
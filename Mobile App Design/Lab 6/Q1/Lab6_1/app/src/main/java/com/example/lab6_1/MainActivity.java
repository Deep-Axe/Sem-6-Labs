package com.example.lab6_1;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("XYZ Fitness Center");
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.menu_workout_plans) {
            startActivity(new Intent(this, WorkoutPlansActivity.class));
            return true;
        } else if (id == R.id.menu_trainers) {
            startActivity(new Intent(this, TrainersActivity.class));
            return true;
        } else if (id == R.id.menu_membership) {
            startActivity(new Intent(this, MembershipActivity.class));
            return true;
        } else if (id == R.id.menu_home) {
            Toast.makeText(this, "You are already on the Homepage", Toast.LENGTH_SHORT).show();
            return true;
        } else if (id == R.id.menu_about_us) {
            startActivity(new Intent(this, AboutUsActivity.class));
            return true;
        } else if (id == R.id.menu_contact_us) {
            startActivity(new Intent(this, ContactUsActivity.class));
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
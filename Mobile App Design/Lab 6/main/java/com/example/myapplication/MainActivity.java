package com.example.myapplication;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

public class MainActivity extends AppCompatActivity {

    private TextView contentDisplay;
    private ImageView imageDisplay;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        contentDisplay = findViewById(R.id.content_display);
        imageDisplay = findViewById(R.id.image_display);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int id = item.getItemId();
        imageDisplay.setVisibility(View.GONE); // Hide by default

        if (id == R.id.menu_homepage) {
            contentDisplay.setText("XYZ Fitness Center - Homepage\n\nYour journey to fitness starts here!");
            return true;
        } else if (id == R.id.menu_about_us) {
            contentDisplay.setText("About Us\n\nXYZ Fitness Center has been providing top-notch fitness services since 2010.");
            return true;
        } else if (id == R.id.menu_contact_us) {
            contentDisplay.setText("Contact Us\n\nEmail: info@xyzfitness.com\nPhone: +1 234 567 890\nAddress: 123 Fitness St.");
            return true;
        } else if (id == R.id.menu_workout_plans) {
            contentDisplay.setText("Workout Plans\n\n1. Weight Loss - Focused on calorie burning.\n2. Cardio - Improving heart health.\n3. Strength Training - Build muscle.");
            return true;
        } else if (id == R.id.menu_trainers) {
            imageDisplay.setVisibility(View.VISIBLE);
            // Using a system drawable as a placeholder for trainer photo
            imageDisplay.setImageResource(android.R.drawable.ic_menu_myplaces); 
            contentDisplay.setText("Our Trainers\n\n1. John Doe - Specialization: Yoga & Flexibility.\n2. Jane Smith - Specialization: HIIT & Strength.\n3. Mike Ross - Specialization: Bodybuilding.");
            return true;
        } else if (id == R.id.menu_membership) {
            contentDisplay.setText("Membership Packages\n\n- Basic: $29/mo\n- Premium: $49/mo\n- Elite: $79/mo");
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
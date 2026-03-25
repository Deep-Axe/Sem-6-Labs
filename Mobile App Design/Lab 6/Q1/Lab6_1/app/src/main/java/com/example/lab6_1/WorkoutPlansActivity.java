package com.example.lab6_1;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class WorkoutPlansActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_workout_plans);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle("Workout Plans");
        }
    }
}
package com.example.lab4_part1;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnShowToast = findViewById(R.id.btnShowToast);
        ToggleButton toggleBtnToast = findViewById(R.id.toggleBtnToast);

        btnShowToast.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showCustomToast("Button Clicked!", R.drawable.ic_launcher_foreground);
                showNavigationDialog(ModeActivity.class, "Do you want to go to the Mode Selection screen?");
            }
        });

        toggleBtnToast.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String status = toggleBtnToast.isChecked() ? "Toggle ON" : "Toggle OFF";
                showCustomToast(status, R.drawable.ic_launcher_background);
                showNavigationDialog(VersionsActivity.class, "Do you want to go to the Android Versions screen?");
            }
        });
    }

    private void showNavigationDialog(Class<?> targetActivity, String message) {
        new AlertDialog.Builder(this)
                .setTitle("Navigation")
                .setMessage(message)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Intent intent = new Intent(MainActivity.this, targetActivity);
                        startActivity(intent);
                    }
                })
                .setNegativeButton("No", null)
                .show();
    }

    private void showCustomToast(String message, int imageResId) {
        LayoutInflater inflater = getLayoutInflater();
        View layout = inflater.inflate(R.layout.custom_toast, (ViewGroup) findViewById(R.id.custom_toast_container));

        ImageView imageView = layout.findViewById(R.id.toast_image);
        imageView.setImageResource(imageResId);

        TextView textView = layout.findViewById(R.id.toast_text);
        textView.setText(message);

        Toast toast = new Toast(getApplicationContext());
        toast.setDuration(Toast.LENGTH_SHORT);
        toast.setView(layout);
        toast.show();
    }
}

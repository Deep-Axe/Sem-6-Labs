package com.example.lab4_part1;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class VersionsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_versions);

        Button btn12 = findViewById(R.id.btnAndroid12);
        Button btn13 = findViewById(R.id.btnAndroid13);
        Button btn14 = findViewById(R.id.btnAndroid14);
        Button btnBack = findViewById(R.id.btnBack);

        btn12.setOnClickListener(v -> showCustomToast("Android 12 (S)", R.drawable.ic_launcher_foreground));
        btn13.setOnClickListener(v -> showCustomToast("Android 13 (Tiramisu)", R.drawable.ic_launcher_background));
        btn14.setOnClickListener(v -> showCustomToast("Android 14 (Upside Down Cake)", R.drawable.ic_launcher_foreground));

        btnBack.setOnClickListener(v -> finish());
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

package com.example.lab_1;

import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView helloWorldText = findViewById(R.id.hello_world_text);
        
        // Show "Hello World!" then change it after 2 seconds
        new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
            @Override
            public void run() {
                helloWorldText.setText(R.string.modified_text);
            }
        }, 2000);

        TextView customFontText = findViewById(R.id.custom_font_text);
        // Using a built-in monospace typeface as a "custom" example
        customFontText.setTypeface(Typeface.MONOSPACE, Typeface.BOLD);
    }
}
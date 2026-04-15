package com.endsem.practise;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

public class LoginActivity extends AppCompatActivity {

    EditText etUsername, etPassword;
    Button btnSubmit;
    TextView tvToggle, tvTitle;
    DatabaseHelper db;
    boolean isLoginMode = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        db = new DatabaseHelper(this);
        etUsername = findViewById(R.id.etUsername);
        etPassword = findViewById(R.id.etPassword);
        btnSubmit = findViewById(R.id.btnSubmit);
        tvToggle = findViewById(R.id.tvToggle);
        tvTitle = findViewById(R.id.tvTitle);

        tvToggle.setOnClickListener(v -> {
            isLoginMode = !isLoginMode;
            if (isLoginMode) {
                tvTitle.setText("Login");
                btnSubmit.setText("Login");
                tvToggle.setText("Don't have an account? Signup");
            } else {
                tvTitle.setText("Signup");
                btnSubmit.setText("Signup");
                tvToggle.setText("Already have an account? Login");
            }
        });

        btnSubmit.setOnClickListener(v -> {
            String user = etUsername.getText().toString().trim();
            String pass = etPassword.getText().toString().trim();

            if (user.isEmpty() || pass.isEmpty()) {
                Toast.makeText(this, "Please enter all fields", Toast.LENGTH_SHORT).show();
                return;
            }

            if (isLoginMode) {
                // Login Logic
                if (db.loginUser(user, pass)) {
                    Toast.makeText(this, "Login Successful", Toast.LENGTH_SHORT).show();
                    startActivity(new Intent(LoginActivity.this, MainActivity.class));
                    finish();
                } else {
                    Toast.makeText(this, "Invalid Credentials", Toast.LENGTH_SHORT).show();
                }
            } else {
                // Signup Logic
                long result = db.signupUser(user, pass);
                if (result != -1) {
                    Toast.makeText(this, "Signup Successful! Now please login.", Toast.LENGTH_SHORT).show();
                    // Switch to login mode automatically
                    tvToggle.performClick();
                } else {
                    Toast.makeText(this, "User already exists or error occurred", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
}

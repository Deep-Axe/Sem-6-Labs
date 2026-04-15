package com.endsem.practise;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Toast;
import android.widget.ToggleButton;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

public class AuthorDetailsActivity extends AppCompatActivity {

    EditText etName, etEmail, etTitle;
    RadioGroup rgType;
    Spinner spinnerDomain;
    ToggleButton toggleCoAuthors;
    LinearLayout layoutCoAuthors;
    CheckBox cbCo1, cbCo2, cbCo3;
    DatabaseHelper db;

    // To store co-author details
    String[] coAuthorNames = {"", "", ""};
    String[] coAuthorRoles = {"", "", ""};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_author_details);

        db = new DatabaseHelper(this);
        etName = findViewById(R.id.etAuthorName);
        etEmail = findViewById(R.id.etEmail);
        etTitle = findViewById(R.id.etPaperTitle);
        rgType = findViewById(R.id.rgType);
        spinnerDomain = findViewById(R.id.spinnerDomain);
        toggleCoAuthors = findViewById(R.id.toggleCoAuthors);
        layoutCoAuthors = findViewById(R.id.layoutCoAuthors);

        cbCo1 = findViewById(R.id.cbCo1);
        cbCo2 = findViewById(R.id.cbCo2);
        cbCo3 = findViewById(R.id.cbCo3);

        toggleCoAuthors.setOnCheckedChangeListener((buttonView, isChecked) -> {
            layoutCoAuthors.setVisibility(isChecked ? View.VISIBLE : View.GONE);
        });

        cbCo1.setOnClickListener(v -> handleCoAuthorClick(0, cbCo1));
        cbCo2.setOnClickListener(v -> handleCoAuthorClick(1, cbCo2));
        cbCo3.setOnClickListener(v -> handleCoAuthorClick(2, cbCo3));

        findViewById(R.id.btnBack).setOnClickListener(v -> finish());

        findViewById(R.id.btnNext).setOnClickListener(v -> {
            if (validate()) {
                showSummaryDialog();
            } else {
                Toast.makeText(this, "Please fill all required fields", Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void handleCoAuthorClick(int index, CheckBox cb) {
        if (cb.isChecked()) {
            showCoAuthorDialog(index, cb);
        } else {
            coAuthorNames[index] = "";
            coAuthorRoles[index] = "";
            cb.setText("Co-author " + (index + 1));
        }
    }

    private void showCoAuthorDialog(int index, CheckBox cb) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Co-Author " + (index + 1) + " Details");

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(50, 40, 50, 10);

        final EditText etCoName = new EditText(this);
        etCoName.setHint("Name");
        etCoName.setText(coAuthorNames[index]);
        layout.addView(etCoName);

        final Spinner spRole = new Spinner(this);
        String[] roles = {"Researcher", "Student", "Professor", "Industry Expert"};
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, roles);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spRole.setAdapter(adapter);
        
        for (int i = 0; i < roles.length; i++) {
            if (roles[i].equals(coAuthorRoles[index])) {
                spRole.setSelection(i);
                break;
            }
        }
        layout.addView(spRole);

        builder.setView(layout);
        builder.setPositiveButton("OK", (dialog, which) -> {
            String name = etCoName.getText().toString();
            if (name.isEmpty()) {
                cb.setChecked(false);
                coAuthorNames[index] = "";
                coAuthorRoles[index] = "";
                cb.setText("Co-author " + (index + 1));
            } else {
                coAuthorNames[index] = name;
                coAuthorRoles[index] = spRole.getSelectedItem().toString();
                cb.setText("✓ " + name + " (" + coAuthorRoles[index] + ")");
            }
        });
        builder.setNegativeButton("Cancel", (dialog, which) -> {
            if (coAuthorNames[index].isEmpty()) {
                cb.setChecked(false);
            }
        });
        builder.setCancelable(false);
        builder.show();
    }

    private boolean validate() {
        return !etName.getText().toString().isEmpty() &&
               !etEmail.getText().toString().isEmpty() &&
               !etTitle.getText().toString().isEmpty() &&
               rgType.getCheckedRadioButtonId() != -1;
    }

    private void showSummaryDialog() {
        int selectedId = rgType.getCheckedRadioButtonId();
        RadioButton rb = findViewById(selectedId);
        String type = rb.getText().toString();
        String domain = spinnerDomain.getSelectedItem().toString();

        StringBuilder coAuthorsData = new StringBuilder();
        for (int i = 0; i < 3; i++) {
            if (!coAuthorNames[i].isEmpty()) {
                if (coAuthorsData.length() > 0) coAuthorsData.append("; ");
                coAuthorsData.append(coAuthorNames[i]).append(" (").append(coAuthorRoles[i]).append(")");
            }
        }

        String summary = "Author: " + etName.getText().toString() + "\n" +
                         "Email: " + etEmail.getText().toString() + "\n" +
                         "Title: " + etTitle.getText().toString() + "\n" +
                         "Type: " + type + "\n" +
                         "Domain: " + domain + "\n" +
                         "Co-authors: " + (coAuthorsData.length() > 0 ? coAuthorsData.toString() : "None");

        new AlertDialog.Builder(this)
                .setTitle("Confirm Summary")
                .setMessage(summary)
                .setPositiveButton("Confirm", (dialog, which) -> {
                    long result = db.insertPaper(etName.getText().toString(), etEmail.getText().toString(),
                            etTitle.getText().toString(), type, domain, coAuthorsData.toString());
                    
                    if (result == -1) {
                        Toast.makeText(this, "Error: Paper Title already exists!", Toast.LENGTH_SHORT).show();
                    } else {
                        Intent intent = new Intent(AuthorDetailsActivity.this, SchedulingActivity.class);
                        intent.putExtra("PAPER_TITLE", etTitle.getText().toString());
                        intent.putExtra("AUTHOR_NAME", etName.getText().toString());
                        startActivity(intent);
                    }
                })
                .setNegativeButton("Edit", null)
                .show();
    }
}

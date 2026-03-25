package com.example.lab7_q3;

import android.graphics.Color;
import android.os.Bundle;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.BackgroundColorSpan;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private TextView textViewContent;
    private String originalContent;
    private String currentKeyword = "";

    private static final String DIGITAL_TRANSFORMATION_TEXT = 
            "Digital transformation is the process of using digital technologies to create new or modify existing business processes, culture, and customer experiences to meet changing business and market requirements. " +
            "This reimagining of business in the digital age is digital transformation.\n\n" +
            "It transcends traditional roles like sales, marketing, and customer service. Instead, digital transformation begins and ends with how you think about, and engage with, customers. " +
            "As we move from paper to spreadsheets to smart applications for managing our business, we have the chance to reimagine how we do business how we engage our customers with digital technology on our side.\n\n" +
            "For small businesses just getting started, there's no need to set up your business processes and transform them later. You can future-proof your organization from the word go. " +
            "Building a 21st-century business on sticks and bricks and paper is just not sustainable. Thinking, planning, and building digitally sets you up to be agile, flexible, and ready to grow.\n\n" +
            "As they embark on digital transformation, many companies are taking a step back to ask if they are really doing the right things.";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        textViewContent = findViewById(R.id.textViewContent);
        originalContent = DIGITAL_TRANSFORMATION_TEXT;
        textViewContent.setText(originalContent);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.submenu_search) {
            showInputDialog("Search Keywords", (input) -> {
                currentKeyword = input;
                searchContent(input);
            });
            return true;
        } else if (id == R.id.submenu_highlight) {
            showInputDialog("Highlight Keywords", (input) -> {
                highlightContent(input);
            });
            return true;
        } else if (id == R.id.sort_alphabetically) {
            sortContent(false);
            return true;
        } else if (id == R.id.sort_relevance) {
            sortContent(true);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void showInputDialog(String title, InputCallback callback) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title);
        final EditText input = new EditText(this);
        builder.setView(input);
        builder.setPositiveButton("OK", (dialog, which) -> callback.onInputReceived(input.getText().toString()));
        builder.setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        builder.show();
    }

    private void searchContent(String keyword) {
        if (keyword.isEmpty()) {
            Toast.makeText(this, "Please enter a keyword", Toast.LENGTH_SHORT).show();
            return;
        }
        int count = 0;
        int index = originalContent.toLowerCase().indexOf(keyword.toLowerCase());
        while (index != -1) {
            count++;
            index = originalContent.toLowerCase().indexOf(keyword.toLowerCase(), index + 1);
        }
        Toast.makeText(this, "Found " + count + " occurrences of '" + keyword + "'", Toast.LENGTH_SHORT).show();
    }

    private void highlightContent(String keyword) {
        if (keyword.isEmpty()) {
            textViewContent.setText(originalContent);
            return;
        }
        SpannableString spannable = new SpannableString(textViewContent.getText().toString());
        String text = spannable.toString().toLowerCase();
        keyword = keyword.toLowerCase();
        
        int index = text.indexOf(keyword);
        while (index != -1) {
            spannable.setSpan(new BackgroundColorSpan(Color.YELLOW), index, index + keyword.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            index = text.indexOf(keyword, index + 1);
        }
        textViewContent.setText(spannable);
    }

    private void sortContent(boolean byRelevance) {
        String[] paragraphs = textViewContent.getText().toString().split("\n\n");
        List<String> list = new ArrayList<>(Arrays.asList(paragraphs));

        if (byRelevance) {
            if (currentKeyword.isEmpty()) {
                Toast.makeText(this, "Search for a keyword first to sort by relevance", Toast.LENGTH_SHORT).show();
                return;
            }
            Collections.sort(list, (p1, p2) -> {
                int count1 = countOccurrences(p1, currentKeyword);
                int count2 = countOccurrences(p2, currentKeyword);
                return Integer.compare(count2, count1); // Higher relevance first
            });
        } else {
            Collections.sort(list);
        }

        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < list.size(); i++) {
            sb.append(list.get(i));
            if (i < list.size() - 1) sb.append("\n\n");
        }
        textViewContent.setText(sb.toString());
    }

    private int countOccurrences(String text, String keyword) {
        int count = 0;
        int index = text.toLowerCase().indexOf(keyword.toLowerCase());
        while (index != -1) {
            count++;
            index = text.toLowerCase().indexOf(keyword.toLowerCase(), index + 1);
        }
        return count;
    }

    interface InputCallback {
        void onInputReceived(String input);
    }
}
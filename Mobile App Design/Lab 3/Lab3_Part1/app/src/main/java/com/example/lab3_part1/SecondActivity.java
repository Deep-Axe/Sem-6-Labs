package com.example.lab3_part1;

import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import com.google.android.material.tabs.TabLayout;

public class SecondActivity extends AppCompatActivity {

    String[] songs = {"Song A", "Song B", "Song C", "Song D", "Song E", "Song F"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);

        TabLayout tabLayout = findViewById(R.id.tabLayout);
        TextView tabText = findViewById(R.id.tabText);
        ListView songListView = findViewById(R.id.songListView);

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, songs);
        songListView.setAdapter(adapter);

        tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                if (tab.getPosition() == 2) { // Songs tab
                    tabText.setVisibility(View.GONE);
                    songListView.setVisibility(View.VISIBLE);
                } else {
                    tabText.setVisibility(View.VISIBLE);
                    songListView.setVisibility(View.GONE);
                    tabText.setText("This is the " + tab.getText() + " tab");
                }
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {}

            @Override
            public void onTabReselected(TabLayout.Tab tab) {}
        });

        Button btnBack = findViewById(R.id.btnBack);
        btnBack.setOnClickListener(v -> finish());
    }
}

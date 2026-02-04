package com.example.lab3_part1;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ListView;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    String[] countries = {"American Samoa", "El Salvador", "Saint Helena", "Saint Kitts and Nevis", "Saint Lucia", "Saint Pierre and Miquelon", "Samoa", "San Marino", "Saudi Arabia"};
    int[] icons = {android.R.drawable.ic_menu_gallery, android.R.drawable.ic_menu_camera, android.R.drawable.ic_menu_call, 
                   android.R.drawable.ic_menu_send, android.R.drawable.ic_menu_add, android.R.drawable.ic_menu_delete,
                   android.R.drawable.ic_menu_edit, android.R.drawable.ic_menu_search, android.R.drawable.ic_menu_info_details};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ListView listView = findViewById(R.id.listView);
        ArrayAdapter<String> listAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, countries);
        listView.setAdapter(listAdapter);

        GridView gridView = findViewById(R.id.gridView);
        gridView.setAdapter(new ImageAdapter());

        Button btnNext = findViewById(R.id.btnNext);
        btnNext.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, SecondActivity.class);
            startActivity(intent);
        });
    }

    class ImageAdapter extends BaseAdapter {
        @Override
        public int getCount() { return icons.length; }
        @Override
        public Object getItem(int position) { return null; }
        @Override
        public long getItemId(int position) { return 0; }
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ImageView imageView;
            if (convertView == null) {
                imageView = new ImageView(MainActivity.this);
                imageView.setLayoutParams(new ViewGroup.LayoutParams(150, 150));
                imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
                imageView.setPadding(8, 8, 8, 8);
            } else {
                imageView = (ImageView) convertView;
            }
            imageView.setImageResource(icons[position]);
            return imageView;
        }
    }
}

package com.example.lab8q3;

import android.database.Cursor;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private EditText etMovieName, etMovieYear, etMovieRating;
    private Button btnSave;
    private ListView lvMovies;
    private TextView tvDetailName, tvDetailYear, tvDetailRating;
    private MovieDatabaseHelper dbHelper;
    private ArrayList<String> movieNames;
    private ArrayList<Long> movieIds;
    private ArrayAdapter<String> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        dbHelper = new MovieDatabaseHelper(this);

        etMovieName = findViewById(R.id.et_movie_name);
        etMovieYear = findViewById(R.id.et_movie_year);
        etMovieRating = findViewById(R.id.et_movie_rating);
        btnSave = findViewById(R.id.btn_save);
        lvMovies = findViewById(R.id.lv_movies);
        tvDetailName = findViewById(R.id.tv_detail_name);
        tvDetailYear = findViewById(R.id.tv_detail_year);
        tvDetailRating = findViewById(R.id.tv_detail_rating);

        movieNames = new ArrayList<>();
        movieIds = new ArrayList<>();
        adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, movieNames);
        lvMovies.setAdapter(adapter);

        loadMovies();

        btnSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String name = etMovieName.getText().toString().trim();
                String yearStr = etMovieYear.getText().toString().trim();
                String ratingStr = etMovieRating.getText().toString().trim();

                if (name.isEmpty() || yearStr.isEmpty() || ratingStr.isEmpty()) {
                    Toast.makeText(MainActivity.this, "Please fill all fields", Toast.LENGTH_SHORT).show();
                    return;
                }

                int year = Integer.parseInt(yearStr);
                int rating = Integer.parseInt(ratingStr);

                if (rating < 1 || rating > 5) {
                    Toast.makeText(MainActivity.this, "Rating should be between 1 and 5", Toast.LENGTH_SHORT).show();
                    return;
                }

                long id = dbHelper.insertMovie(name, year, rating);
                if (id != -1) {
                    Toast.makeText(MainActivity.this, "Review Saved", Toast.LENGTH_SHORT).show();
                    etMovieName.setText("");
                    etMovieYear.setText("");
                    etMovieRating.setText("");
                    loadMovies();
                } else {
                    Toast.makeText(MainActivity.this, "Error saving review", Toast.LENGTH_SHORT).show();
                }
            }
        });

        lvMovies.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                long movieId = movieIds.get(position);
                displayMovieDetails(movieId);
            }
        });
    }

    private void loadMovies() {
        movieNames.clear();
        movieIds.clear();
        Cursor cursor = dbHelper.getAllMovies();
        if (cursor != null && cursor.moveToFirst()) {
            do {
                long id = cursor.getLong(cursor.getColumnIndexOrThrow(MovieDatabaseHelper.COLUMN_ID));
                String name = cursor.getString(cursor.getColumnIndexOrThrow(MovieDatabaseHelper.COLUMN_NAME));
                movieNames.add(name);
                movieIds.add(id);
            } while (cursor.moveToNext());
            cursor.close();
        }
        adapter.notifyDataSetChanged();
    }

    private void displayMovieDetails(long id) {
        Cursor cursor = dbHelper.getMovieDetails(id);
        if (cursor != null && cursor.moveToFirst()) {
            String name = cursor.getString(cursor.getColumnIndexOrThrow(MovieDatabaseHelper.COLUMN_NAME));
            int year = cursor.getInt(cursor.getColumnIndexOrThrow(MovieDatabaseHelper.COLUMN_YEAR));
            int rating = cursor.getInt(cursor.getColumnIndexOrThrow(MovieDatabaseHelper.COLUMN_RATING));

            tvDetailName.setText(name);
            tvDetailYear.setText(String.valueOf(year));
            tvDetailRating.setText(String.valueOf(rating));
            cursor.close();
        }
    }
}

package com.example.lab8q1;

import android.app.DatePickerDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private EditText etTaskName, etDueDate;
    private Spinner spinnerPriority;
    private Button btnSave;
    private ListView listViewTasks;
    private TaskAdapter adapter;
    private List<Task> taskList;
    private SharedPreferences sharedPreferences;
    private int editingIndex = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        etTaskName = findViewById(R.id.etTaskName);
        etDueDate = findViewById(R.id.etDueDate);
        spinnerPriority = findViewById(R.id.spinnerPriority);
        btnSave = findViewById(R.id.btnSave);
        listViewTasks = findViewById(R.id.listViewTasks);

        sharedPreferences = getSharedPreferences("TaskPrefs", Context.MODE_PRIVATE);
        taskList = loadTasks();
        adapter = new TaskAdapter(this, taskList);
        listViewTasks.setAdapter(adapter);

        etDueDate.setOnClickListener(v -> showDatePicker());

        btnSave.setOnClickListener(v -> saveTask());

        listViewTasks.setOnItemClickListener((parent, view, position, id) -> editTask(position));

        listViewTasks.setOnItemLongClickListener((parent, view, position, id) -> {
            showDeleteDialog(position);
            return true;
        });
    }

    private void showDatePicker() {
        final Calendar c = Calendar.getInstance();
        int year = c.get(Calendar.YEAR);
        int month = c.get(Calendar.MONTH);
        int day = c.get(Calendar.DAY_OF_MONTH);

        DatePickerDialog datePickerDialog = new DatePickerDialog(this,
                (view, year1, monthOfYear, dayOfMonth) -> etDueDate.setText(year1 + "-" + (monthOfYear + 1) + "-" + dayOfMonth),
                year, month, day);
        datePickerDialog.show();
    }

    private void saveTask() {
        String name = etTaskName.getText().toString().trim();
        String date = etDueDate.getText().toString().trim();
        String priority = spinnerPriority.getSelectedItem().toString();

        if (name.isEmpty() || date.isEmpty()) {
            Toast.makeText(this, "Please fill all fields", Toast.LENGTH_SHORT).show();
            return;
        }

        Task task = new Task(name, date, priority);
        if (editingIndex == -1) {
            taskList.add(task);
        } else {
            taskList.set(editingIndex, task);
            editingIndex = -1;
            btnSave.setText("Save Task");
        }

        saveTasksToPrefs();
        adapter.notifyDataSetChanged();
        clearFields();
    }

    private void editTask(int position) {
        Task task = taskList.get(position);
        etTaskName.setText(task.getName());
        etDueDate.setText(task.getDueDate());
        
        for (int i = 0; i < spinnerPriority.getCount(); i++) {
            if (spinnerPriority.getItemAtPosition(i).toString().equalsIgnoreCase(task.getPriority())) {
                spinnerPriority.setSelection(i);
                break;
            }
        }
        
        editingIndex = position;
        btnSave.setText("Update Task");
    }

    private void showDeleteDialog(int position) {
        new AlertDialog.Builder(this)
                .setTitle("Delete Task")
                .setMessage("Are you sure you want to delete this task?")
                .setPositiveButton("Yes", (dialog, which) -> {
                    taskList.remove(position);
                    saveTasksToPrefs();
                    adapter.notifyDataSetChanged();
                    if (editingIndex == position) {
                        editingIndex = -1;
                        btnSave.setText("Save Task");
                        clearFields();
                    }
                })
                .setNegativeButton("No", null)
                .show();
    }

    private void clearFields() {
        etTaskName.setText("");
        etDueDate.setText("");
        spinnerPriority.setSelection(0);
    }

    private void saveTasksToPrefs() {
        JSONArray jsonArray = new JSONArray();
        for (Task task : taskList) {
            try {
                jsonArray.put(task.toJSONObject());
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        sharedPreferences.edit().putString("tasks", jsonArray.toString()).apply();
    }

    private List<Task> loadTasks() {
        List<Task> tasks = new ArrayList<>();
        String tasksJson = sharedPreferences.getString("tasks", null);
        if (tasksJson != null) {
            try {
                JSONArray jsonArray = new JSONArray(tasksJson);
                for (int i = 0; i < jsonArray.length(); i++) {
                    tasks.add(Task.fromJSONObject(jsonArray.getJSONObject(i)));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        return tasks;
    }
}
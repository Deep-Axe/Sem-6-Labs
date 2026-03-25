package com.example.lab8q1;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.List;

public class TaskAdapter extends ArrayAdapter<Task> {
    public TaskAdapter(Context context, List<Task> tasks) {
        super(context, 0, tasks);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Task task = getItem(position);
        if (convertView == null) {
            convertView = LayoutInflater.from(getContext()).inflate(R.layout.item_task, parent, false);
        }
        TextView tvName = convertView.findViewById(R.id.tvTaskName);
        TextView tvDetails = convertView.findViewById(R.id.tvTaskDetails);

        tvName.setText(task.getName());
        tvDetails.setText("Due: " + task.getDueDate() + " | Priority: " + task.getPriority());

        return convertView;
    }
}
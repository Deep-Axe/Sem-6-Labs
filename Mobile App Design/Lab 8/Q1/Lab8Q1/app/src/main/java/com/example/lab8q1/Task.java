package com.example.lab8q1;

import org.json.JSONException;
import org.json.JSONObject;

public class Task {
    private String name;
    private String dueDate;
    private String priority;

    public Task(String name, String dueDate, String priority) {
        this.name = name;
        this.dueDate = dueDate;
        this.priority = priority;
    }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public String getDueDate() { return dueDate; }
    public void setDueDate(String dueDate) { this.dueDate = dueDate; }
    public String getPriority() { return priority; }
    public void setPriority(String priority) { this.priority = priority; }

    public JSONObject toJSONObject() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put("name", name);
        obj.put("dueDate", dueDate);
        obj.put("priority", priority);
        return obj;
    }

    public static Task fromJSONObject(JSONObject obj) throws JSONException {
        return new Task(
                obj.getString("name"),
                obj.getString("dueDate"),
                obj.getString("priority")
        );
    }
}
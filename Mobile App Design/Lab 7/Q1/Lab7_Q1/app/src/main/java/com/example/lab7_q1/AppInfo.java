package com.example.lab7_q1;

import android.graphics.drawable.Drawable;

public class AppInfo {
    private String name;
    private String packageName;
    private Drawable icon;
    private boolean isSystemApp;
    private String version;

    public AppInfo(String name, String packageName, Drawable icon, boolean isSystemApp, String version) {
        this.name = name;
        this.packageName = packageName;
        this.icon = icon;
        this.isSystemApp = isSystemApp;
        this.version = version;
    }

    public String getName() { return name; }
    public String getPackageName() { return packageName; }
    public Drawable getIcon() { return icon; }
    public boolean isSystemApp() { return isSystemApp; }
    public String getVersion() { return version; }
}

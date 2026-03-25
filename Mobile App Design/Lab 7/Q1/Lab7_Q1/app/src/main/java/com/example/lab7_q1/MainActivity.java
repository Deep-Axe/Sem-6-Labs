package com.example.lab7_q1;

import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private RecyclerView recyclerView;
    private AppAdapter adapter;
    private List<AppInfo> appList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        recyclerView = findViewById(R.id.recyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        loadInstalledApps();

        adapter = new AppAdapter(appList, this::showAppOptions);
        recyclerView.setAdapter(adapter);
    }

    private void loadInstalledApps() {
        PackageManager pm = getPackageManager();
        List<ApplicationInfo> packages = pm.getInstalledApplications(PackageManager.GET_META_DATA);

        appList.clear();
        for (ApplicationInfo appInfo : packages) {
            String name = pm.getApplicationLabel(appInfo).toString();
            String packageName = appInfo.packageName;
            Drawable icon = pm.getApplicationIcon(appInfo);
            boolean isSystemApp = (appInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0;
            String version = "Unknown";
            try {
                PackageInfo pInfo = pm.getPackageInfo(packageName, 0);
                version = pInfo.versionName;
            } catch (PackageManager.NameNotFoundException e) {
                // Version could not be found
            }

            appList.add(new AppInfo(name, packageName, icon, isSystemApp, version));
        }
    }

    private void showAppOptions(AppInfo app) {
        String type = app.isSystemApp() ? "System App" : "User-Installed App";
        String permissions = getSpecialPermissions(app.getPackageName());
        String version = "Version: " + app.getVersion();
        
        String storageUsage = "Size: Unknown";
        try {
            ApplicationInfo ai = getPackageManager().getApplicationInfo(app.getPackageName(), 0);
            File file = new File(ai.publicSourceDir);
            long size = file.length();
            storageUsage = "Size: " + (size / (1024 * 1024)) + " MB";
        } catch (PackageManager.NameNotFoundException e) {
            // Ignore
        }

        String message = "Type: " + type + "\n" + version + "\n" + storageUsage + "\n\n" + permissions;

        new AlertDialog.Builder(this)
                .setTitle(app.getName())
                .setMessage(message)
                .setPositiveButton("Open App", (dialog, which) -> openApp(app.getPackageName()))
                .setNegativeButton("Uninstall", (dialog, which) -> showUninstallConfirmation(app))
                .setNeutralButton("View Details", (dialog, which) -> {
                    Intent intent = new Intent(MainActivity.this, AppDetailsActivity.class);
                    intent.putExtra("packageName", app.getPackageName());
                    startActivity(intent);
                })
                .show();
    }

    private void showUninstallConfirmation(AppInfo app) {
        new AlertDialog.Builder(this)
                .setTitle("Confirm Uninstall")
                .setMessage("Are you sure you want to uninstall " + app.getName() + "?")
                .setPositiveButton("Yes", (dialog, which) -> uninstallApp(app.getPackageName()))
                .setNegativeButton("No", null)
                .show();
    }

    private String getSpecialPermissions(String packageName) {
        StringBuilder sb = new StringBuilder("Special Permissions Granted: ");
        PackageManager pm = getPackageManager();
        
        String[] permissionsToCheck = {
                android.Manifest.permission.ACCESS_FINE_LOCATION,
                android.Manifest.permission.CAMERA
        };
        
        boolean found = false;
        for (String permission : permissionsToCheck) {
            if (pm.checkPermission(permission, packageName) == PackageManager.PERMISSION_GRANTED) {
                String permissionName = permission.substring(permission.lastIndexOf(".") + 1);
                sb.append(permissionName).append(" ");
                found = true;
            }
        }

        if (!found) sb.append("None");
        return sb.toString();
    }

    private void openApp(String packageName) {
        Intent intent = getPackageManager().getLaunchIntentForPackage(packageName);
        if (intent != null) {
            startActivity(intent);
        } else {
            Toast.makeText(this, "Cannot open this app", Toast.LENGTH_SHORT).show();
        }
    }

    private void uninstallApp(String packageName) {
        Intent intent = new Intent(Intent.ACTION_DELETE);
        intent.setData(Uri.parse("package:" + packageName));
        startActivity(intent);
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Refresh the list in case an app was uninstalled while away
        loadInstalledApps();
        if (adapter != null) {
            adapter.notifyDataSetChanged();
        }
    }
}

package com.example.lab7_q1;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class AppDetailsActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_app_details);

        String packageName = getIntent().getStringExtra("packageName");
        if (packageName == null) {
            finish();
            return;
        }

        ImageView iconView = findViewById(R.id.detailAppIcon);
        TextView nameView = findViewById(R.id.detailAppName);
        TextView packageView = findViewById(R.id.detailPackageName);
        TextView versionView = findViewById(R.id.detailVersion);
        TextView permissionsView = findViewById(R.id.detailPermissions);
        TextView sizeView = findViewById(R.id.detailSize);

        PackageManager pm = getPackageManager();
        try {
            PackageInfo packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_PERMISSIONS);
            Drawable icon = pm.getApplicationIcon(packageName);
            String name = pm.getApplicationLabel(pm.getApplicationInfo(packageName, 0)).toString();

            iconView.setImageDrawable(icon);
            nameView.setText(name);
            packageView.setText(packageName);
            versionView.setText("Version: " + packageInfo.versionName);

            StringBuilder permissions = new StringBuilder("Permissions:\n");
            if (packageInfo.requestedPermissions != null) {
                for (String p : packageInfo.requestedPermissions) {
                    permissions.append(p).append("\n");
                }
            } else {
                permissions.append("No permissions requested.");
            }
            permissionsView.setText(permissions.toString());

            File file = new File(pm.getApplicationInfo(packageName, 0).publicSourceDir);
            long size = file.length();
            sizeView.setText("Size: " + (size / (1024 * 1024)) + " MB");

        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
    }
}

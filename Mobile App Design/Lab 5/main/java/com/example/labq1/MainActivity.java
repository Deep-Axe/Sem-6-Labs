package com.example.labq1;

import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Random;

public class MainActivity extends AppCompatActivity {

    private Spinner vehicleTypeSpinner;
    private EditText vehicleNumberET, rcNumberET;
    private LinearLayout inputLayout, resultLayout;
    private TextView displayVehicleType, displayVehicleNumber, displayRCNumber;
    private Button submitBtn, editBtn, confirmBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize views
        vehicleTypeSpinner = findViewById(R.id.vehicleTypeSpinner);
        vehicleNumberET = findViewById(R.id.vehicleNumberET);
        rcNumberET = findViewById(R.id.rcNumberET);
        inputLayout = findViewById(R.id.inputLayout);
        resultLayout = findViewById(R.id.resultLayout);
        displayVehicleType = findViewById(R.id.displayVehicleType);
        displayVehicleNumber = findViewById(R.id.displayVehicleNumber);
        displayRCNumber = findViewById(R.id.displayRCNumber);
        submitBtn = findViewById(R.id.submitBtn);
        editBtn = findViewById(R.id.editBtn);
        confirmBtn = findViewById(R.id.confirmBtn);

        // Setup Spinner
        String[] vehicleTypes = {"Car", "Bike", "Truck", "Bus", "Scooter"};
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, vehicleTypes);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        vehicleTypeSpinner.setAdapter(adapter);

        submitBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String vehicleType = vehicleTypeSpinner.getSelectedItem().toString();
                String vehicleNumber = vehicleNumberET.getText().toString().trim();
                String rcNumber = rcNumberET.getText().toString().trim();

                if (vehicleNumber.isEmpty() || rcNumber.isEmpty()) {
                    Toast.makeText(MainActivity.this, "Please fill all details", Toast.LENGTH_SHORT).show();
                    return;
                }

                // Display details
                displayVehicleType.setText("Vehicle Type: " + vehicleType);
                displayVehicleNumber.setText("Vehicle Number: " + vehicleNumber);
                displayRCNumber.setText("RC Number: " + rcNumber);

                // Switch views
                inputLayout.setVisibility(View.GONE);
                resultLayout.setVisibility(View.VISIBLE);
            }
        });

        editBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Switch back to input
                resultLayout.setVisibility(View.GONE);
                inputLayout.setVisibility(View.VISIBLE);
            }
        });

        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Generate unique serial number
                String serialNumber = "VP-" + (1000 + new Random().nextInt(9000));
                
                Toast.makeText(MainActivity.this, "Parking Allotted! Serial No: " + serialNumber, Toast.LENGTH_LONG).show();

                // Reset for next registration
                resetForm();
            }
        });
    }

    private void resetForm() {
        vehicleNumberET.setText("");
        rcNumberET.setText("");
        vehicleTypeSpinner.setSelection(0);
        resultLayout.setVisibility(View.GONE);
        inputLayout.setVisibility(View.VISIBLE);
    }
}
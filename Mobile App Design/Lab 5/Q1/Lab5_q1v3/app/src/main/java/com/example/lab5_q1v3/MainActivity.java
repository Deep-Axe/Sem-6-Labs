package com.example.lab5_q1v3;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private LinearLayout inputForm, confirmationView, preConfirmButtons;
    private Spinner vehicleTypeSpinner;
    private EditText vehicleNumberEdit, rcNumberEdit;
    private TextView displayVehicleType, displayVehicleNumber, displayRcNumber;
    private Button submitBtn, editBtn, confirmBtn, backToStartBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize Views
        inputForm = findViewById(R.id.inputForm);
        confirmationView = findViewById(R.id.confirmationView);
        preConfirmButtons = findViewById(R.id.preConfirmButtons);
        
        vehicleTypeSpinner = findViewById(R.id.vehicleTypeSpinner);
        vehicleNumberEdit = findViewById(R.id.vehicleNumberEdit);
        rcNumberEdit = findViewById(R.id.rcNumberEdit);
        
        displayVehicleType = findViewById(R.id.displayVehicleType);
        displayVehicleNumber = findViewById(R.id.displayVehicleNumber);
        displayRcNumber = findViewById(R.id.displayRcNumber);
        
        submitBtn = findViewById(R.id.submitBtn);
        editBtn = findViewById(R.id.editBtn);
        confirmBtn = findViewById(R.id.confirmBtn);
        backToStartBtn = findViewById(R.id.backToStartBtn);

        submitBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String vehicleNumber = vehicleNumberEdit.getText().toString().trim();
                String rcNumber = rcNumberEdit.getText().toString().trim();

                if (vehicleNumber.isEmpty() || rcNumber.isEmpty()) {
                    Toast.makeText(MainActivity.this, "Please enter all details", Toast.LENGTH_SHORT).show();
                    return;
                }

                // 1. Hide Keyboard when moving to next page
                hideKeyboard(v);

                // Show confirmation details
                displayVehicleType.setText("Type: " + vehicleTypeSpinner.getSelectedItem().toString());
                displayVehicleNumber.setText("Vehicle Number: " + vehicleNumber);
                displayRcNumber.setText("RC Number: " + rcNumber);

                // Switch views
                inputForm.setVisibility(View.GONE);
                confirmationView.setVisibility(View.VISIBLE);
                preConfirmButtons.setVisibility(View.VISIBLE);
                backToStartBtn.setVisibility(View.GONE);
            }
        });

        editBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Return to form to edit
                confirmationView.setVisibility(View.GONE);
                inputForm.setVisibility(View.VISIBLE);
            }
        });

        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 2. Generate unique serial number and show Toast on 2nd view
                String serialNumber = "PK-" + UUID.randomUUID().toString().substring(0, 8).toUpperCase();
                Toast.makeText(MainActivity.this, 
                    "Parking Allotted! Serial Number: " + serialNumber, 
                    Toast.LENGTH_LONG).show();

                // 3. Stay on 2nd view but hide edit/confirm and show "Back" button
                preConfirmButtons.setVisibility(View.GONE);
                backToStartBtn.setVisibility(View.VISIBLE);
            }
        });

        backToStartBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Reset form and go back to first view
                vehicleNumberEdit.setText("");
                rcNumberEdit.setText("");
                vehicleTypeSpinner.setSelection(0);
                
                confirmationView.setVisibility(View.GONE);
                inputForm.setVisibility(View.VISIBLE);
            }
        });
    }

    private void hideKeyboard(View view) {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm != null) {
            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
        }
    }
}

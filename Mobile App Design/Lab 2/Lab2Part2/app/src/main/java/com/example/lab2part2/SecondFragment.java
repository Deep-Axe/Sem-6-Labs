package com.example.lab2part2;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;

import com.example.lab2part2.databinding.FragmentSecondBinding;

public class SecondFragment extends Fragment {

    private FragmentSecondBinding binding;

    @Override
    public View onCreateView(
            @NonNull LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {

        binding = FragmentSecondBinding.inflate(inflater, container, false);
        return binding.getRoot();

    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.buttonSecond.setOnClickListener(v ->
                NavHostFragment.findNavController(SecondFragment.this)
                        .navigate(R.id.action_SecondFragment_to_FirstFragment)
        );

        binding.buttonOk.setOnClickListener(v -> {
            String url = binding.editText.getText().toString().trim();
            if (!url.isEmpty()) {
                if (!url.startsWith("http://") && !url.startsWith("https://")) {
                    url = "https://" + url;
                }
                try {
                    Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                    // Explicitly try to open in Chrome if available, otherwise use default browser
                    intent.setPackage("com.android.chrome");
                    try {
                        startActivity(intent);
                    } catch (Exception e) {
                        // Chrome not found, fall back to default browser
                        intent.setPackage(null);
                        startActivity(intent);
                    }
                } catch (Exception e) {
                    Toast.makeText(getContext(), "Invalid URL", Toast.LENGTH_SHORT).show();
                }
            } else {
                Toast.makeText(getContext(), "Please enter a URL", Toast.LENGTH_SHORT).show();
            }
        });

        binding.buttonCancel.setOnClickListener(v -> binding.editText.setText(""));
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

}
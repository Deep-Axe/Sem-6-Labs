package com.example.lab3_part2;

import android.app.AlertDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import java.util.HashMap;
import java.util.Map;

public class NewsFragment extends Fragment {

    private static final String ARG_SECTION_NAME = "section_name";
    private String sectionName;

    private String[] sportsList = {"Football", "Basketball", "Cricket", "Tennis", "Badminton"};
    private Map<String, String> sportDescriptions = new HashMap<>();

    public static NewsFragment newInstance(String sectionName) {
        NewsFragment fragment = new NewsFragment();
        Bundle args = new Bundle();
        args.putString(ARG_SECTION_NAME, sectionName);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            sectionName = getArguments().getString(ARG_SECTION_NAME);
        }

        sportDescriptions.put("Football", "A team sport played with a spherical ball between two teams of 11 players.");
        sportDescriptions.put("Basketball", "A game played between two teams of five players in which goals are scored by throwing a ball through a netted hoop.");
        sportDescriptions.put("Cricket", "A bat-and-ball game played between two teams of eleven players on a field.");
        sportDescriptions.put("Tennis", "A racket sport that can be played individually against a single opponent or between two teams of two players each.");
        sportDescriptions.put("Badminton", "A racket sport played using rackets to hit a shuttlecock across a net.");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_news, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        TextView contentText = view.findViewById(R.id.newsContentText);
        ListView listView = view.findViewById(R.id.newsSportsListView);

        if ("Sports".equals(sectionName)) {
            listView.setVisibility(View.VISIBLE);
            contentText.setVisibility(View.GONE);

            ArrayAdapter<String> adapter = new ArrayAdapter<>(requireContext(), android.R.layout.simple_list_item_1, sportsList);
            listView.setAdapter(adapter);

            listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    String sport = sportsList[position];
                    String description = sportDescriptions.get(sport);
                    showDescriptionDialog(sport, description);
                }
            });
        } else {
            listView.setVisibility(View.GONE);
            contentText.setVisibility(View.VISIBLE);
            
            String report = "";
            if ("Top Stories".equals(sectionName)) {
                report = "January 15, 2024: Major breakthrough in renewable energy technology announced today. Researchers have developed a more efficient solar cell that could reduce costs by 30%. This news has sparked interest globally as countries look to meet climate targets.";
            } else if ("Entertainment".equals(sectionName)) {
                report = "January 20, 2024: The highly anticipated sequel to the blockbuster 'Starry Night' premiered in Los Angeles last night. Fans gathered in thousands to see their favorite stars on the red carpet. Early reviews suggest it might be the biggest hit of the year.";
            }
            contentText.setText(report);
        }
    }

    private void showDescriptionDialog(String title, String message) {
        new AlertDialog.Builder(requireContext())
                .setTitle(title)
                .setMessage(message)
                .setPositiveButton("Close", null)
                .show();
    }
}
package com.example.prgcheck;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {
    private static final String DATABASE_NAME = "Shop.db";
    private static final int DATABASE_VERSION = 1;

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onConfigure(SQLiteDatabase db) {
        super.onConfigure(db);
        // Enabling Write-Ahead Logging allows for better live updates in App Inspection
        db.enableWriteAheadLogging();
        db.setForeignKeyConstraintsEnabled(true);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE Customers (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, email TEXT UNIQUE)");
        db.execSQL("CREATE TABLE Menu (id INTEGER PRIMARY KEY AUTOINCREMENT, item_name TEXT NOT NULL, price REAL NOT NULL)");
        db.execSQL("CREATE TABLE Transactions (id INTEGER PRIMARY KEY AUTOINCREMENT, customer_id INTEGER, menu_id INTEGER, quantity INTEGER, FOREIGN KEY(customer_id) REFERENCES Customers(id), FOREIGN KEY(menu_id) REFERENCES Menu(id))");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS Transactions");
        db.execSQL("DROP TABLE IF EXISTS Menu");
        db.execSQL("DROP TABLE IF EXISTS Customers");
        onCreate(db);
    }
}

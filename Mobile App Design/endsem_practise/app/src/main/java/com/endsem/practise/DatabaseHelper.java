package com.endsem.practise;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "ConferenceDB";
    private static final int DATABASE_VERSION = 4; // Incremented version to 4

    // Users Table
    public static final String TABLE_USERS = "users";
    public static final String COL_USER_ID = "id";
    public static final String COL_USERNAME = "username";
    public static final String COL_PASSWORD = "password";

    // Papers Table
    public static final String TABLE_NAME = "papers";
    public static final String COL_ID = "id";
    public static final String COL_AUTHOR = "author_name";
    public static final String COL_EMAIL = "email";
    public static final String COL_TITLE = "paper_title";
    public static final String COL_TYPE = "presentation_type";
    public static final String COL_DOMAIN = "domain";
    public static final String COL_DEADLINE = "deadline";
    public static final String COL_SLOT = "slot";
    public static final String COL_TRACK = "track";
    public static final String COL_KEYWORDS = "keywords";
    public static final String COL_REVIEWER = "reviewer";
    public static final String COL_CONFIDENCE = "confidence_score";
    public static final String COL_CO_AUTHORS_DATA = "co_authors_data";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String createUsersTable = "CREATE TABLE " + TABLE_USERS + " (" +
                COL_USER_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                COL_USERNAME + " TEXT UNIQUE, " +
                COL_PASSWORD + " TEXT)";
        db.execSQL(createUsersTable);

        String createPapersTable = "CREATE TABLE " + TABLE_NAME + " (" +
                COL_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                COL_AUTHOR + " TEXT, " +
                COL_EMAIL + " TEXT, " +
                COL_TITLE + " TEXT UNIQUE, " +
                COL_TYPE + " TEXT, " +
                COL_DOMAIN + " TEXT, " +
                COL_DEADLINE + " TEXT, " +
                COL_SLOT + " TEXT, " +
                COL_TRACK + " TEXT, " +
                COL_KEYWORDS + " TEXT, " +
                COL_REVIEWER + " TEXT, " +
                COL_CONFIDENCE + " INTEGER, " +
                COL_CO_AUTHORS_DATA + " TEXT)";
        db.execSQL(createPapersTable);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        if (oldVersion < 4) {
            String createUsersTable = "CREATE TABLE " + TABLE_USERS + " (" +
                    COL_USER_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                    COL_USERNAME + " TEXT UNIQUE, " +
                    COL_PASSWORD + " TEXT)";
            db.execSQL(createUsersTable);
        }
    }

    // User Methods
    public long signupUser(String username, String password) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COL_USERNAME, username);
        values.put(COL_PASSWORD, password);
        return db.insert(TABLE_USERS, null, values);
    }

    public boolean loginUser(String username, String password) {
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.rawQuery("SELECT * FROM " + TABLE_USERS + " WHERE " + COL_USERNAME + "=? AND " + COL_PASSWORD + "=?", new String[]{username, password});
        boolean exists = cursor.getCount() > 0;
        cursor.close();
        return exists;
    }

    // Paper Methods
    public long insertPaper(String author, String email, String title, String type, String domain, String coAuthorsData) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COL_AUTHOR, author);
        values.put(COL_EMAIL, email);
        values.put(COL_TITLE, title);
        values.put(COL_TYPE, type);
        values.put(COL_DOMAIN, domain);
        values.put(COL_CO_AUTHORS_DATA, coAuthorsData);
        return db.insert(TABLE_NAME, null, values);
    }

    public int updateScheduling(String title, String deadline, String slot, String track, String keywords) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COL_DEADLINE, deadline);
        values.put(COL_SLOT, slot);
        values.put(COL_TRACK, track);
        values.put(COL_KEYWORDS, keywords);
        return db.update(TABLE_NAME, values, COL_TITLE + "=?", new String[]{title});
    }

    public int updateReviewer(String title, String reviewer, int score) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COL_REVIEWER, reviewer);
        values.put(COL_CONFIDENCE, score);
        return db.update(TABLE_NAME, values, COL_TITLE + "=?", new String[]{title});
    }

    public Cursor getPaperByTitle(String title) {
        SQLiteDatabase db = this.getReadableDatabase();
        return db.rawQuery("SELECT * FROM " + TABLE_NAME + " WHERE " + COL_TITLE + "=?", new String[]{title});
    }

    public Cursor getPapersInDomain(String domain) {
        SQLiteDatabase db = this.getReadableDatabase();
        return db.rawQuery("SELECT * FROM " + TABLE_NAME + " WHERE " + COL_DOMAIN + "=?", new String[]{domain});
    }

    public Cursor getStats() {
        SQLiteDatabase db = this.getReadableDatabase();
        return db.rawQuery("SELECT COUNT(*), " + COL_DOMAIN + " FROM " + TABLE_NAME + " GROUP BY " + COL_DOMAIN, null);
    }

    public Cursor getAllPapers() {
        SQLiteDatabase db = this.getReadableDatabase();
        return db.rawQuery("SELECT * FROM " + TABLE_NAME, null);
    }

    public int deletePaper(String title) {
        SQLiteDatabase db = this.getWritableDatabase();
        return db.delete(TABLE_NAME, COL_TITLE + "=?", new String[]{title});
    }

    public void clearAll() {
        SQLiteDatabase db = this.getWritableDatabase();
        db.execSQL("DELETE FROM " + TABLE_NAME);
        db.execSQL("DELETE FROM " + TABLE_USERS);
    }
}

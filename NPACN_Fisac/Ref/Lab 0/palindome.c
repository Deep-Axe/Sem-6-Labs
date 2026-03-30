#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <wctype.h>

int is_unicode_palindrome(const wchar_t *str) {
    int left = 0;
    int right = wcslen(str) - 1;

    while (left < right) {
        while (left < right && (str[left] < 32)) left++;
        while (left < right && (str[right] < 32)) right--;

        if (iswlower(str[left]) != iswlower(str[right])) {
             if (str[left] != str[right]) return 0;
        }
        
        if (str[left] != str[right]) return 0;
        
        left++;
        right--;
    }
    return 1;
}

int main() {
    setlocale(LC_ALL, ""); // Support local Unicode settings

    wchar_t buffer[256];
    wchar_t clean_buffer[256];

    wprintf(L"Palindrome Check\n");
    wprintf(L"Type 'exit' to quit.\n\n");

    while (1) {
        wprintf(L"Enter string: ");
        if (fgetws(buffer, 256, stdin) == NULL) break;
        size_t len = wcslen(buffer);
        if (len > 0 && buffer[len - 1] == L'\n') buffer[len - 1] = L'\0';
        if (wcscmp(buffer, L"exit") == 0) break;

        // 3. Filter out ANSI escape sequences and control codes
        int j = 0;
        for (int i = 0; buffer[i] != L'\0'; i++) {
            if (buffer[i] >= 32) { 
                clean_buffer[j++] = buffer[i];
            }
        }
        clean_buffer[j] = L'\0';

        if (is_unicode_palindrome(clean_buffer)) {
            wprintf(L"Result: \"%ls\" is a palindrome.\n\n", clean_buffer);
        } else {
            wprintf(L"Result: \"%ls\" is NOT a palindrome.\n\n", clean_buffer);
        }
    }
    return 0;
}

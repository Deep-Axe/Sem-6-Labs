#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

int compare_unicode(const void *a, const void *b) {
    return wcscoll(*(const wchar_t **)a, *(const wchar_t **)b);
}

wchar_t* get_wstring() {
    size_t size = 16, len = 0;
    wchar_t *str = malloc(size * sizeof(wchar_t));
    wint_t ch;

    while ((ch = getwchar()) != L'\n' && ch != WEOF) {
        if (len + 1 >= size) {
            size *= 2;
            str = realloc(str, size * sizeof(wchar_t));
        }
        str[len++] = (wchar_t)ch;
    }
    str[len] = L'\0';
    return str;
}

int main() {
    setlocale(LC_ALL, "");

    wchar_t **list = NULL;
    int count = 0;
    wprintf(L"");
    wprintf(L"Enter strings: \n");

    while (1) {
        wprintf(L"Add string [%d]: ", count + 1);
        wchar_t *input = get_wstring();
        
        if (wcscmp(input, L"done") == 0) {
            free(input);
            break;
        }

        list = realloc(list, (count + 1) * sizeof(wchar_t *));
        list[count++] = input;
    }

    if (count > 0) {
        qsort(list, count, sizeof(wchar_t *), compare_unicode);
        wprintf(L"\nSorted Unicode List:\n");
        for (int i = 0; i < count; i++) wprintf(L"%d. %ls\n", i + 1, list[i]);
    }

    wprintf(L"\nEnter substrings to search (type 'exit' to quit):\n");
    while (1) {
        wprintf(L"Search for: ");
        wchar_t *query = get_wstring();

        if (wcscmp(query, L"exit") == 0) {
            free(query);
            break;
        }

        int found = 0;
        for (int i = 0; i < count; i++) {
            if (wcsstr(list[i], query)) {
                wprintf(L" - Found in: %ls\n", list[i]);
                found = 1;
            }
        }
        if (!found) wprintf(L" - No matches found.\n");
        free(query);
    }

    for (int i = 0; i < count; i++) free(list[i]);
    free(list);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *extract_substring(char *s, int i, int j) {
    if (s == NULL || i < 0) {
        return NULL; // Handle invalid input
    }

    int len = strlen(s);
    if (i >= len) {
        return NULL; // i is outside the string
    }

    if (j >= len) {
        j = len - 1; // Adjust j to the end of the string
    }

    if (i > j) {
        // If i is greater than j, swap them to reverse the substring
        int temp = i;
        i = j;
        j = temp;
    }

    int sub_len = j - i + 1;
    char *substring = (char *)malloc(sub_len + 1); // +1 for the null-terminator

    if (substring == NULL) {
        return NULL; // Memory allocation failed
    }

    strncpy(substring, s + i, sub_len); // Copy the substring from s to the new memory
    substring[sub_len] = '\0'; // Null-terminate the new string

    return substring;
}


int main() {
    char *original = "Hello, World!";
    int i = 7; // Start index
    int j = 0; // End index

    char *result = extract_substring(original, i, j);

    if (result != NULL) {
        printf("Extracted Substring: %s\n", result);
        free(result); // Remember to free the allocated memory
    } else {
        printf("Failed to extract substring.\n");
    }

    return 0;
}

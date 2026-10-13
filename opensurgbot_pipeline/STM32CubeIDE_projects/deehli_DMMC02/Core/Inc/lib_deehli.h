/*
 * lib_deehli.h
 *	Library to define useful functions for the demo
 *  Created on: June 18, 2025
 *      Author: LeoG
 */

#ifndef INC_LIB_DEEHLI_H_
#define INC_LIB_DEEHLI_H_

#include <string.h>

/*
 * Convert given string to a double
 */
int chars_to_double(char *in_string, double *out_value) {
    if (in_string == NULL || out_value == NULL) {return -1;} // Invalid input
    char *end;
    double value = strtod(in_string, &end);

    if (*end == '\0') { // Check if the conversion was successful
        *out_value = value;
        return 0; // Success
    } else {
        return -2; // Conversion error
    }
}

/*
 * Split given string
 */
int split_string(const char *in_string, char*** out_strings, int *out_count, const char *opt_separator) {
    if (in_string == NULL || out_count == NULL || out_strings == NULL) {
        return -1; // Invalid input
    }

    const char* separator = opt_separator ? opt_separator : ",";

    // First, count the number of tokens
    int token_count = 0;
    char* input_copy = strdup(in_string); // Create a copy of the input string
    char* token = strtok(input_copy, separator);
    while (token != NULL) {
        token_count++;
        token = strtok(NULL, separator);
    }
    free(input_copy); // Free the copied string

    // Allocate memory for the array of strings
    *out_strings = (char**)malloc(token_count * sizeof(char*));
    if (*out_strings == NULL) {
        return -2; // Memory allocation failed
    }

    // Tokenize the input string and store the results
    input_copy = strdup(in_string); // Create another copy of the input string
    token = strtok(input_copy, separator);
    for (int i = 0; i < token_count; i++) {
        (*out_strings)[i] = strdup(token); // Allocate memory for each token and copy it
        token = strtok(NULL, separator);
    }
    free(input_copy); // Free the copied string

    *out_count = token_count; // Set the count of tokens
    return 0;
}


#endif /* INC_LIB_DEEHLI_H_ */

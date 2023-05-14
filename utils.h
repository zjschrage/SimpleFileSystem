#pragma once

/**
 * @brief Returns min between a and b
 * 
 * @param a first value
 * @param b second valie
 * @return int minimum of a and b 
 */
int min(int a, int b);

/**
 * @brief Returns max between a and b
 * 
 * @param a first value
 * @param b second valie
 * @return int maximum of a and b 
 */
int max(int a, int b);

/**
 * @brief Takes the ceiling function of a/b.  
 * 
 * @param a dividend
 * @param b divisor
 * @return int ceil(a/b)
 */
int ceil_div(int a, int b);

/**
 * @brief Compares two strings by character
 * 
 * @param a first string
 * @param b second string
 * @return int true (1) or false (0)
 */
int match_string(char* a, char* b);

/**
 * @brief Checks to see if string a matches string b up to a delimeter
 * 
 * @param a string to be matched
 * @param b string with prefix
 * @param delim prefix delimeter
 * @return int true (1) or false (0)
 */
int prefix_matching(char* a, char* b, char delim);

/**
 * @brief Truncates a string by returning index of first instance past a delimeter
 * 
 * @param a string
 * @param delim delimeter
 * @return int index of truncated start
 */
int truncate_prefix(char* a, char delim);

/**
 * @brief Prints first n elements of a char array (debugging tool)
 * 
 * @param arr char array
 * @param n number of elements to print
 */
void print_char_arr(char* arr, int n);
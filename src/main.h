#pragma once

/**
 * @brief The entry point of the program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return An integer representing the exit status of the program.
 */
int main(int argc, char *argv[]);

/**
 * Checks the architecture compatibility of the system.
 *
 * This function checks if the architecture meets the requirements for the
 * program to run. It verifies that the size of a byte is 8 bits and the size of
 * an int is 32 bits.
 *
 * @return 0 if the architecture is supported, 1 otherwise.
 */
int check_architecture();

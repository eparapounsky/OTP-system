#include <stdio.h> // for printf
#include <stdlib.h> // for atoi
#include <time.h> // for srand

// macros
#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
#define CHARACTERS_LENGTH 27

/**
 * Generates a random key from a predefined set of characters.
 * Length of the key is specified by user from command line.
 * @param argument_count: int, the number of command line arguments
 * @param argument_array: array, the command line arguments entered (first is the program name,
 * second is the length of the key)
 * Adapted from Stack Overflow: https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
 */
int main(int argument_count, char* argument_array[]) {
	if (argument_count < 2) {
		fprintf(stderr, "Please specify the length of the key.\n");
		exit(1);
	} else if (argument_count > 2) {
		fprintf(stderr, "Please ONLY specify the length of the key.\n");
		exit(1);
	}

	int key_length = atoi(argument_array[1]); // convert cla to integer
	srand(time(NULL)); // seed rand to generate different keys
	char* key = (char*)calloc((key_length + 1), sizeof(char));

	if (!key) {
		fprintf(stderr, "Memory could not be allocated for key.\n");
		exit(1);
	}

	// generate key
	for (int i = 0; i < key_length; i++) {
		key[i] = ALLOWED_CHARACTERS[rand() % CHARACTERS_LENGTH];
	}

	printf("%s\n", key);
	free(key);
	return 0;
}

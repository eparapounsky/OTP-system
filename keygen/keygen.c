#include <stdio.h>	// for printf
#include <stdlib.h> // for atoi
#include <time.h>	// for srand
#include <errno.h>	// for errno
#include <limits.h> // for INT_MAX

// macros
#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
#define CHARACTERS_LENGTH (sizeof(ALLOWED_CHARACTERS) - 1)
#define MAX_KEY_LENGTH 100000 // maximum reasonable key length

/**
 * Generates a random key from a predefined set of characters.
 * Length of the key is specified by user from command line.
 * @param argument_count: int, the number of command line arguments
 * @param argument_array: array, the command line arguments entered (first is the program name,
 * second is the length of the key)
 */
int main(int argument_count, char *argument_array[])
{
	if (argument_count < 2)
	{
		fprintf(stderr, "Please specify the length of the key.\n");
		exit(1);
	}
	else if (argument_count > 2)
	{
		fprintf(stderr, "Please ONLY specify the length of the key.\n");
		exit(1);
	}

	// validate and convert command line argument to integer
	char *endptr;
	errno = 0;
	long key_length_long = strtol(argument_array[1], &endptr, 10);

	// check for conversion errors
	if (errno != 0)
	{
		fprintf(stderr, "ERROR: Invalid number format for key length\n");
		exit(2);
	}

	// check if entire string was converted
	if (endptr == argument_array[1] || *endptr != '\0')
	{
		fprintf(stderr, "ERROR: Key length must be a valid integer\n");
		exit(2);
	}

	// check for valid range
	if (key_length_long <= 0)
	{
		fprintf(stderr, "ERROR: Key length must be a positive integer\n");
		exit(2);
	}

	if (key_length_long > MAX_KEY_LENGTH)
	{
		fprintf(stderr, "ERROR: Key length too large (maximum %d)\n", MAX_KEY_LENGTH);
		exit(2);
	}

	// check if value fits in int
	if (key_length_long > INT_MAX)
	{
		fprintf(stderr, "ERROR: Key length exceeds maximum integer value\n");
		exit(2);
	}

	int key_length = (int)key_length_long;

	// seed random number generator
	srand(time(NULL));

	// allocate memory for key with bounds checking
	char *key = calloc((size_t)key_length + 1, sizeof(char));
	if (!key)
	{
		fprintf(stderr, "ERROR: Memory could not be allocated for key of length %d\n", key_length);
		exit(3);
	}

	// generate key
	for (int i = 0; i < key_length; i++)
	{
		int random_index = rand() % CHARACTERS_LENGTH;
		key[i] = ALLOWED_CHARACTERS[random_index];
	}

	key[key_length] = '\0'; // ensure null termination

	printf("%s\n", key);
	free(key);
	return 0;
}

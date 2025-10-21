#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> // send(), recv()
#include <netdb.h>		// gethostbyname()
#include <stdbool.h>

// macros
#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "

// function prototypes
void setup_client_address_struct(struct sockaddr_in *socket_address, int port_number, char *host_name);
char *read_file(char *file_path, int *file_size);
void send_message(int connection_socket_fd, char *message, int message_size);
char *receive_message(int connection_socket_fd);

/**
 * Sets up a client socket address struct to connect to the server.
 * @param socket_address: pointer to sockaddr_in structure
 * @param port_number: int, port number on which the server is listening
 * @param host_name: string, host name of the server to connect to
 */
void setup_client_address_struct(struct sockaddr_in *socket_address, int port_number, char *host_name)
{
	memset((char *)socket_address, '\0', sizeof(*socket_address)); // clear out the socket_address struct
	socket_address->sin_family = AF_INET;						   // the address should be network capable
	socket_address->sin_port = htons(port_number);				   // bind to port number after converting to network byte order

	// get the DNS entry for this host name
	struct hostent *host_info = gethostbyname(host_name);
	if (host_info == NULL)
	{
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(1);
	}

	// copy the first IP address from the DNS entry to sin_addr.s_addr (IP address field)
	memcpy((char *)&socket_address->sin_addr.s_addr, host_info->h_addr_list[0], host_info->h_length);
}

/**
 * Reads the contents of a file into a string.
 * @param file_path: path to the file
 * @param file_size: pointer to an int where the file size will be stored
 * @return file_contents: string containing the file's contents
 */
char *read_file(char *file_path, int *file_size)
{
	// open the file
	FILE *file = fopen(file_path, "r");
	if (!file)
	{
		fprintf(stderr, "CLIENT: ERROR- could not open file %s\n", file_path);
		exit(1);
	}

	// get file size
	fseek(file, 0, SEEK_END); // move indicator to end of file
	*file_size = ftell(file); // get position of indicator
	fseek(file, 0, SEEK_SET); // move indicator to beginning of file

	// allocate memory for file contents
	char *file_contents = calloc(*file_size + 1, sizeof(char)); // +1 for null terminator
	if (!file_contents)
	{
		fclose(file);
		fprintf(stderr, "CLIENT: ERROR- could not allocate memory for file %s\n", file_path);
		exit(1);
	}

	// read the file
	size_t total_bytes_read = 0;
	while (total_bytes_read < *file_size)
	{
		// move pointer forward in file, and only read remaining file
		size_t bytes_read = fread(file_contents + total_bytes_read, sizeof(char), *file_size - total_bytes_read, file);
		// error handling?
		total_bytes_read += bytes_read;
	}

	file_contents[strlen(file_contents) - 1] = '\0'; // strip off newline

	// check file for bad characters
	int file_length = strlen(file_contents);
	for (int i = 0; i < file_length; i++)
	{
		char *character_found = strchr(ALLOWED_CHARACTERS, file_contents[i]);
		if (!character_found)
		{
			fclose(file);
			fprintf(stderr, "error: input contains bad characters");
			exit(1);
		}
	}
	fclose(file);
	return file_contents;
}

/**
 * Sends a message from the client side over the given socket.
 * Sends the message size first, so the recipient can dynamically allocate memory, then sends the message.
 * @param connection_socket_fd: int, the file descriptor for the connection socket
 * @param message: string, the message to be sent
 * @param message_size: int, the size of the message in bytes
 */
void send_message(int connection_socket_fd, char *message, int message_size)
{
	// send message size
	int converted_size = htonl(message_size); // convert to network byte order
	if (send(connection_socket_fd, &converted_size, sizeof(int), 0) < 0)
	{
		close(connection_socket_fd);
		fprintf(stderr, "CLIENT: ERROR sending message size\n");
		exit(1);
	}

	// send message
	int total_bytes_sent = 0;
	while (total_bytes_sent < message_size)
	{
		// move pointer forward in message, and only send remaining message
		int bytes_sent = send(connection_socket_fd, message + total_bytes_sent, message_size - total_bytes_sent, 0);
		if (bytes_sent == -1)
		{
			close(connection_socket_fd);
			fprintf(stderr, "CLIENT: ERROR sending message\n");
			exit(1);
		}
		total_bytes_sent += bytes_sent;
	}
}

/**
 * Receives a message on the client side over the given socket, and
 * returns a pointer to the message in memory.
 * @param connection_socket_fd: int, file descriptor of the connection socket
 * @return message: string, the full message
 */
char *receive_message(int connection_socket_fd)
{
	// receive message size
	int message_size;
	if (recv(connection_socket_fd, &message_size, sizeof(int), 0) < 0)
	{
		close(connection_socket_fd);
		fprintf(stderr, "CLIENT: ERROR receiving message size\n");
		exit(1);
	}
	message_size = ntohl(message_size); // convert to host byte order

	// allocate memory for message based on size
	char *message = calloc(message_size + 1, sizeof(char)); // +1 for null terminator
	if (!message)
	{
		close(connection_socket_fd);
		fprintf(stderr, "CLIENT: ERROR allocating memory for message\n");
		exit(1);
	}

	// receive the message
	int total_bytes_received = 0;

	while (total_bytes_received < message_size)
	{
		// move pointer forward in message, and only receive remaining message
		int bytes_received = recv(connection_socket_fd, message + total_bytes_received, message_size - total_bytes_received, 0);
		if (bytes_received < 0)
		{
			close(connection_socket_fd);
			free(message);
			fprintf(stderr, "CLIENT: ERROR receiving message\n");
			exit(1);
		}
		total_bytes_received += bytes_received;
	}
	message[message_size] = '\0'; // ensure null termination
	return message;
}

/**
 * Main function for the encryption client.
 * Connects to the encryption server, sends the plaintext and encryption key,
 * receives and prints the ciphertext.
 * @param argument_count: int, the number of command line arguments
 * @param argument_array: array, the command line arguments entered (1: program name,
 * 2: plaintext file name, 3: key file name, 4: port number)
 */
int main(int argument_count, char *argument_array[])
{
	int connection_socket_fd;
	struct sockaddr_in client_socket_address; // struct to hold socket address (IP address + port number) of client

	// check if correct amount of arguments is given
	if (argument_count != 4)
	{
		fprintf(stderr, "USAGE: hostname plaintext key port\n");
		exit(1);
	}

	// read plaintext and key from files
	int plaintext_size;
	int encryption_key_size;
	char *plaintext = read_file(argument_array[1], &plaintext_size);
	char *encryption_key = read_file(argument_array[2], &encryption_key_size);

	// check that encryption key is at least as long as the plaintext
	if (encryption_key_size < plaintext_size)
	{
		fprintf(stderr, "CLIENT: ERROR, encryption key is too short\n");
		free(plaintext);
		free(encryption_key);
		exit(1);
	}

	// create a socket
	connection_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP
	if (connection_socket_fd < 0)
	{
		fprintf(stderr, "CLIENT: ERROR opening socket\n");
		exit(1);
	}
	setup_client_address_struct(&client_socket_address, atoi(argument_array[3]), "localhost"); // set up the address struct for the client socket

	// connect to server
	if (connect(connection_socket_fd, (struct sockaddr *)&client_socket_address, sizeof(client_socket_address)) < 0)
	{
		close(connection_socket_fd);
		fprintf(stderr, "CLIENT: ERROR connecting to server\n");
		exit(2);
	}

	// send identification, plaintext, and encryption key to server
	send(connection_socket_fd, "encrypt", 7, 0);
	send_message(connection_socket_fd, plaintext, plaintext_size);
	send_message(connection_socket_fd, encryption_key, encryption_key_size);

	// receive ciphertext from server
	char *ciphertext = receive_message(connection_socket_fd);
	if (!ciphertext)
	{
		fprintf(stderr, "CLIENT: ERROR receiving ciphertext\n");
		free(plaintext);
		free(encryption_key);
		close(connection_socket_fd);
		exit(1);
	}

	printf("%s\n", ciphertext); // add newline back

	// clean up and exit
	free(plaintext);
	free(encryption_key);
	free(ciphertext);
	close(connection_socket_fd); // close the socket
	return 0;
}

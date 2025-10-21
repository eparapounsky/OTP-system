#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/wait.h> // for waitpid

// function prototypes
void setup_server_address_struct(struct sockaddr_in *socket_address, int port_number);
void check_client_type(int connection_socket_fd);
void handle_client_child(int connection_socket_fd);
void decrypt_ciphertext(char *plaintext, char *encryption_key, char *ciphertext);
void send_message(int connection_socket_fd, char *message, int message_size);
char *receive_message(int connection_socket_fd);

/**
 * Sets up a server socket address struct.
 * @param socket_address: pointer to sockaddr_in structure
 * @param port_number: int, port number on which the server will listen
 */
void setup_server_address_struct(struct sockaddr_in *socket_address, int port_number)
{
	memset((char *)socket_address, '\0', sizeof(*socket_address)); // clear out the socket address struct
	socket_address->sin_family = AF_INET;						   // the address should be network capable
	socket_address->sin_port = htons(port_number);				   // bind to port number after converting to network byte order
	socket_address->sin_addr.s_addr = INADDR_ANY;				   // allow a client at any address to connect to this server
}

/**
 * Receives the client type from the client (encrypt or decrypt).
 * Rejects the client connection if the client is not 'decrypt'.
 * @param connection_socket_fd: int, file descriptor of the connection socket
 */
void check_client_type(int connection_socket_fd)
{
	char client_type[8];
	int bytes_received = 0;
	int total_bytes_received = 0;
	int expected_bytes = 7;

	// receive client type with partial receive handling
	while (total_bytes_received < expected_bytes)
	{
		bytes_received = recv(connection_socket_fd, client_type + total_bytes_received,
							  expected_bytes - total_bytes_received, 0);

		if (bytes_received < 0)
		{
			fprintf(stderr, "SERVER: ERROR receiving client type\n");
			close(connection_socket_fd);
			_exit(1);
		}
		else if (bytes_received == 0)
		{
			fprintf(stderr, "SERVER: ERROR client disconnected unexpectedly\n");
			close(connection_socket_fd);
			_exit(1);
		}

		total_bytes_received += bytes_received;
	}

	client_type[7] = '\0'; // ensure null-termination

	// close connection if the client type is not 'decrypt'
	if (strcmp(client_type, "decrypt") != 0)
	{
		fprintf(stderr, "SERVER: ERROR- client rejected\n");
		close(connection_socket_fd);
		_exit(1);
	}
}

/**
 * Handles the client in a separate process.
 * Checks the client type, receives the ciphertext and encryption key from the client,
 * calls decrypt_ciphertext(), and sends the plaintext to the client.
 * @param connection_socket_fd: int, file descriptor of the connection socket
 */
void handle_client_child(int connection_socket_fd)
{
	check_client_type(connection_socket_fd);

	// receive ciphertext from client
	char *ciphertext = receive_message(connection_socket_fd);
	if (!ciphertext)
	{
		fprintf(stderr, "SERVER: ERROR receiving ciphertext\n");
		close(connection_socket_fd);
		_exit(1);
	}

	// receive encryption key from client
	char *encryption_key = receive_message(connection_socket_fd);
	if (!encryption_key)
	{
		fprintf(stderr, "SERVER: ERROR receiving encryption key\n");
		close(connection_socket_fd);
		free(ciphertext);
		_exit(1);
	}

	// check that encryption key is at least as long as the ciphertext
	if (strlen(encryption_key) < strlen(ciphertext))
	{
		fprintf(stderr, "SERVER: ERROR- encryption key is too short\n");
		close(connection_socket_fd);
		free(ciphertext);
		free(encryption_key);
		_exit(1);
	}

	// allocate memory for plaintext
	char *plaintext = calloc(strlen(ciphertext) + 1, sizeof(char)); // +1 for null terminator
	if (!plaintext)
	{
		fprintf(stderr, "SERVER: ERROR on allocating memory for plaintext\n");
		close(connection_socket_fd);
		free(ciphertext);
		free(encryption_key);
		_exit(1);
	}

	decrypt_ciphertext(plaintext, encryption_key, ciphertext);
	send_message(connection_socket_fd, plaintext, strlen(plaintext));

	// clean up
	free(ciphertext);
	free(encryption_key);
	free(plaintext);
	close(connection_socket_fd);
}

/**
 * Decrypts ciphertext to plaintext using the one time pad method.
 * @param plaintext: string, the decrypted message
 * @param encryption_key: string, the key used for decryption
 * @param ciphertext: string, the message to be decrypted
 */
void decrypt_ciphertext(char *plaintext, char *encryption_key, char *ciphertext)
{
	int i; // loop variable
	int ciphertext_length = strlen(ciphertext);

	for (i = 0; i < ciphertext_length; i++)
	{
		// convert ciphertext to numbers
		int converted_ciphertext;
		if (ciphertext[i] == ' ')
		{
			converted_ciphertext = 26; // space is 26 in CHARACTERS
		}
		else
		{
			converted_ciphertext = ciphertext[i] - 'A'; // convert A-Z to number from 0-25
		}

		// convert encryption key to numbers
		int converted_encryption_key;
		if (encryption_key[i] == ' ')
		{
			converted_encryption_key = 26;
		}
		else
		{
			converted_encryption_key = encryption_key[i] - 'A';
		}

		// apply decryption (add 27 to avoid negative values)
		int decrypted_value = (converted_ciphertext - converted_encryption_key + 27) % 27;

		// convert decrypted value to characters
		if (decrypted_value == 26)
		{
			plaintext[i] = ' ';
		}
		else
		{
			plaintext[i] = 'A' + decrypted_value; // convert 0-25 to character from A-Z
		}
	}
	plaintext[i] = '\0'; // add null terminator
}

/**
 * Sends a message from the server side over the given socket.
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
		fprintf(stderr, "SERVER: ERROR sending message size\n");
		close(connection_socket_fd);
		_exit(1);
	}

	// send message
	int total_bytes_sent = 0;
	while (total_bytes_sent < message_size)
	{
		// move pointer forward in message, and only send remaining message
		int bytes_sent = send(connection_socket_fd, message + total_bytes_sent, message_size - total_bytes_sent, 0);
		if (bytes_sent == -1)
		{
			fprintf(stderr, "SERVER: ERROR sending message\n");
			close(connection_socket_fd);
			_exit(1);
		}
		total_bytes_sent += bytes_sent;
	}
}

/**
 * Receives a message on the server side over the given socket, and
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
		fprintf(stderr, "SERVER: ERROR receiving message size\n");
		_exit(1);
	}
	message_size = ntohl(message_size); // convert to host byte order

	// allocate memory for message based on size
	char *message = calloc(message_size + 1, sizeof(char)); // +1 for null terminator
	if (!message)
	{
		close(connection_socket_fd);
		fprintf(stderr, "SERVER: ERROR allocating memory for message\n");
		_exit(1);
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
			fprintf(stderr, "SERVER: ERROR receiving message\n");
			_exit(1);
		}
		total_bytes_received += bytes_received;
	}

	message[message_size] = '\0'; // ensure null termination
	return message;
}

/**
 * Main function for the decryption server.
 * Creates a server socket that listens for client connections and handles each client in a child process.
 * @param argument_count: int, the number of command line arguments
 * @param argument_array: array, the command line arguments entered (first is the program name,
 * second is the port number)
 */
int main(int argument_count, char *argument_array[])
{
	int connection_socket_fd; // to hold socket descriptor of a connected client

	// structs to hold socket addresses (IP address + port number) of server and client
	struct sockaddr_in server_socket_address;
	struct sockaddr_in client_socket_address;

	// check if correct amount of arguments is given
	if (argument_count < 2)
	{
		fprintf(stderr, "Please specify the port number.\n");
		exit(1);
	}
	else if (argument_count > 2)
	{
		fprintf(stderr, "Please ONLY specify the port number.\n");
		exit(1);
	}

	// create the socket that will listen for connections
	int listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP
	if (listening_socket_fd < 0)
	{
		fprintf(stderr, "SERVER: ERROR opening socket\n");
		exit(1);
	}

	setup_server_address_struct(&server_socket_address, atoi(argument_array[1])); // set up the address struct for the server socket

	// associate the server socket with the given port
	if (bind(listening_socket_fd, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address)) < 0)
	{
		fprintf(stderr, "SERVER: ERROR on binding\n");
		exit(1);
	}

	listen(listening_socket_fd, 5);								   // start listening for client connections; allow up to 5 connections to queue up
	socklen_t size_of_client_info = sizeof(client_socket_address); // to hold size of client's socket address

	// accept client connections, blocking if one is not available, until one connects
	while (true)
	{
		// accept the connection request, which creates a connection socket
		connection_socket_fd = accept(listening_socket_fd, (struct sockaddr *)&client_socket_address, &size_of_client_info);

		if (connection_socket_fd < 0)
		{
			fprintf(stderr, "SERVER: ERROR on accept\n");
			exit(1);
		}

		pid_t child_PID = fork(); // create new process

		switch (child_PID)
		{
		case -1:
			perror("fork() failed\n");
			exit(1);
			break;

		case 0: // child process
			handle_client_child(connection_socket_fd);
			_exit(0); // terminate child process

		default:						 // parent process
			close(connection_socket_fd); // close the connection socket for this client

			// wait for any terminated children
			while (waitpid(-1, NULL, WNOHANG) > 0)
				;
		}
	}
	close(listening_socket_fd); // close the listening socket
	return 0;
}

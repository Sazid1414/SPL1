#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include "AES.h"
#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
static _Atomic unsigned int cli_count = 0;
static int uid = 10;
#define MAX_STRING_LENGTH 100
struct User
{
	char email[MAX_STRING_LENGTH];
	char password[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
};
/* Client structure */
typedef struct
{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout()
{
	printf("\r%s", "> ");
	fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{ // trim \n
		if (arr[i] == '\n')
		{
			arr[i] = '\0';
			break;
		}
	}
}

void print_client_addr(struct sockaddr_in address)
{
	printf("%d.%d.%d.%d",
		   address.sin_addr.s_addr & 0xff,
		   (address.sin_addr.s_addr & 0xff00) >> 8,
		   (address.sin_addr.s_addr & 0xff0000) >> 16,
		   (address.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add clients to queue */
void queue_add(client_t *cl)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (!clients[i])
		{
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid == uid)
			{
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message(char *s, int uid)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid != uid)
			{
				if (write(clients[i]->sockfd, s, strlen(s)) < 0)
				{
					printf("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{
	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	if (recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1)
	{
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	}
	else
	{
		strcpy(cli->name, name);
		sprintf(buff_out, "%s has joined\n", cli->name);
		printf("%s", buff_out);
		send_message(buff_out, cli->uid);
	}

	bzero(buff_out, BUFFER_SZ);

	while (1)
	{
		if (leave_flag)
		{
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0)
		{
			if (strlen(buff_out) > 0)
			{
				send_message(buff_out, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s -> %s\n", buff_out, cli->name);
				// printf("%s:", buff_out);
			}
		}
		else if (receive == 0 || strcmp(buff_out, "exit") == 0)
		{
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		}
		else
		{
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

	/* Delete client from queue and yield thread */
	close(cli->sockfd);
	queue_remove(cli->uid);
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());

	return NULL;
}
void write_user_data(const char *filename, const struct User *new_user)
{
	FILE *file = fopen(filename, "a");

	if (!file)
	{
		printf("Error opening user file");
		return;
	}

	fprintf(file, "%s %s %s\n", new_user->email, new_user->password, new_user->name);

	fclose(file);
}

int authenticate_user(const char *filename, const char *email, const char *password)
{
	FILE *file = fopen(filename, "r");

	if (!file)
	{
		printf("Error opening user file");
		return 0; // Authentication failed
	}

	struct User user;
	while (fscanf(file, "%99s %99s %99s", user.email, user.password, user.name) == 3)
	{
		if (strcmp(user.email, email) == 0 && strcmp(user.password, password) == 0)
		{
			fclose(file);
			return 1; // Authentication successful
		}
	}

	fclose(file);
	return 0; // Authentication failed
}

int register_user()
{
	struct User new_user;

	printf("Enter email: ");
	scanf("%s", new_user.email);

	printf("Enter password: ");
	scanf("%s", new_user.password);

	printf("Enter name: ");
	scanf("%s", new_user.name);

	// Check if the email is already registered
	if (authenticate_user("user.txt", new_user.email, new_user.password))
	{
		printf("Error: Email already registered.\n");
		return 0;
	}
	else
	{
		// Save the new user data to the file
		write_user_data("user.txt", &new_user);

		printf("Registration successful!\n");
		return 1;
	}
	return 1;
}

int login_user()
{
	char email[MAX_STRING_LENGTH];
	char password[MAX_STRING_LENGTH];

	printf("Enter email: ");
	scanf("%99s", email);

	printf("Enter password: ");
	scanf("%99s", password);

	if (authenticate_user("user.txt", email, password))
	{
		printf("Login successful!\n");
		return 1;
	}
	else
	{
		printf("Login failed. Invalid email or password.\n");
		return 0;
	}
	return 1;
}
int main(int argc, char **argv)
{
	int options;
	int continueLoop = 0;
	do
	{

		printf("Select an option:\n");
		printf("1. Register\n");
		printf("2. Login\n");
		printf("3. Exit\n");

		scanf("%d", &options);

		switch (options)
		{
		case 1:
			if (register_user())
			{
				continueLoop = 1;
			}

			break;
		case 2:
			if (login_user())
			{
				continueLoop = 1;
			}
			break;
		case 3:
			printf("Exiting...\n");
			break;
		default:
			printf("Invalid option. Please try again.\n");
		}
	} while (continueLoop == 0);

	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	// if (argc != 2)
	// {
	// 	printf("Usage: %s <port>\n", argv[0]);
	// 	return EXIT_FAILURE;
	// }

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);
	int option = 1;
	int listenfd = 0, connfd = 0;
	/*struct sockaddr_in
  {
	__SOCKADDR_COMMON (sin_);
	in_port_t sin_port;
	struct in_addr sin_addr;


	unsigned char sin_zero[sizeof (struct sockaddr)
			   - __SOCKADDR_COMMON_SIZE
			   - sizeof (in_port_t)
			   - sizeof (struct in_addr)];
  };*/
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t threadIdentifier;

	/* Socket settings */
	/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	/* Convert Internet host address from numbers-and-dots notation in CP
   into binary data in network byte order.  */
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	// htons =HostToNetwork Converts the port number in netwok byte order
	serv_addr.sin_port = htons(port);

	/* Ignore pipe signals */
	// Prevents the code from being crashed if client exists early
	// SIG_IGN is a constant that indicates the signal should be ignored
	signal(SIGPIPE, SIG_IGN);
	/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
/*#ifndef __USE_TIME_BITS64
	extern int setsockopt(int __fd, int __level, int __optname,
						  const void *__optval, socklen_t __optlen) __THROW; */

	if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
	{
		printf("setsockopt failed!");
		return EXIT_FAILURE;
	}

	/* Bind */
	if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Socket binding failed!");
		return EXIT_FAILURE;
	}

	/* Listen */
	if (listen(listenfd, 10) < 0)
	{
		printf("Socket listening failed!");
		return EXIT_FAILURE;
	}

	printf("***WELCOME TO THE CHATSERVER***\n");

	while (1)
	{
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if ((cli_count + 1) == MAX_CLIENTS)
		{
			printf("Max clients reached. Rejected: ");
			print_client_addr(cli_addr);
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&threadIdentifier, NULL, &handle_client, (void *)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}
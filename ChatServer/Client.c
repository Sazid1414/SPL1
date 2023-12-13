#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define LENGTH 2048
#define MAX_USERS 100
#define MAX_STRING_LENGTH 100
struct User
{
	char email[MAX_STRING_LENGTH];
	char password[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
};
// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_overwrite_stdout()
{
	printf("%s", "> ");
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

void catch_ctrl_c_and_exit(int sig)
{
	flag = 1;
}

void send_msg_handler()
{
	char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

	while (1)
	{
		str_overwrite_stdout();
		fgets(message, LENGTH, stdin);
		str_trim_lf(message, LENGTH);

		if (strcmp(message, "exit") == 0)
		{
			break;
		}
		else
		{
			sprintf(buffer, "%s: %s\n", name, message);
			send(sockfd, buffer, strlen(buffer), 0);
		}

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
	}
	catch_ctrl_c_and_exit(2);
}

void recv_msg_handler()
{
	char message[LENGTH] = {};
	while (1)
	{
		int receive = recv(sockfd, message, LENGTH, 0);
		if (receive > 0)
		{
			printf("%s", message);
			str_overwrite_stdout();
		}
		else if (receive == 0)
		{
			break;
		}
		else
		{
			// -1
		}
		memset(message, 0, sizeof(message));
	}
}
void write_user_data(const char *filename, const struct User *new_user)
{
	FILE *file = fopen(filename, "a");

	if (!file)
	{
		perror("Error opening user file");
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
		perror("Error opening user file");
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
                // After successful login, get the user's name
                printf("Please enter your name: \n");
                fgets(name, 32, stdin);
                str_trim_lf(name, strlen(name));
                
                // Send name
                send(sockfd, name, 32, 0);

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
	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);
	signal(SIGINT, catch_ctrl_c_and_exit);
	char name[32];
	printf("Please enter your name: \n");
	fgets(name, 32, stdin);
	str_trim_lf(name, strlen(name));
	if (strlen(name) > 32 || strlen(name) < 2)
	{
		printf("Name must be less than 30 and more than 2 characters.\n");
		return EXIT_FAILURE;
	}
	struct sockaddr_in server_addr;
	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	// Connect to Server
	int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Send name
	send(sockfd, name, 32, 0);

	printf("=== WELCOME TO THE CHATROOM ===\n");

	pthread_t send_msg_thread;
	if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)
	{
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
	if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
	{
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if (flag)
		{
			printf("\nBye\n");
			break;
		}
	}

	close(sockfd);

	return EXIT_SUCCESS;
}
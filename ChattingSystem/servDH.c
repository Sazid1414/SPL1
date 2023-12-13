#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/dh.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#define PORT 12345
#define AES_KEY_SIZE 256
#define AES_BLOCK_SIZE 16

void handleErrors()
{
    fprintf(stderr, "Error occurred\n");
    exit(EXIT_FAILURE);
}

void generateKeyAndIV(unsigned char *key, unsigned char *iv)
{
    if (RAND_bytes(key, AES_KEY_SIZE / 8) != 1 || RAND_bytes(iv, AES_BLOCK_SIZE) != 1)
        handleErrors();
}

void performDHKeyExchange(DH *dh, unsigned char *shared_key, const BIGNUM *other_party_key)
{
    if (DH_compute_key(shared_key, other_party_key, dh) == -1)
        handleErrors();
}

void aesEncrypt(const unsigned char *plaintext, int plaintext_len, const unsigned char *key, const unsigned char *iv, unsigned char *ciphertext)
{
    AES_KEY enc_key;
    if (AES_set_encrypt_key(key, AES_KEY_SIZE, &enc_key) != 0)
        handleErrors();

    AES_cbc_encrypt(plaintext, ciphertext, plaintext_len, &enc_key, iv, AES_ENCRYPT);
}

void aesDecrypt(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, const unsigned char *iv, unsigned char *decryptedtext)
{
    AES_KEY dec_key;
    if (AES_set_decrypt_key(key, AES_KEY_SIZE, &dec_key) != 0)
        handleErrors();

    AES_cbc_encrypt(ciphertext, decryptedtext, ciphertext_len, &dec_key, iv, AES_DECRYPT);
}

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    DH *dh;
    unsigned char *shared_key;
    BIGNUM *server_private, *server_public, *client_public;
    unsigned char *key, *iv;
    unsigned char received_data[1024], encrypted_data[1024], decrypted_data[1024];

    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Binding failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 5) == -1)
    {
        perror("Listening failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept connection
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize)) == -1)
    {
        perror("Accepting connection failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Allocate memory
    dh = DH_new();
    shared_key = (unsigned char *)malloc(DH_size(dh));
    server_private = BN_new();
    server_public = BN_new();
    client_public = BN_new();
    key = (unsigned char *)malloc(AES_KEY_SIZE / 8);
    iv = (unsigned char *)malloc(AES_BLOCK_SIZE);

    // Generate DH parameters
    if (DH_generate_parameters_ex(dh, 256, DH_GENERATOR_2, NULL) != 1 ||
        DH_generate_key(dh) != 1)
        handleErrors();

    // Server generates private and public keys
    if (RAND_bytes((unsigned char *)server_private, DH_size(dh)) != 1)
        handleErrors();

    if (DH_compute_key(server_public, client_public, dh) == -1)
        handleErrors();

    // Send DH parameters and public key to the client
    send(clientSocket, dh->p, DH_size(dh), 0);
    send(clientSocket, dh->g, DH_size(dh), 0);
    send(clientSocket, server_public, DH_size(dh), 0);

    // Receive client's public key
    recv(clientSocket, client_public, DH_size(dh), 0);

    // Perform DH key exchange
    performDHKeyExchange(dh, shared_key, client_public);

    // Receive encrypted data from the client
    recv(clientSocket, received_data, sizeof(received_data), 0);

    // Decrypt the received data
    aesDecrypt(received_data, sizeof(received_data), shared_key, iv, decrypted_data);

    // Output results
    printf("Decrypted Text: %s\n", decrypted_data);

    // Clean up
    free(shared_key);
    BN_free(server_private);
    BN_free(server_public);
    BN_free(client_public);
    free(key);
    free(iv);
    DH_free(dh);

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}

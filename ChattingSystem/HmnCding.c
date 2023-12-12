#include <stdio.h>
#include <stdlib.h>

struct Node {
    char data;
    int freq;
    struct Node* left;
    struct Node* right;
};

struct Node* build_huffman_tree(char value, int freq) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->data = value;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

struct Node* build_huffman_tree_from_frequencies(int* frequencies) {
    // Priority queue for nodes
    struct Node** nodes = (struct Node**)malloc(256 * sizeof(struct Node*));
    int node_count = 0;

    // Initialize nodes for each character
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            nodes[node_count++] = build_huffman_tree((char)i, frequencies[i]);
        }
    }

    // Build Huffman tree
    while (node_count > 1) {
        // Find two nodes with the lowest frequency
        int min1 = 0, min2 = 1;
        for (int i = 2; i < node_count; i++) {
            if (nodes[i]->freq < nodes[min1]->freq) {
                min2 = min1;
                min1 = i;
            } else if (nodes[i]->freq < nodes[min2]->freq) {
                min2 = i;
            }
        }

        // Create a new node with the sum of frequencies
        struct Node* merged_node = build_huffman_tree('\0', nodes[min1]->freq + nodes[min2]->freq);
        merged_node->left = nodes[min1];
        merged_node->right = nodes[min2];

        // Remove the used nodes and add the new merged node
        nodes[min1] = merged_node;
        nodes[min2] = nodes[node_count - 1];
        node_count--;
    }

    struct Node* root = nodes[0];
    free(nodes);

    return root;
}

void print_huffman_tree(struct Node* root, int depth) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    if (root->data == '\0') {
        printf("Internal Node (freq=%d)\n", root->freq);
    } else {
        printf("Leaf Node: %c (freq=%d)\n", root->data, root->freq);
    }

    print_huffman_tree(root->left, depth + 1);
    print_huffman_tree(root->right, depth + 1);
}

void calculate_frequencies(const char* text, int* frequencies) {
    while (*text != '\0') {
        frequencies[*text]++;
        text++;
    }
}

int calculate_original_size(int* frequencies) {
    int size = 0;
    for (int i = 0; i < 256; i++) {  // Assuming ASCII characters
        size += frequencies[i] * sizeof(char) * 8;  // 8 bits per character
    }
    return size;
}

int calculate_compressed_size(struct Node* root, int depth) {
    if (root == NULL) {
        return 0;
    }

    if (root->data != '\0') {
        return root->freq * depth;
    }

    return calculate_compressed_size(root->left, depth + 1) + calculate_compressed_size(root->right, depth + 1);
}

void huffman_decode(char* encoded_text, struct Node* root) {
    struct Node* current_node = root;
    int i = 0;

    while (encoded_text[i] != '\0') {
        if (encoded_text[i] == '0') {
            current_node = current_node->left;
        } else {
            current_node = current_node->right;
        }

        if (current_node->data != '\0') {
            printf("%c", current_node->data);
            current_node = root;
        }

        i++;
    }
}

int main() {
    char original_text[] = "abracadabra";
    int frequencies[256] = {0};

    calculate_frequencies(original_text, frequencies);

    int original_size = calculate_original_size(frequencies);

    // Build Huffman tree
    struct Node* huffman_tree = build_huffman_tree_from_frequencies(frequencies);

    // Example encoded text
    char encoded_text[] = "01010101100";

    // Decode the text
    printf("Original Text: %s\n", original_text);
    printf("Encoded Text: %s\n", encoded_text);
    printf("Decoded Text: ");
    huffman_decode(encoded_text, huffman_tree);
    printf("\n");

    // Print the Huffman tree
    printf("Huffman Tree:\n");
    print_huffman_tree(huffman_tree, 0);

    // Calculate sizes and compression ratio
    int compressed_size = calculate_compressed_size(huffman_tree, 0);
    float compression_ratio = (float)(( original_size/compressed_size )*100);

    printf("\nOriginal Size: %d bits\n", original_size);
    printf("Compressed Size: %d bits\n", compressed_size);
    printf("Compression Ratio: %.2f\n", compression_ratio);

    return 0;
}

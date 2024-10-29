#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100

// A Huffman Tree node
struct MinHeapNode {
    char data; // Input character
    unsigned freq; // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right children
};

// A Min Heap: Collection of Huffman Tree nodes
struct MinHeap {
    unsigned size; 
    unsigned capacity; 
    struct MinHeapNode** array;
};

// A function to create a new Huffman tree node
struct MinHeapNode* createNode(char data, unsigned freq) {
    struct MinHeapNode* newNode = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    newNode->left = newNode->right = NULL;
    newNode->data = data;
    newNode->freq = freq;
    return newNode;
}

// A function to create a min heap of a given capacity
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Swap function for two min heap nodes
void swapNodes(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Min-heapify function
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Extract the minimum node from the heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Insert a new node into the min heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Function to build a Huffman Tree
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createNode(data[i], freq[i]);

    minHeap->size = size;
    for (int i = (minHeap->size - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

// Print the Huffman codes
void printCodes(struct MinHeapNode* root, int arr[], int top, FILE *output) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, output);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, output);
    }

    if (!(root->left) && !(root->right)) {
        fprintf(output, "%c:", root->data);
        for (int i = 0; i < top; ++i)
            fprintf(output, "%d", arr[i]);
        fprintf(output, "\n");
    }
}

// Compress the input file
void compressFile(const char *inputFileName, const char *outputFileName) {
    FILE *input = fopen(inputFileName, "r");
    if (!input) {
        printf("File not found\n");
        return;
    }

    int freq[256] = {0};
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        freq[(int)ch]++;
    }
    fclose(input);

    char data[256];
    int dataFreq[256];
    int size = 0;

    for (int i = 0; i < 256; ++i) {
        if (freq[i]) {
            data[size] = (char)i;
            dataFreq[size] = freq[i];
            size++;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(data, dataFreq, size);

    int arr[MAX_TREE_HT], top = 0;
    FILE *output = fopen(outputFileName, "w");
    printCodes(root, arr, top, output);
    fclose(output);
    printf("File compressed successfully\n");
}

// Main function to get file names from the user
int main() {
    char inputFileName[100], outputFileName[100];

    printf("Enter the input file name: ");
    scanf("%s", inputFileName);

    printf("Enter the output file name: ");
    scanf("%s", outputFileName);

    compressFile(inputFileName, outputFileName);

    return 0;
}

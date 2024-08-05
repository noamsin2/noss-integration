//shoham huri 208014951
//noam karasso 209406867
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
afsdfsdfs
#define MAX_THREADS 4

typedef struct {
    int *array;
    int left;
    int right;
} ThreadData;

void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
//temp
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
//free the temp
    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void *threadedMergeSort(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    mergeSort(data->array, data->left, data->right);
    pthread_exit(NULL);
}

void multiThreadedMergeSort(int arr[], int n) {
    pthread_t threads[MAX_THREADS];
    ThreadData threadData[MAX_THREADS];
    int segmentSize = n / MAX_THREADS;
    
//create thread for each segment 
    for (int i = 0; i < MAX_THREADS; i++) {
        threadData[i].array = arr;
        threadData[i].left = i * segmentSize;
        threadData[i].right = (i == MAX_THREADS - 1) ? n - 1 : (i + 1) * segmentSize - 1;
        pthread_create(&threads[i], NULL, threadedMergeSort, &threadData[i]);
    }
//join threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
//merge the sorted segments
    for (int i = 1; i < MAX_THREADS; i++) {
        merge(arr, 0, threadData[i-1].right, threadData[i].right);
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int n = 15000; 
    int arr[n];
    srand(time(0));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }

    printf("Original array: \n");
    printArray(arr, n);

    clock_t start, end;
    double cpu_time_used;

    // Single-threaded MergeSort
    start = clock();
    mergeSort(arr, 0, n-1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Sorted array (Single-threaded): \n");
    printArray(arr, n);
    printf("Time taken by single-threaded MergeSort: %f seconds\n", cpu_time_used);

    // Generate a new random array
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }
    printf("Original array: \n");
    printArray(arr, n);

    // Multi-threaded MergeSort
    start = clock();
    multiThreadedMergeSort(arr, n);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Sorted array (Multi-threaded): \n");
    printArray(arr, n);
    printf("Time taken by multi-threaded MergeSort: %f seconds\n", cpu_time_used);
fdsf
    return 0;
}



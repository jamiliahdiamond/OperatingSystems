#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

typedef struct {
    int thread_id;
} ThreadArgs;

void* computeSum(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    int start_row = targs->thread_id * 2;
    int end_row = start_row + 2;

    for (int i = start_row; i < end_row && i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }

    free(args);
    return NULL;
}

void* computeDiff(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    int start_row = targs->thread_id * 2;
    int end_row = start_row + 2;

    for (int i = start_row; i < end_row && i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }

    free(args);
    return NULL;
}

void* computeProduct(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    int start_row = targs->thread_id * 2;
    int end_row = start_row + 2;

    for (int i = start_row; i < end_row && i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    free(args);
    return NULL;
}

int main() {
    srand(time(0));

    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread_t objects for our threads.
    pthread_t threads[NUM_THREADS * 3];

    // 4. Create threads for each matrix operation.
    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs* sum_args = malloc(sizeof(ThreadArgs));
        ThreadArgs* diff_args = malloc(sizeof(ThreadArgs));
        ThreadArgs* prod_args = malloc(sizeof(ThreadArgs));

        sum_args->thread_id = diff_args->thread_id = prod_args->thread_id = i;

        pthread_create(&threads[i], NULL, computeSum, sum_args);
        pthread_create(&threads[i + NUM_THREADS], NULL, computeDiff, diff_args);
        pthread_create(&threads[i + 2 * NUM_THREADS], NULL, computeProduct, prod_args);
    }

    // 5. Wait for all threads to finish.
    for (int i = 0; i < NUM_THREADS * 3; i++) {
        pthread_join(threads[i], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
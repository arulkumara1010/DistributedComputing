#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4 // Matrix size (NxN)

void printMatrix(int matrix[N][N], const char *name) {
  printf("\n%s Matrix:\n", name);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++)
      printf("%3d ", matrix[i][j]);
    printf("\n");
  }
}

void initRandomMatrix(int matrix[N][N]) {
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      matrix[i][j] = rand() % 10; // Random values between 0-9
}

int main(int argc, char *argv[]) {
  int rank, size;
  int A[N][N], B[N][N], C[N][N] = {0}; // Matrices
  int local_rows;
  int *sendcounts;
  int *displs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Calculate how many rows each process will handle
  local_rows = N / size;
  if (rank < N % size) {
    local_rows++;
  }

  // Allocate memory for local arrays
  int(*local_A)[N] = malloc(local_rows * sizeof(*local_A));
  int(*local_C)[N] = malloc(local_rows * sizeof(*local_C));

  // Initialize to zero
  for (int i = 0; i < local_rows; i++) {
    for (int j = 0; j < N; j++) {
      local_C[i][j] = 0;
    }
  }

  // For scatterv and gatherv
  if (rank == 0) {
    sendcounts = malloc(size * sizeof(int));
    displs = malloc(size * sizeof(int));

    int disp = 0;
    for (int i = 0; i < size; i++) {
      int rows_for_proc = N / size;
      if (i < N % size) {
        rows_for_proc++;
      }
      sendcounts[i] = rows_for_proc * N;
      displs[i] = disp;
      disp += rows_for_proc * N;
    }
  }

  // Initialize random matrices on rank 0
  if (rank == 0) {
    srand(time(NULL));
    initRandomMatrix(A);
    initRandomMatrix(B);

    // Print input matrices
    printMatrix(A, "A");
    printMatrix(B, "B");
  }

  // Distribute matrix A using MPI_Scatterv
  MPI_Scatterv(A, sendcounts, displs, MPI_INT, local_A, local_rows * N, MPI_INT,
               0, MPI_COMM_WORLD);

  // Broadcast matrix B to all processes
  MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

  // Compute matrix multiplication
  for (int i = 0; i < local_rows; i++) {
    for (int j = 0; j < N; j++) {
      local_C[i][j] = 0;
      for (int k = 0; k < N; k++) {
        local_C[i][j] += local_A[i][k] * B[k][j];
      }
    }
  }

  // Gather results back to process 0
  MPI_Gatherv(local_C, local_rows * N, MPI_INT, C, sendcounts, displs, MPI_INT,
              0, MPI_COMM_WORLD);

  // Print the result
  if (rank == 0) {
    printMatrix(C, "C (Result)");
    free(sendcounts);
    free(displs);
  }

  // Free allocated memory
  free(local_A);
  free(local_C);

  MPI_Finalize();
  return 0;
}

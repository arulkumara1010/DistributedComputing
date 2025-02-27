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

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Calculate how many rows each process will handle
  int rows_per_proc = N / size;
  int remainder = N % size;

  // Calculate start_row and end_row for each process
  int start_row = rank * rows_per_proc + (rank < remainder ? rank : remainder);
  int end_row = start_row + rows_per_proc + (rank < remainder ? 1 : 0);
  int local_rows = end_row - start_row;

  // Allocate memory for local arrays
  int(*local_A)[N] = malloc(local_rows * sizeof(*local_A));
  int(*local_C)[N] = malloc(local_rows * sizeof(*local_C));

  // Initialize matrices on root process
  if (rank == 0) {
    srand(time(NULL));
    initRandomMatrix(A);
    initRandomMatrix(B);

    // Print input matrices
    printMatrix(A, "A");
    printMatrix(B, "B");
  }

  // Distribute matrix B to all processes
  if (rank == 0) {
    // Master keeps its portion of A
    for (int i = 0; i < local_rows; i++) {
      for (int j = 0; j < N; j++) {
        local_A[i][j] = A[i][j];
      }
    }

    // Send portions of A to other processes
    int current_row = local_rows;
    for (int dest = 1; dest < size; dest++) {
      int dest_rows = rows_per_proc + (dest < remainder ? 1 : 0);
      MPI_Send(&A[current_row][0], dest_rows * N, MPI_INT, dest, 0,
               MPI_COMM_WORLD);
      current_row += dest_rows;
    }

    // Send matrix B to all other processes
    for (int dest = 1; dest < size; dest++) {
      MPI_Send(&B[0][0], N * N, MPI_INT, dest, 1, MPI_COMM_WORLD);
    }
  } else {
    // Receive portion of A
    MPI_Recv(local_A, local_rows * N, MPI_INT, 0, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    // Receive entire matrix B
    MPI_Recv(B, N * N, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Perform local matrix multiplication
  for (int i = 0; i < local_rows; i++) {
    for (int j = 0; j < N; j++) {
      local_C[i][j] = 0;
      for (int k = 0; k < N; k++) {
        local_C[i][j] += local_A[i][k] * B[k][j];
      }
    }
  }

  // Gather results to the root process
  if (rank == 0) {
    // Copy local results to C
    for (int i = 0; i < local_rows; i++) {
      for (int j = 0; j < N; j++) {
        C[i][j] = local_C[i][j];
      }
    }

    // Receive results from other processes
    int current_row = local_rows;
    for (int src = 1; src < size; src++) {
      int src_rows = rows_per_proc + (src < remainder ? 1 : 0);
      MPI_Recv(&C[current_row][0], src_rows * N, MPI_INT, src, 2,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      current_row += src_rows;
    }

    // Print result matrix
    printMatrix(C, "C (Result)");
  } else {
    // Send local results back to root
    MPI_Send(local_C, local_rows * N, MPI_INT, 0, 2, MPI_COMM_WORLD);
  }

  // Free allocated memory
  free(local_A);
  free(local_C);

  MPI_Finalize();
  return 0;
}

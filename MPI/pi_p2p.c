#include <mpi.h>
#include <stdio.h>

double f(double x) {
  return 4.0 / (1.0 + x * x); // Function to integrate
}

int main(int argc, char *argv[]) {
  int rank, size;
  double pi = 0.0, local_sum = 0.0;
  int n = 1000000; // Number of intervals
  double h = 1.0 / n;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int start = rank * (n / size);
  int end = (rank + 1) * (n / size);

  for (int i = start; i < end; i++) {
    double x = (i + 0.5) * h;
    local_sum += f(x);
  }

  // Print only once per process
  printf("Process %d computed local sum: %f\n", rank, local_sum);

  // Reduce all local sums into pi at process 0
  MPI_Reduce(&local_sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    pi *= h;
    printf("Final Calculated Pi: %.15f\n", pi);
  }

  MPI_Finalize();
  return 0;
}

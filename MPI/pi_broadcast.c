#include <mpi.h>
#include <stdio.h>

double f(double x) {
  return 4.0 / (1.0 + x * x); // Function to integrate
}

int main(int argc, char *argv[]) {
  int rank, size;
  double pi = 0.0, local_sum = 0.0;
  int n = 100000000; // Number of intervals
  double h = 1.0 / n;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Broadcast the number of intervals to all processes
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Each process computes its local sum
  for (int i = rank; i < n; i += size) {
    double x = (i + 0.5) * h;
    local_sum += f(x);
  }

  local_sum *= h;

  // Print the local contribution of each process
  printf("Process %d computed local sum: %.15f\n", rank, local_sum);

  // Reduce local sums to compute the final Pi value
  MPI_Reduce(&local_sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Final Calculated Pi: %.15f\n", pi);
  }

  MPI_Finalize();
  return 0;
}

#include <stdio.h>
#include <upc.h>

shared int A[THREADS];
shared int B[THREADS]; 
shared int C[THREADS];

void read_matrix_from_file(shared int matrix[], char *filename, const int N);
void print_matrix(shared int matrix[], const int N);

int main(int argc, char **argv)
{
    // 1. initialisation
    int N = (int)sqrt(THREADS);

    // 2. Read matrices from file
    if (MYTHREAD == 0)
    {
        read_matrix_from_file(A, "A.txt", N);
        read_matrix_from_file(B, "B.txt", N);
    }

    upc_barrier;

    // 3. Algorithm (shifting and multiplication)
    int cnt;
	upc_forall(cnt = 0; cnt < THREADS; cnt++; &C[cnt])
	{
		//row
		int i = cnt / N;
		//column
		int j = cnt % N;
	
		//first shift
		int Ai = i;
		int Aj = (j + i) % N;
		int Bi = (i + j) % N;
		int Bj = j;
	
        //shifts
		for(int shift = 0; shift < N; shift++) 
		{
			Aj = (Aj + 1) % N;
			Bi = (Bi + 1) % N;

			int indexA = Ai * N + Aj;
			int indexB = Bi * N + Bj;
			C[cnt] += A[indexA] * B[indexB];
		}
	}

    upc_barrier;

    if(MYTHREAD == 0)
	{
		print_matrix(C, N);
	}

    return 0;
}

void read_matrix_from_file(shared int matrix[], char *filename, const int N)
{
    FILE *file;
    char ch[1024];
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Cannot open file %s!\n", filename);
        exit(1);
    }

    int insertion_counter = 0;
    int scanf_result;

    do
    {
        scanf_result = fscanf(file, " %1023s", ch);
        if (scanf_result == EOF)
            break;
        int int_to_insert = atoi(ch);
        matrix[insertion_counter] = int_to_insert;
        insertion_counter++;
    } while (scanf_result != EOF);

    fclose(file);
}

void print_matrix(shared int matrix[], const int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d\t", matrix[i*N + j]);
        printf("\n");
    }
    printf("\n");
}
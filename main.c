//////////////////////////////////////////////////
//		Matrix CSR Implementation
//
//		Craig Swearingen
//
//		17 May 2016
//
//
///////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Definition of struct to hold uncompressed matrix
typedef struct Matrix{
	int** matrix;
	int row;
	int column;
} Matrix;

// Definition of struct to hold CSR matrix
typedef struct CSR{
	int* value;
	int* column_index;
	int* row_pointer;
	int row;
	int nnz;
} CSR;

//Reads uncompressed matrix into struct
//Precondition: The matrix struct has been allocated but indiviual elements have not
// 				file_addres contains a file with a matrix in the format of:
//				Number_Of_Rows Number_Of_Columns
//				Number Number Number
//				Number Number Number ...
// Example of file format
/*
3 3
1 2 3
4 5 6
7 8 9
*/
void readMatrix(Matrix* m, char* file_address)
{
	//Open file for reading
	FILE* file_pointer;
	file_pointer = fopen(file_address, "r");
	
	// read row and col size and assign
	fscanf(file_pointer, "%d %d", &(m->row), &(m->column));

	//malloc the row of pointers to int
	int** tmp = (int**) malloc(m->row * sizeof(int*));

	// loop through row of pointers to ints
	for(unsigned int i = 0; i < m->row; i++)
	{
		//allocate each array of int pointers
		tmp[i] = (int*) malloc(m->column * sizeof(int));
		for(unsigned int x = 0; x < m->column; x++)
		{
			// read the int from file and assign
			fscanf(file_pointer, "%d", &(tmp[i][x]));
		}
	}
	fclose(file_pointer);
	m->matrix = tmp;
}

// Print a human readable matrix to the console
// Precondition: The matrix has been allocated and assigned
// Postcondition: The matrix is left unchanged
// 				  The matrix is printed to the console
void printMatrix(Matrix* m)
{
	for(unsigned int i = 0; i < m->row; i++)
	{
		printf("{");
		for(unsigned int x = 0; x < m->column; x++)
			printf(" %d ", m->matrix[i][x]);
		printf("}\n");
	}
}

// Helper Function for compressMatrixToCSR(Matrix* m, CSR* csr)
// Precondition: Matrix is an allocated and initialized
// Postcondition: Matrix is the same
// 				  Returns the number of non-zero elements in the Matrix
int getNNZ(Matrix* m)
{
	unsigned int count = 0;
	for(unsigned int x = 0; x < m->row; x++)
	{
		for(unsigned int i = 0; i < m->column; i++)
		{
			if(m->matrix[x][i] != 0)
				count++;
		}
	}

	return count;
}

// Function to compress a matrix to CSR format
// Precondition: Matrix is an allocated and initialized
// 				 The CSR struct has been allocated but the individual elements have not
// Postcondition: Matrix is left the same
// 				  CSR is allocated and stores the CSR compressed matrix
void compressMatrixToCSR(Matrix* m, CSR* csr)
{
	// Get and store the number of non-zero numbers
	csr->nnz = getNNZ(m);

	// Get and store the number of rows
	csr->row = m->row;

	// allocate the values array
	csr->value = malloc(csr->nnz * sizeof(int));
	csr->column_index = malloc(csr->nnz * sizeof(int));
	csr->row_pointer = malloc(m->row * sizeof(int));

	// Create pointers to iterator over each array
	int* value_temp_pointer = csr->value;
	int* column_temp_pointer = csr->column_index;
	int* row_temp_pointer = csr->row_pointer;

	// Create an int that keeps track of the index of the start of each row
	int row_pointer_count = 0;

	// Loop over the matrix
	// This is the outer, row loop
	for(unsigned int i = 0; i < m->row; i++)
	{

		// Flag for if it is the first non-zero in a row
		int flag = 1;

		// Loop over the matrix
		// This is the inner, column loop
		for(unsigned int x = 0; x < m->column; x++)
		{

			// Check if the number is a non-zero
			if(m->matrix[i][x] != 0)
			{
				//Assign the non-zero to the csr->value array
				*value_temp_pointer = m->matrix[i][x];

				// Move the csr->value iterator to the next position
				value_temp_pointer++;

				// Store the column numner in csr->column index
				// I would rather store without the + 1, as a normal index
				// but every example I saw had it stored this way
				*column_temp_pointer = x + 1;

				// Move the csr->column_index iterator to the next position
				column_temp_pointer++;

				// Keeps track of the which index of csr->value is the start of a new row
				// It is added first to start the index at 1
				row_pointer_count++;
				
				// Check if it the first non-zero in the row
				if(flag)
				{
					// Assign the index from csr->value for the start of this row to csr->row_pointer
					*row_temp_pointer = row_pointer_count;

					// Move the csr->row_pointer iterator to the next position
					row_temp_pointer++;

					// Set the flag showing we already have had a non-zero for this row
					flag = 0;
				}
			}
		}

		// Reset the flag for if we have had a non-zero for the start of a new row
		flag = 1;

	}

}

// Print a human readable CSR to the console
// Precondition: The CSR has been allocated and assigned
// Postcondition: The CSR is left unchanged
// 				  The CSR is printed to the console
void printCSR(CSR* csr)
{
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("Values:\n");
	for(unsigned int i = 0; i < csr->nnz; i++)
	{
		printf(" %d ", csr->value[i]);
	}

	printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("Column Indexes:\n");
	for(unsigned int i = 0; i < csr->nnz; i++)
	{
		printf(" %d ", csr->column_index[i]);
	}

	printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("Row Numbers:\n");
	for(unsigned int i = 0; i < csr->row; i++)
	{
		printf(" %d ", csr->row_pointer[i]);
	}
	printf("\n");
}

// Frees Internal Memory for Matrix
// Precondition: m is an allocated matrix structure
// Postcondition: Memory that m uses internally is freed
void freeMatrix(Matrix* m)
{
	for(unsigned int i = 0; i < m->row; i++)
	{
		free(m->matrix[i]);
	}
	free(m->matrix);
}

// Frees Internal Memory for CSR
// Precondition: csr is an allocated CSR structure
// Postcondition: Memory that csr uses internally is freed
void freeCSR(CSR* csr)
{
	free(csr->value);
	free(csr->column_index);
	free(csr->row_pointer);
}

int main(int argc, char** argv)
{
	if(argc == 2)
	{
		Matrix* m;
		m = malloc(sizeof(Matrix));
		readMatrix(m, argv[1]);
		printMatrix(m);

		CSR* csr = malloc(sizeof(CSR));
		compressMatrixToCSR(m,csr);
		printCSR(csr);

		freeMatrix(m);
		free(m);

		freeCSR(csr);
		free(csr);
	}
	else
	{
		printf("Usage:\n./out /home/...\n");
	}
	return 0;
}

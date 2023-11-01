#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define NUMBER_OF_ELEMENTS 100

int main()
{
	/* Initialize seed for pseudo random number generator */
	srand(time(0));

	int32_t *parr = calloc(NUMBER_OF_ELEMENTS, sizeof(int32_t));

	if (parr == NULL) /* Memory allocation fails */
	{
		printf("Couldn't allocate memory");
	}
	else  /* Memory allocation successful */
	{
		printf("Memory allocation successful\n");

		/* Generate and store random numbers */
		for (int i = 0; i < NUMBER_OF_ELEMENTS; i++)
		{
			parr[i] = rand();
		}

		printf("Before reallocation: %d %d %d\n", parr[3], parr[7], parr[11]);

		/* Reduce the array size to half */
		parr = realloc(parr, (NUMBER_OF_ELEMENTS / 2) * sizeof(int32_t));

		if (parr == NULL) /* Memory reallocation fails */
		{
			printf("Memory reallocation fails");
		}
		else  /* Memory reallocation successful */
		{
			printf("Memory reallocation successful\n");

			/* Proof that realloc keeps old array contents */
			printf("After  reallocation: %d %d %d\n", parr[3], parr[7], parr[11]);
		}
	}

	printf("\nsizeof parr = %d", sizeof(parr));

	free(parr);
	parr = NULL;

	return 0;
}


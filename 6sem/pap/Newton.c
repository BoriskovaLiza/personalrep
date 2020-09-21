#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//non-parallel version w/o breckets solving

int main(int argc, char *argv[]){

	if (argc != 3){
		printf("Please, write an input and output filenames\n");
		exit(1);
	}

	time_t start, end;
	start = time (&start);

	char* filename = argv[1];
	FILE* points;
	if ((points = fopen(filename, "r"))==NULL) {
		printf("Cannot open input file.\n");
		exit(2);
	}

	filename = argv[2];
	FILE* interp;
	if ((interp = fopen(filename, "w"))==NULL) {
		printf("Cannot open output file.\n");
		exit(3);
	}
	
	int N = 0;
	double *X, *Y, *B;

	fscanf(points, "%d", &N);

	if((X = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for X\n");
		exit(-1);
    	}
	if((Y = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for Y\n");
		exit(-1);
    	}
	if((B = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for B\n");
		exit(-1);
    	}

	int i = 0;//up to N-1
	int j = 0;

	for(; i < N; i++){ fscanf(points, "%le %le", &X[i], &Y[i]); }
	fclose(points);
	B[0] = Y[0];
	//printf("\n----- j = %d -----", j);	

	for (int j = 1; j < N; j++){
		//printf("\n----- j = %d -----", j);
		for (i = 0; i + j < N; i++){
			B[i+j] = (Y[j+i]-Y[j+i-1])/(X[i+j]-X[i]);
			printf(" %d - B[%d] = %.2f = (%d - %d)\n", j, i+j, B[i+j],  j+i, j+i-1);
		}
		for (i = 0; i + j < N; i++){
			Y[i+j] = B[i+j];
		}
	}

	printf("\n\n-> N(x) = ");
	for(i = 0; i < N-1; i++){ if (B[i] != 0.0) printf("%.2le*[x]^%d + ", B[i], i); }
	printf("%.2le*[x]^%d\n\n-> Worktime: ", B[i], i);

	end = time (&end);
	printf("%e\n\n", difftime(end, start));

	for(i = 0; i < N-1; i++){ fprintf(interp, "%le ", B[i]); }
	fprintf(interp, "%le\n", B[i]);
	fprintf(interp, "%e\n", difftime(end, start));

	free(B);
	free(Y);
	free(X);
	fclose(interp);

	return 0;
}

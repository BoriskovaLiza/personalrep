#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]){
    
    double *u, *unew, delta, maxdelta;
    double eps=1.e-7;
    double h, tau;
    
    int N;
    int i;
    
    FILE *ff;
    
    if(argc !=2 ){
	printf("Usage: exefile npoints\n");
	exit(-1);
    }
    
    N = atoi(argv[1]);
    if (N == 0) {
	printf("Set N to 1000\n");
	N =1000;
    }
    
    if((u = malloc((N)*sizeof(double))) == NULL){
	printf("Can't allocate memory for u\n");
	exit(-1);
    }

    if((unew = malloc((N)*sizeof(double))) == NULL){
	printf("Can't allocate memory for unew\n");
	free(u);
	exit(-1);
    }
    
    // begin & bound values
    
    for(i=0; i<N; i++){
	u[i] = 0;
    }
    
    unew[0] = u[0] = 1.0;
    unew[N-1] = 0;
    
    h = 1.0/N;
    tau = 0.5*(h*h);
        
    while(1){
	for (i=1; i<N-1; i++){
	   unew[i] = u[i]+(tau/(h*h))*(u[i-1]-2*u[i]+u[i+1]);
	}   	
	
        maxdelta = 0;
        for (i=1; i<N-1; i++){
            delta = fabs(unew[i]-u[i]);
            if(delta > maxdelta) maxdelta = delta;
        }
        if(maxdelta < eps) break;
        
        for(i = 1; i < N-1; i++){
    	    u[i] = unew[i];
        }
    }
    
    if ((ff = fopen("resfile_not_paral", "w+")) == NULL){
       printf("Can't open file\n"); free(u); free(unew);
       exit(-1);
    }
    
    for(i=0; i<N; i++)
       fprintf(ff, "%f ", unew[i]);
       
    fclose(ff); free(u); free(unew);
    return 0;  
}      

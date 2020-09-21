#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <jpeglib.h>

extern int count_meancolor(struct jpeg_decompress_struct* cinfo, int begin, int end, unsigned long* meancolor);
extern int get_colorspace(struct jpeg_decompress_struct* cinfo, char* colorspace, int* components, int* width, int* height);

//parallel version of counting mean colour of an image
 
int main(int argc, char *argv[]) {

	int st;
	st = MPI_Init(&argc, &argv);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	int size = 0;
	st = MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 2);
	}
	int rank = 0;
	st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 3);
	}
	
	int i = 0;
	int begin, end;

	const char* filename = argv[1];
	char colorspace[15];
	int components, width, height;

	time_t start, stop;
	if (rank == 0) { start = time (&start); }

	FILE *infile = fopen(filename, "rb");
	if (!infile) {
		if (rank == 0) {printf("Error opening jpeg file %s\n!", filename);}
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 5);
		}
		return -1;
	}
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	if (get_colorspace(&cinfo, colorspace, &components, &width, &height) != 0){
		MPI_Abort(MPI_COMM_WORLD, 6);
		return -1;
	}

	printf("Components = %d\n", components);

	if (rank == 0) {
		printf("	File name: %s\n", filename);
		printf("	Width: %d\n	Height: %d\n", width, height);
	}

	unsigned long* meancolor;
	if((meancolor = (unsigned long*)malloc((components)*sizeof(unsigned long))) == NULL){
		printf("Can't allocate memory for meancolor components\n");
		MPI_Abort(MPI_COMM_WORLD, 7);
		return -1;
    	}

	begin = (int)height/size*rank;
	end = (int)height/size*(rank+1);
	if (rank == size - 1){ end = (int)height; }
	
	printf("everything until meancolour counting is fine\n");
	if (count_meancolor(&cinfo, begin, end, meancolor) != 0){
		printf("Can't count meancolor\n");
		MPI_Abort(MPI_COMM_WORLD, 8);
		return -1;
	}
	printf("meancolor counted\n");
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	if (rank == 0){ st = MPI_Reduce(MPI_IN_PLACE, meancolor, components, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD); }
	else { st = MPI_Reduce(meancolor, meancolor, components, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD); }
	
	printf("value reduced\n");
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 9);
		return -1;
	}
	
	if (rank == 0) { stop = time (&stop); }

	if (rank == 0) {
		printf("	Meancolor: %d", components);
		for (i = 0; i < components; i++){
			printf("%.2f: ", (float)meancolor[i]/(width*height));
		}
		printf("in %s colorspace\n", colorspace);
		printf("	Worktime: %.2e\n", difftime(stop, start));
	}

	fclose(infile);
	free(meancolor);

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 10);
	}

	return 0;
}

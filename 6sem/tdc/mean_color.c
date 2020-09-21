#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <time.h>

//non-parallel version of counting mean colour of an image

const char *cspaces[] = {"UNKNOWN",
			"GRAYSCALE",
			"RGB",
			"YCbCr",
			"CMYK",
			"YCCK"};
 
int main(int argc, char *argv[]) {

	if (argc != 2){
		printf("Please, write an input jpeg file\n");
		exit(1);
	}

	int i = 0;
	time_t start, end;
	start = time (&start);

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	const char* filename = argv[1];
	FILE *infile = fopen(filename, "rb");
	if (!infile) {
		printf("Error opening jpeg file %s\n!", filename );
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned long meancolor[cinfo.num_components];
	for (i = 0; i < cinfo.num_components; i++){ meancolor[i] = 0; }
	printf("	File name: %s\n", filename);
	printf("	Width: %d\n	Height: %d\n", cinfo.output_width, cinfo.output_height);


	row_pointer[0] = (unsigned char*)malloc(cinfo.output_width*cinfo.num_components);

	while(cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for(i = 0; i < cinfo.image_width*cinfo.num_components; i++)
			meancolor[i%cinfo.num_components] += row_pointer[0][i];
	}
	end = time (&end);

	printf("	Meancolor: ");
	for (i = 0; i < cinfo.num_components; i++){
		printf("%.2f: ", (float)meancolor[i]/(cinfo.output_width*cinfo.output_height));
	}
	printf("in %s colorspace\n", cspaces[cinfo.out_color_space]);
	printf("	Worktime: %.2e\n", difftime(end, start));

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
	fclose(infile);

	return 0;
}

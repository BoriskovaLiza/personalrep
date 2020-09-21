#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jpeglib.h>

const char *cspaces[] = {"UNKNOWN",
			"GRAYSCALE",
			"RGB",
			"YCbCr",
			"CMYK",
			"YCCK"};

int count_meancolor(struct jpeg_decompress_struct* cinfo, int begin, int end, unsigned long* meancolor){

	JSAMPROW row_pointer[1];

	for (int i = 0; i < cinfo->num_components; i++){ meancolor[i] = 0; }
	row_pointer[0] = (unsigned char*)malloc(cinfo->output_width * cinfo->num_components);

	jpeg_skip_scanlines(cinfo, begin); 
	while(cinfo->output_scanline < end) {
		jpeg_read_scanlines(cinfo, row_pointer, 1);
		for(int i = 0; i < cinfo->image_width*cinfo->num_components; i++){
			meancolor[i % cinfo->num_components] += row_pointer[0][i];
		}
	}
	
	jpeg_skip_scanlines(cinfo, cinfo->image_height - end);
	free(row_pointer[0]);
	return 0;
}

int get_colorspace(struct jpeg_decompress_struct* cinfo, char* colorspace, int* components, int* width, int* height){

	*components = cinfo->num_components;
	*width = (int)cinfo->image_width;
	*height = (int)cinfo->image_height;
	
	strcpy(colorspace, cspaces[cinfo->out_color_space]);

	return 0;

}

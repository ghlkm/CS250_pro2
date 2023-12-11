// Version 0.1: 2020-05-18

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <ctime>
#include <chrono>

inline int idx(int x, int y, int width) {
	return y*width+x;
}

void 
step_naive(float* temp, float* temp2, float* conduct, int width, int height) {
	for ( int i = 1; i < width-1; i++ ) {
		for ( int j = 1; j < height-1; j++ ) {
			temp2[idx(i,j,width)] = 
				temp[idx(i,j,width)] +
				(
					(temp[idx(i-1,j,width)] 
					- temp[idx(i,j,width)]) *conduct[idx(i-1,j,width)]

					+ (temp[idx(i+1,j,width)] 
					- temp[idx(i,j,width)]) *conduct[idx(i+1,j,width)]


					+ (temp[idx(i,j-1,width)]
					- temp[idx(i,j,width)]) *conduct[idx(i,j-1,width)]
					+ (temp[idx(i,j+1,width)] 
					- temp[idx(i,j,width)]) *conduct[idx(i,j+1,width)]

				)*0.2;
		}
	}
	return;
}

extern void stencil_optimized(float* temp, float* conduct, int width, int height, int substeps);



int
main (int argc, char** argv) {
	printf( "usage: ./%s [steps] [initial state file] [naive?]\n", argv[0] );


	int steps = 8192;
	if ( argc >= 2 ) steps = atoi(argv[1]);
	
	
	const char* filename = "init.dat";
	if ( argc >= 3 ) {
		filename = argv[2];
	}

	bool isnaive = true;
	if ( argc >= 4 ) {
		if ( argv[3][0] == 'N' || argv[3][0] == 'n' ) isnaive = false;
	}

	FILE* fin = fopen(filename, "rb");
    FILE* fin2 = fopen("init2.dat", "rb");

	if ( !fin ) {
		printf( "Input file %s not found!\n", filename );
		exit(1);
	}



	int width;
	int height;
	if ( !fread(&width, sizeof(int), 1, fin) || !fread(&height, sizeof(int), 1, fin) ) {
		printf( "Input file read failed\n" );
		exit(1);
	}
	printf( "Reading data file width, height = %d %d\n", width, height );
	float* temp = (float*)malloc(sizeof(float)*width*height);
	float* temp2 = (float*)malloc(sizeof(float)*width*height);
	float* conduct = (float*)malloc(sizeof(float)*width*height);

	if ( fread(temp, sizeof(float), width*height, fin) != (size_t)(width*height)
		|| fread(conduct, sizeof(float), width*height, fin2) != (size_t)(width*height) ) {

		printf( "Input file read failed\n" );
		exit(1);
	 }
	fclose(fin);
    fclose(fin2);
//	FILE* fout = fopen("init1.dat", "wb");
//	fwrite(&width, sizeof(int), 1, fout);
//	fwrite(&height, sizeof(int), 1, fout);
//	fwrite(temp, sizeof(float), width*height, fout);
//    fclose(fout);
//    FILE* fout2 = fopen("init2.dat", "wb");
//    fwrite(conduct, sizeof(float), width*height, fout2);
//    fclose(fout2);
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point now;
	std::chrono::microseconds duration_micro;

	printf( "Starting stencil calculation\n" );
	start = std::chrono::high_resolution_clock::now();

	if ( isnaive ) {
		for ( int i = 0; i < steps; i++ ) {
			step_naive(temp, temp2, conduct, width, height);
			if ( i < steps - 1) {
				float* t = temp;
				temp = temp2;
				temp2 = t;
			}
		}
	} else {
		stencil_optimized(temp, conduct, width, height, steps);
	}
	int ii = 0;


	now = std::chrono::high_resolution_clock::now();
	duration_micro = std::chrono::duration_cast<std::chrono::microseconds> (now-start);
	printf( "Done : %f s\n", 0.000001f*duration_micro.count() );
	printf( "Stencil calculation done\n" );

	FILE* fout = fopen("output.dat", "wb");
	fwrite(&width, sizeof(int), 1, fout);
	fwrite(&height, sizeof(int), 1, fout);
	fwrite(temp, sizeof(float), width*height, fout);
	fwrite(conduct, sizeof(float), width*height, fout);
	fclose(fout);

	printf( "All done! Exiting...\n" );
}

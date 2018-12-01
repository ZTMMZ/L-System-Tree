#include <stdio.h>

unsigned char* TexTo2DTexture(char *filename, int *width, int *height) {
	FILE *fp;
	unsigned char *texture, *tp;
	int s, t, nums, numt;

	fopen_s(&fp, filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open Tex file '%s'\n", filename);
		return NULL;
	}

	nums = fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	numt = fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	fgetc(fp);

	texture = new unsigned char[4 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\b");
		return NULL;
	}
	
	for (t = 0, tp = texture; t < numt; t++)
	{
		for (s = 0; s < nums; s++, tp += 4)
		{
			*(tp + 0) = fgetc(fp);
			*(tp + 1) = fgetc(fp);
			*(tp + 2) = fgetc(fp);
			*(tp + 3) = fgetc(fp);
		}
	}
	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

unsigned char* TexTo3DTexture(char *filename, int *width, int *height, int *depth) {
	FILE *fp;
	unsigned char *texture, *tp;
	int s, t, p, nums, numt, nump;

	fopen_s(&fp, filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open Tex file '%s'\n", filename);
		return NULL;
	}

	nums = fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	numt = fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	nump = fgetc(fp);
	fgetc(fp);
	fgetc(fp);
	fgetc(fp);

	texture = new unsigned char[4 * nums * numt * nump];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\b");
		return NULL;
	}
	for (p = 0, tp = texture; p < nump; p++) {
		for (t = 0; t < numt; t++) {
			for (s = 0; s < nums; s++, tp += 4) {
				*(tp + 0) = fgetc(fp);
				*(tp + 1) = fgetc(fp);
				*(tp + 2) = fgetc(fp);
				*(tp + 3) = fgetc(fp);
			}
		}
	}
	fclose(fp);

	*width = nums;
	*height = numt;
	*depth = nump;

	return texture;
}

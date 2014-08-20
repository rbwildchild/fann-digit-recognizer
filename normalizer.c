#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int count_commas(char* string) {
	int count = 0;
	int i = 0;
	for (i = 0; i < strlen(string); i++)
		if (string[i] == ',') count++;
	return count;
}

void setInput(int *input, char* line, int ni, int skipFirstCol){
	int i;
	char* token;
	char* line_copy = (char*) malloc((strlen(line) + 1) * sizeof(char));
	strcpy(line_copy, line);
	line_copy[strlen(line)] = '\0';
	int offset = skipFirstCol ? 1 : 0;
	for (token = strtok(line_copy, ","), i = 0;token && *token && i < ni + offset;token = strtok(NULL, ","), i++){
		if(skipFirstCol && i == 0)
			continue;
		int value = atoi(token);
		input[i-offset] = value;
	}
	free(line_copy);
}

void calculateStats(float *mean, float *sigma, FILE * fr, int m, int ni, int skipFirstCol){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int min[ni], max[ni];
	int i;
	int count = 0;
	for(i = 0;i < ni;i++){
		min[i] = INT_MAX;
		max[i] = INT_MIN;
	}

	while ((read = getline(&line, &len, fr)) != -1) {
		if(count > 0){
			//int *input = (int*)malloc(sizeof(int) * ni);
			int input[ni];
			setInput(input, line, ni, skipFirstCol);
			for(i = 0;i < ni;i++){
				mean[i] += (float) input[i];
				if(input[i] < min[i])
					min[i] = input[i];
				if(input[i] > max[i])
					max[i] = input[i];
			}
		}
		count++;
	}
	for(i = 0;i < ni;i++){
		sigma[i] = max[i] - min[i];
		mean[i] /= m;
	}
	rewind(fr);
	free(line);
	/*printf("Mean\n");
	for(i = 0;i < ni; i++)
		printf("%f ", mean[i]);
	printf("\nSigma\n");
	for(i = 0;i < ni; i++)
		printf("%f ", sigma[i]);
	printf("\n");*/
}

char* getNormalizedValues(char* line, int ni, float *mean, float *sigma, int skipFirstCol){
	if(skipFirstCol)
		ni++;
	int input[ni];
	float floatValues[ni];
	char charValues[ni][50];
	int offset = skipFirstCol ? 1 : 0;
	int stringSize = 0;
	int i;
	char* normValues = NULL;

	setInput(input, line, ni, 0);
	for(i = 0;i < ni;i++){
		if(skipFirstCol && i == 0)
			floatValues[i] = input[i];
		else{
			float iSigma = sigma[i - offset] != 0 ? sigma[i - offset] : 1;
			floatValues[i] = ((float)input[i] - mean[i - offset]) / iSigma;
		}
	}
	
	for(i = 0;i < ni;i++){
		if(skipFirstCol && i == 0)
			sprintf(charValues[i], "%d", (int)floatValues[i]);
		else
			sprintf(charValues[i], "%f", floatValues[i]);
		stringSize += strlen(charValues[i]) + 1;
	}
	normValues = (char *)malloc(stringSize+10);
	strcpy(normValues, "");
	for(i = 0;i < ni;i++){
		strcat(normValues, charValues[i]);
		if(i < ni -1)
			strcat(normValues, ",");
	}	
	normValues[stringSize] = '\0';
	return normValues;
}

int main(int argc, char **argv){
	FILE * fr;
	FILE * fw;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char* fileInput = "train.csv"; 
	int lines_between_reports = 4000;
	int train = 1;
	
	char* fileOutput; 

	if(argc == 4){
		fileInput = argv[1]; 
		lines_between_reports = atoi(argv[2]);
		train = atoi(argv[3]);
	}

	const char *ptr = strchr(fileInput, '.');
	if(ptr) {
		int index = ptr - fileInput;
		fileOutput = (char *) malloc((index + 10) * sizeof(char));
		memcpy(fileOutput, fileInput, index * sizeof(char));
		fileOutput[index] = '_';
		fileOutput[index+1] = 'n';fileOutput[index+2] = 'o';
		fileOutput[index+3] = 'r';fileOutput[index+4] = 'm';
		fileOutput[index+5] = '.';
		fileOutput[index+6] = 'c';fileOutput[index+7] = 's';
		fileOutput[index+8] = 'v';fileOutput[index+9] = '\0';
	}
	else{
		size_t size = strlen(fileInput);
		fileOutput = (char *) malloc((size + 1) * sizeof(char));
		strcpy(fileOutput, fileInput);
		fileOutput[size] = '\0';
	}

	//printf("File output %s\n", fileOutput);

	fr = fopen(fileInput, "r");
	fw = fopen(fileOutput, "w");
	if (fr == NULL || fw == NULL)
		exit(EXIT_FAILURE);

	int count = 0, m = 0, ni = 0;

	while ((read = getline(&line, &len, fr)) != -1) {
		if(count == 0){
			ni = count_commas(line);
			if(!train)
				ni++;
		}
		count++;
	}

	m = count - 1;
	count = 0;
	float *mean = (float*)malloc(sizeof(float)*ni);
	float *sigma = (float*)malloc(sizeof(float)*ni);
	char * normValues;
	rewind(fr);

	printf("Number of training pairs %d, number of inputs %d\n", m, ni);
	
	printf("Calculating stats\n");
	calculateStats(mean, sigma, fr, m, ni, train);
	printf("Calculated stats!\n");

	time_t ltime;
	ltime=time(NULL);
	
	while ((read = getline(&line, &len, fr)) != -1) {
		if(count > 0){
			if(((count-1) % lines_between_reports) == 0){
				printf("Line: %d at %s", count, asctime(localtime(&ltime)));
			}
			char * normValues = getNormalizedValues(line, ni, mean, sigma, train);
			fprintf(fw, "%s\n", normValues);
			if(normValues)
				free(normValues);
		}
		else
			fprintf(fw, "%s", line);
		count++;
	}

	if(line)
		free(line);
	free(fileOutput);
	free(mean);
	free(sigma);
	fclose(fr);
	fclose(fw);
	exit(EXIT_SUCCESS);
}

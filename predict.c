#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "floatfann.h"

int count_commas(char* string) {
	int count = 0;
	int i = 0;
	for (i = 0; i < strlen(string); i++)
		if (string[i] == ',') count++;
	return count;
}

void setInput(fann_type *input, char* line, int ni){
	int i;
	char* token;
	char* line_copy = (char*) malloc((strlen(line) + 1) * sizeof(char));
	strcpy(line_copy, line);
	line_copy[strlen(line)] = '\0';
	for (token = strtok(line_copy, ","), i = 0;token && *token && i < ni;token = strtok(NULL, ","), i++){
		int value = atoi(token);
		input[i] = value;
	}
	free(line_copy);
}

int getMaxOutput(fann_type *calc_out, int n){
	fann_type maxValue = 0;
	int maxOutput = 0;
	int i;
	for(i = 0;i < n;i++){
		if(maxValue < calc_out[i]){
			maxValue = calc_out[i];
			maxOutput = i;
		}
	}
	return maxOutput;
}

int main(int argc, char **argv){
	FILE * fr;
	char* fileInput = "test.csv";
	char * line = NULL;
	ssize_t read;
	size_t len = 0;
	int maxLines = -1;
	char* neurons = "100";
	char* netFile = "nets/digit_float_100.net";

	fann_type *calc_out;

	if(argc == 4){
		fileInput = argv[1]; 
		maxLines = atoi(argv[2]);
		neurons = argv[3];
		netFile = (char*)malloc(strlen(netFile)+strlen(neurons)-2);
		strcpy(netFile,"nets/digit_float_");
		int i;
		for(i = 17;i < 17 + strlen(neurons);i++)
			netFile[i] = neurons[i - 17];
		netFile[i] = '.';netFile[i+1] = 'n';
		netFile[i+2] = 'e';netFile[i+3] = 't';
		netFile[i+4] = '\0';
	}
	
	//printf("File: %s\n",netFile);
	printf("ImageId,label\n");

	fr = fopen(fileInput, "r");
	int count = 0, ni = 0;
	while ((read = getline(&line, &len, fr)) != -1) {
		ni = count_commas(line) + 1;
		break;
	}
	fann_type *input = (fann_type*)malloc(sizeof(fann_type)*ni);
	count = 0;
	rewind(fr);

	struct fann *ann = fann_create_from_file(netFile);

	while ((read = getline(&line, &len, fr)) != -1) {
		if(count > 0){
			setInput(input, line, ni);
			/*int i;
			for(i = 0;i < ni; i++)
				printf("%0.0f ", input[i]);
			printf("\n");*/
			calc_out = fann_run(ann, input);
			//int n = sizeof(calc_out) / sizeof(calc_out[0]);
			int output = getMaxOutput(calc_out, 10);
			/*if((count -1) % 10 == 0)
				printf("\n");
			printf("%d ", output);*/
			printf("%d,%d\n", count,output);
		}
		if(maxLines > 0 && count == maxLines)
			break;
		count++;
	}
	//printf("\n");
	fann_destroy(ann);
	return 0;
}

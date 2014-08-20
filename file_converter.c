#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* getLabel(char* line){
	const char *ptr = strchr(line, ',');
	if(ptr) {
		int index = ptr - line;
		char* subst = (char *) malloc((index + 1) * sizeof(char));
		memcpy(subst, line, index * sizeof(char));
		subst[index] = '\0';
		return subst;
	}
	return NULL;
}

char* getValues(char* line){
	const char *ptr = strchr(line, ',');
	if(ptr) {
		int index = ptr - line + 1;
		size_t size = strlen(line) - index; 
		char* subst = (char *) malloc((size + 1) * sizeof(char));
		memcpy(subst, &line[index], size * sizeof(char));
		subst[size] = '\0';
		char *pos;
		for(pos = subst;pos < subst + strlen(subst);pos+=sizeof(char)){
			if(*pos == ',')
				*pos = ' ';
		}
		return subst;
	}
	return NULL;
}

int count_commas(char* string) {
	int count = 0;
	int i = 0;
	for (i = 0; i < strlen(string); i++)
		if (string[i] == ',') count++;
	return count;
}

char * getOutput(int num) {
	char* output = (char *)malloc(20 * sizeof(char));
	strcpy(output, "0 0 0 0 0 0 0 0 0 0");
	output[2*num] = '1';
	return output;
}

int main(int argc, char **argv){
	FILE * fr;
	FILE * fw;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char* fileInput = "train.csv"; 
	int lines_between_reports = 4000;
	
	char* fileOutput; 

	if(argc == 3){
		fileInput = argv[1]; 
		lines_between_reports = atoi(argv[2]);
	}

	const char *ptr = strchr(fileInput, '.');
	if(ptr) {
		int index = ptr - fileInput;
		fileOutput = (char *) malloc((index + 11) * sizeof(char));
		memcpy(fileOutput, fileInput, index * sizeof(char));
		fileOutput[index] = '_';
		fileOutput[index+1] = 'f';fileOutput[index+2] = 'o';
		fileOutput[index+3] = 'r';fileOutput[index+4] = 'm';
		fileOutput[index+5] = '.';
		fileOutput[index+6] = 'd';fileOutput[index+7] = 'a';
		fileOutput[index+8] = 't';fileOutput[index+9] = 'a';
		fileOutput[index+10] = '\0';
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
	const int no = 10;

	while ((read = getline(&line, &len, fr)) != -1) {
		if(count == 0){
			ni = count_commas(line);
		}
		count++;
	}

	m = count - 1;
	count = 0;
	rewind(fr);

	printf("Number of training pairs %d, number of inputs %d, number of outputs %d\n", m, ni, no);
	fprintf(fw, "%d %d %d\n", m, ni, no);

	time_t ltime;
	ltime=time(NULL);
	while ((read = getline(&line, &len, fr)) != -1) {
		if(count > 0){
			char * label = getLabel(line);
			char * output = getOutput(atoi(label));
			char * values = getValues(line);
			fprintf(fw, "%s", values);
			fprintf(fw, "%s\n", output);
			if(((count-1) % lines_between_reports) == 0){
				printf("Line: %d at %s", count, asctime(localtime(&ltime)));
				//printf("Label: %s = %s\n", label, output);
				//printf("Values: %s", values);
			}
			free(label);
			free(output);
			free(values);
		}
		count++;
	}
	if (line)
		free(line);
	free(fileOutput);
	fclose(fr);
	fclose(fw);
	exit(EXIT_SUCCESS);
}

#include "fann.h"
#include "floatfann.h"

int main(int argc, char **argv)
{
	const unsigned int num_input = 784;
	const unsigned int num_output = 10;
	const unsigned int num_layers = 3;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = 500000;

	char* fileInput = "train_form.data"; 
	unsigned int epochs_between_reports = 10000;
	unsigned int num_neurons_hidden = 50;

	if(argc == 4){
		fileInput = argv[1];
		num_neurons_hidden = atoi(argv[2]);
		epochs_between_reports = atoi(argv[3]);
	}

	fann_type *calc_out;
	fann_type input[2];

	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, fileInput, max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "digit_float.net");

	fann_destroy(ann);

	return 0;
}

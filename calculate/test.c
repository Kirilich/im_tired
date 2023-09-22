#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RESULT_STRING_LENGTH 32
#define M_PI 3.14159265358
#define array_length(x) (sizeof(x) / sizeof(x[0]))
#define error(...) (fprintf(stderr, __VA_ARGS__))

struct interval_t {
	double left_border;
	double right_border;
};

void free_square(char **square, unsigned int experiments_count);

static double rectangle_calculate(struct interval_t partition) {
	return (partition.right_border - partition.left_border) * //
	 sin((partition.left_border + partition.right_border) / 2);
}

static double simpsons_calculate(struct interval_t partition) {
	return (partition.right_border - partition.left_border) / 6 * (sin(partition.left_border) + 4 *//
            sin((partition.left_border + partition.right_border) / 2) + sin(partition.right_border));
}

static double integrate(struct interval_t interval_s, unsigned int partition_size, double (*method)(interval_t)) {
        double integral_value = 0.0;
        double part_length = (interval_s.right_border - interval_s.left_border) / partition_size;
        double part_left_border = interval_s.left_border;
        for (unsigned int i = 0; i < partition_size; ++i) {
                double part_right_border = part_left_border + part_length;
                struct interval_t partition_s = {part_left_border, part_right_border};
                integral_value += method(partition_s);
                part_left_border = part_right_border;
        }
        return integral_value;
}

static char **calculate_integrals(struct interval_t interval_s, unsigned int experiments_count, const unsigned int *partition_sizes) {
	char **square = (char **)malloc(sizeof(char *) * experiments_count); //создаем место в памяти для подсчитанной пощади
	if (!square) {
		return NULL;
	}
	for (unsigned int i = 0; i < experiments_count; ++i) {
		double integral_rectangle = integrate(interval_s, partition_sizes[i], rectangle_calculate);
		double integral_simpson = integrate(interval_s, partition_sizes[i], simpsons_calculate);
		square[i] = (char *)malloc(sizeof(char) * MAX_RESULT_STRING_LENGTH);
		if (!square[i]) {
			free_square(square, i);
			error("Cannot allocate memory for result string in %d experiment\n", i);
			return NULL;
		}
		if (!sprintf(square[i], "%d %.5f %.5f", partition_sizes[i], integral_rectangle, integral_simpson)) {
			free_square(square, i + 1);
			error("Cannot write results to string in %d experiment\n", i);
			return NULL;
		}
	}
	return square;
}

void free_square(char **square, unsigned int experiments_count) {
	for (unsigned int i = 0; i < experiments_count; ++i) {
		free(square[i]);
	}
	free(square);
}

static int read_interval(struct interval_t *interval_s) {
	if (printf("Enter interval's left border: ") < 0) {
		error("Cannot wtite to stdout to stdout");
		return -1;
	}
	if (scanf("%lf", &(interval_s->left_border)) != 1) {
		error("Cannot read interval's left border");
		return -1;
	}
	if (interval_s->left_border < 0) {
		error("Left border of the interval must be greater than or equal to 0");
		return -1;
	}
	if (printf("Enter interval's right border: ") < 0) {
		error("Cannot wtite to stdout");
		return -1;
	}
	if (scanf("%lf", &(interval_s->right_border)) != 1) {
		error("Cannot read interval's right border");
		return -1;
	}
	if (interval_s->right_border > M_PI) {
		error("Right border of the interval must be less than or equal to pi");
		return -1;
	}
	if (interval_s->left_border > interval_s->right_border) {
		error("Right border of the interval must be greater than left");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) {
	struct interval_t interval_s = {0.0, 0.0};
	if (read_interval(&interval_s)) {
		return 1;
	}
	unsigned int partition_sizes[] = {5, 10, 20, 100, 500, 1000}; //размер деления промежутка
	size_t experiments_count = array_length(partition_sizes); //кол-во показываемых промежутков
	char **square = calculate_integrals(interval_s, experiments_count, partition_sizes);
	if (!square) {
		return 1;
	}
	for (unsigned int i = 0; i < experiments_count; ++i) {
		if (printf("%s\n", square[i]) < 0) {
			error("Cannot wtite %d result to stdout", i);
			break;
		}
	}
	free_square(square, experiments_count);
	return 0;
}

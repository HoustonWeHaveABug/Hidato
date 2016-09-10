#include <stdio.h>
#include <stdlib.h>

#define PIPE_W 1UL
#define PIPE_WN 2UL
#define PIPE_N 4UL
#define PIPE_NE 8UL
#define PIPE_E 128UL
#define PIPE_ES 64UL
#define PIPE_S 32UL
#define PIPE_SW 16UL
#define CELL_WHITE '.'
#define CELL_BLACK 'x'
#define CALLS_MAX 3
#define LINKS_MAX 8

typedef struct value_s value_t;
typedef struct cell_s cell_t;

struct value_s {
	unsigned long number;
	cell_t *cell;
	value_t *last;
	value_t *next;
};

struct cell_s {
	unsigned long pipes;
	value_t *value;
};

typedef enum {
	CALL_CHOOSE,
	CALL_UNROLL,
	CALL_ROLL
}
call_t;

typedef struct {
	cell_t *cell;
	unsigned long pipes;
}
unroll_t;

typedef struct {
	value_t *value;
	cell_t *cell;
	unroll_t *unroll;
}
roll_t;

void set_value(value_t *, unsigned long);
void chain_value(value_t *, value_t *, value_t *);
void set_cell_pipes(cell_t *, unsigned long);
void hidato(void);
void stack_call(call_t *, call_t);
void perform_call(call_t *);
void perform_choose(void);
void print_cell(cell_t *, int);
unsigned long set_choices(value_t *, cell_t *[]);
unsigned long set_links(cell_t *, cell_t *[]);
void set_choice(cell_t **, cell_t *);
void stack_unroll(unroll_t *, cell_t *);
void perform_unroll(unroll_t *);
void remove_cell_value(cell_t *, unsigned long);
void add_pipes(cell_t *, unsigned long);
void add_pipe(cell_t *, unsigned long, cell_t *);
void stack_roll(roll_t *, value_t *, cell_t *, unroll_t *);
void perform_roll(roll_t *);
unsigned long add_cell_value(cell_t *, value_t *);
unsigned long remove_pipes(cell_t *);
unsigned long remove_pipe(cell_t *, unsigned long, cell_t *);
void set_unroll_pipes(unroll_t *, unsigned long);

int digits_max;
unsigned long values_n, rows_n, columns_n, nodes_n, solutions_n, stack_calls_n, stack_unrolls_n, stack_rolls_n;
value_t *values, *value_black;
cell_t *cells;
call_t *stack_calls;
unroll_t *stack_unrolls;
roll_t *stack_rolls;

int main(void) {
unsigned long cells_n, h_cells_n, w_cells_n, i, j;
value_t *value;
cell_t *cell;
	if (scanf("%lu", &values_n) != 1) {
		fprintf(stderr, "Invalid number of values\n");
		return EXIT_FAILURE;
	}
	digits_max = 1;
	i = values_n/10;
	while (i) {
		digits_max++;
		i /= 10;
	}
	values = malloc(sizeof(value_t)*(values_n+1));
	if (!values) {
		fprintf(stderr, "Could not allocate memory for values\n");
		free(cells);
		return EXIT_FAILURE;
	}
	for (i = 0; i < values_n; i++) {
		set_value(values+i, i+1);
	}
	value_black = values+i;
	chain_value(values, value_black, values+1);
	for (value = values+1; value < value_black; value++) {
		chain_value(value, value-1, value+1);
	}
	chain_value(value, value-1, values);
	if (scanf("%lu%lu", &rows_n, &columns_n) != 2 || !rows_n || !columns_n) {
		fprintf(stderr, "Invalid grid size\n");
		free(values);
		return EXIT_FAILURE;
	}
	fgetc(stdin);
	cells_n = rows_n*columns_n;
	cells = malloc(sizeof(cell_t)*cells_n);
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		free(values);
		return EXIT_FAILURE;
	}
	if (rows_n > 1) {
		if (columns_n > 1) {
			set_cell_pipes(cells, PIPE_E+PIPE_ES+PIPE_S);
			cell = cells+1;
			for (j = 1; j < columns_n-1; j++) {
				set_cell_pipes(cell++, PIPE_W+PIPE_E+PIPE_ES+PIPE_S+PIPE_SW);
			}
			set_cell_pipes(cell++, PIPE_W+PIPE_S+PIPE_SW);
			for (i = 1; i < rows_n-1; i++) {
				set_cell_pipes(cell++, PIPE_N+PIPE_NE+PIPE_E+PIPE_ES+PIPE_S);
				for (j = 1; j < columns_n-1; j++) {
					set_cell_pipes(cell++, PIPE_W+PIPE_WN+PIPE_N+PIPE_NE+PIPE_E+PIPE_ES+PIPE_S+PIPE_SW);
				}
				set_cell_pipes(cell++, PIPE_W+PIPE_WN+PIPE_N+PIPE_S+PIPE_SW);
			}
			set_cell_pipes(cell++, PIPE_N+PIPE_NE+PIPE_E);
			for (j = 1; j < columns_n-1; j++) {
				set_cell_pipes(cell++, PIPE_W+PIPE_WN+PIPE_N+PIPE_NE+PIPE_E);
			}
			set_cell_pipes(cell, PIPE_W+PIPE_WN+PIPE_N);
		}
		else {
			set_cell_pipes(cells, PIPE_S);
			for (i = 1; i < rows_n-1; i++) {
				set_cell_pipes(cells+i, PIPE_N+PIPE_S);
			}
			set_cell_pipes(cells+i, PIPE_N);
		}
	}
	else {
		if (columns_n > 1) {
			set_cell_pipes(cells, PIPE_E);
			for (j = 1; j < columns_n-1; j++) {
				set_cell_pipes(cells+j, PIPE_W+PIPE_E);
			}
			set_cell_pipes(cells+j, PIPE_W);
		}
		else {
			set_cell_pipes(cells, 0UL);
		}
	}
	h_cells_n = 0;
	w_cells_n = 0;
	for (i = 0; i < cells_n; i++) {
		if (scanf("%lu", &j) == 1) {
			if (j && j <= values_n) {
				if (values[j-1].cell) {
					fprintf(stderr, "Duplicate hint\n");
					free(values);
					free(cells);
					return EXIT_FAILURE;
				}
				else {
					add_cell_value(cells+i, values+j-1);
				}
				h_cells_n++;
			}
			else {
				fprintf(stderr, "Invalid hint\n");
				free(values);
				free(cells);
				return EXIT_FAILURE;
			}
		}
		else {
			switch (fgetc(stdin)) {
			case CELL_WHITE:
				w_cells_n++;
				break;
			case CELL_BLACK:
				add_cell_value(cells+i, value_black);
				break;
			default:
				fprintf(stderr, "Invalid cell\n");
				free(values);
				free(cells);
				return EXIT_FAILURE;
			}
		}
		fgetc(stdin);
	}
	if (h_cells_n+w_cells_n != values_n) {
		fprintf(stderr, "Inconsistent grid\n");
		free(values);
		free(cells);
		return EXIT_FAILURE;
	}
	if (!h_cells_n) {
		fprintf(stderr, "No hints provided\n");
		free(values);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_calls = malloc(sizeof(call_t)*(cells_n*LINKS_MAX*CALLS_MAX+1));
	if (!stack_calls) {
		fprintf(stderr, "Could not allocate memory for calls stack\n");
		free(values);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_unrolls = malloc(sizeof(unroll_t)*cells_n*LINKS_MAX);
	if (!stack_unrolls) {
		fprintf(stderr, "Could not allocate memory for unrolls stack\n");
		free(stack_calls);
		free(values);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_rolls = malloc(sizeof(roll_t)*cells_n*LINKS_MAX);
	if (!stack_rolls) {
		fprintf(stderr, "Could not allocate memory for rolls stack\n");
		free(stack_unrolls);
		free(stack_calls);
		free(values);
		free(cells);
		return EXIT_FAILURE;
	}
	hidato();
	free(stack_rolls);
	free(stack_unrolls);
	free(stack_calls);
	free(values);
	free(cells);
	return EXIT_SUCCESS;
}

void set_value(value_t *value, unsigned long number) {
	value->number = number;
	value->cell = NULL;
}

void chain_value(value_t *value, value_t *last, value_t *next) {
	value->last = last;
	value->next = next;
}

void set_cell_pipes(cell_t *cell, unsigned long pipes) {
	cell->pipes = pipes;
}

void hidato(void) {
	nodes_n = 0;
	solutions_n = 0;
	stack_calls_n = 0;
	stack_unrolls_n = 0;
	stack_rolls_n = 0;
	stack_call(stack_calls, CALL_CHOOSE);
	do {
		stack_calls_n--;
		perform_call(stack_calls+stack_calls_n);
	}
	while (stack_calls_n);
	printf("\nNodes %lu\nSolutions %lu\n", nodes_n, solutions_n);
}

void stack_call(call_t *call, call_t type) {
	*call = type;
	stack_calls_n++;
}

void perform_call(call_t *call) {
	switch (*call) {
	case CALL_CHOOSE:
		perform_choose();
		break;
	case CALL_UNROLL:
		stack_unrolls_n--;
		perform_unroll(stack_unrolls+stack_unrolls_n);
		break;
	case CALL_ROLL:
		stack_rolls_n--;
		perform_roll(stack_rolls+stack_rolls_n);
	}
}

void perform_choose(void) {
unsigned long choices_n_min, choices_n, i, j;
value_t *value_min, *value;
cell_t *cell, *choices_min[LINKS_MAX], *choices[LINKS_MAX];
	nodes_n++;
	if (value_black->next == value_black) {
		solutions_n++;
		if (solutions_n == 1) {
			puts("");
			cell = cells;
			for (i = 0; i < rows_n; i++) {
				for (j = 0; j < columns_n-1; j++) {
					print_cell(cell++, ' ');
				}
				print_cell(cell++, '\n');
			}
		}
	}
	else {
		value_min = value_black->next;
		choices_n_min = set_choices(value_min, choices_min);
		for (value = value_min->next; value != value_black && choices_n_min > 1; value = value->next) {
			choices_n = set_choices(value, choices);
			if (choices_n < choices_n_min) {
				value_min = value;
				choices_n_min = choices_n;
				for (i = 0; i < choices_n; i++) {
					choices_min[i] = choices[i];
				}
			}
		}
		for (i = 0; i < choices_n_min; i++) {
			stack_unroll(stack_unrolls+stack_unrolls_n, choices_min[i]);
			stack_call(stack_calls+stack_calls_n, CALL_UNROLL);
			stack_call(stack_calls+stack_calls_n, CALL_CHOOSE);
			stack_roll(stack_rolls+stack_rolls_n, value_min, choices_min[i], stack_unrolls+stack_unrolls_n-1);
			stack_call(stack_calls+stack_calls_n, CALL_ROLL);
		}
	}
}

void print_cell(cell_t *cell, int c) {
	if (cell->value == value_black) {
		printf("%*c", digits_max, CELL_BLACK);
	}
	else {
		printf("%*lu", digits_max, cell->value->number);
	}
	putchar(c);
}

unsigned long set_choices(value_t *value, cell_t *choices[]) {
unsigned long links_inf_n, links_sup_n, choices_n, i, j;
cell_t *cell_inf, *links_inf[LINKS_MAX], *cell_sup, *links_sup[LINKS_MAX];
	if (value->number > 1) {
		cell_inf = values[value->number-2].cell;
		if (cell_inf) {
			links_inf_n = set_links(cell_inf, links_inf);
		}
		else {
			links_inf_n = LINKS_MAX+1;
		}
	}
	else {
		links_inf_n = LINKS_MAX+1;
	}
	if (value->number < values_n) {
		cell_sup = values[value->number].cell;
		if (cell_sup) {
			links_sup_n = set_links(cell_sup, links_sup);
		}
		else {
			links_sup_n = LINKS_MAX+1;
		}
	}
	else {
		links_sup_n = LINKS_MAX+1;
	}
	if (links_inf_n > LINKS_MAX) {
		if (links_sup_n > LINKS_MAX) {
			return LINKS_MAX+1;
		}
		else {
			for (i = 0; i < links_sup_n; i++) {
				set_choice(choices+i, links_sup[i]);
			}
			return links_sup_n;
		}
	}
	else {
		if (links_sup_n > LINKS_MAX) {
			for (i = 0; i < links_inf_n; i++) {
				set_choice(choices+i, links_inf[i]);
			}
			return links_inf_n;
		}
		else {
			choices_n = 0;
			j = 0;
			for (i = 0; i < links_inf_n; i++) {
				while (j < links_sup_n && links_sup[j] < links_inf[i]) {
					j++;
				}
				if (j < links_sup_n && links_sup[j] == links_inf[i]) {
					set_choice(choices+choices_n, links_inf[i]);
					choices_n++;
					j++;
				}
			}
			return choices_n;
		}
	}
}

unsigned long set_links(cell_t *cell, cell_t *links[]) {
unsigned long links_n = 0;
	if (cell->pipes & PIPE_WN) {
		set_choice(links, cell-1-columns_n);
		links_n = 1;
	}
	if (cell->pipes & PIPE_N) {
		set_choice(links+links_n, cell-columns_n);
		links_n++;
	}
	if (cell->pipes & PIPE_NE) {
		set_choice(links+links_n, cell-columns_n+1);
		links_n++;
	}
	if (cell->pipes & PIPE_W) {
		set_choice(links+links_n, cell-1);
		links_n++;
	}
	if (cell->pipes & PIPE_E) {
		set_choice(links+links_n, cell+1);
		links_n++;
	}
	if (cell->pipes & PIPE_SW) {
		set_choice(links+links_n, cell+columns_n-1);
		links_n++;
	}
	if (cell->pipes & PIPE_S) {
		set_choice(links+links_n, cell+columns_n);
		links_n++;
	}
	if (cell->pipes & PIPE_ES) {
		set_choice(links+links_n, cell+1+columns_n);
		links_n++;
	}
	return links_n;
}

void set_choice(cell_t **choice, cell_t *cell) {
	*choice = cell;
}

void stack_unroll(unroll_t *unroll, cell_t *cell) {
	unroll->cell = cell;
	stack_unrolls_n++;
}

void perform_unroll(unroll_t *unroll) {
	remove_cell_value(unroll->cell, unroll->pipes);
}

void remove_cell_value(cell_t *cell, unsigned long pipes) {
value_t *value = cell->value;
	cell->value = NULL;
	add_pipes(cell, pipes);
	value->next->last = value;
	value->last->next = value;
	value->cell = NULL;
}

void add_pipes(cell_t *cell, unsigned long pipes) {
	if (pipes & PIPE_W) {
		add_pipe(cell, PIPE_W, cell-1);
	}
	if (pipes & PIPE_WN) {
		add_pipe(cell, PIPE_WN, cell-1-columns_n);
	}
	if (pipes & PIPE_N) {
		add_pipe(cell, PIPE_N, cell-columns_n);
	}
	if (pipes & PIPE_NE) {
		add_pipe(cell, PIPE_NE, cell-columns_n+1);
	}
	if (pipes & PIPE_E) {
		add_pipe(cell, PIPE_E, cell+1);
	}
	if (pipes & PIPE_ES) {
		add_pipe(cell, PIPE_ES, cell+1+columns_n);
	}
	if (pipes & PIPE_S) {
		add_pipe(cell, PIPE_S, cell+columns_n);
	}
	if (pipes & PIPE_SW) {
		add_pipe(cell, PIPE_SW, cell+columns_n-1);
	}
}

void add_pipe(cell_t *cell, unsigned long pipe_used, cell_t *link) {
	link->pipes += PIPE_E/pipe_used;
	cell->pipes += pipe_used;
}

void stack_roll(roll_t *roll, value_t *value, cell_t *cell, unroll_t *unroll) {
	roll->value = value;
	roll->cell = cell;
	roll->unroll = unroll;
	stack_rolls_n++;
}

void perform_roll(roll_t *roll) {
	set_unroll_pipes(roll->unroll, add_cell_value(roll->cell, roll->value));
}

unsigned long add_cell_value(cell_t *cell, value_t *value) {
unsigned long pipes;
	if (value) {
		value->cell = cell;
		if (value != value_black) {
			value->last->next = value->next;
			value->next->last = value->last;
		}
		pipes = remove_pipes(cell);
	}
	else {
		pipes = 0;
	}
	cell->value = value;
	return pipes;
}

unsigned long remove_pipes(cell_t *cell) {
unsigned long pipes = 0;
	if (cell->pipes & PIPE_W) {
		pipes = remove_pipe(cell, PIPE_W, cell-1);
	}
	if (cell->pipes & PIPE_WN) {
		pipes += remove_pipe(cell, PIPE_WN, cell-1-columns_n);
	}
	if (cell->pipes & PIPE_N) {
		pipes += remove_pipe(cell, PIPE_N, cell-columns_n);
	}
	if (cell->pipes & PIPE_NE) {
		pipes += remove_pipe(cell, PIPE_NE, cell-columns_n+1);
	}
	if (cell->pipes & PIPE_E) {
		pipes += remove_pipe(cell, PIPE_E, cell+1);
	}
	if (cell->pipes & PIPE_ES) {
		pipes += remove_pipe(cell, PIPE_ES, cell+1+columns_n);
	}
	if (cell->pipes & PIPE_S) {
		pipes += remove_pipe(cell, PIPE_S, cell+columns_n);
	}
	if (cell->pipes & PIPE_SW) {
		pipes += remove_pipe(cell, PIPE_SW, cell+columns_n-1);
	}
	return pipes;
}

unsigned long remove_pipe(cell_t *cell, unsigned long pipe_used, cell_t *link) {
	if (link->value) {
		cell->pipes -= pipe_used;
		link->pipes -= PIPE_E/pipe_used;
		return pipe_used;
	}
	else {
		return 0;
	}
}

void set_unroll_pipes(unroll_t *unroll, unsigned long pipes) {
	unroll->pipes = pipes;
}

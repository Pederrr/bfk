#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 30000

typedef enum direction {
	BACK = -1,
	FWD = 1,
} direction;

struct stack {
	char *stack;
	long size;
	int ptr;
};

int stack_is_empty(struct stack *st) {
	return st->ptr == 0;
}

int stack_push(struct stack *st, char value) {
	if (st->ptr + 1 >= st->size) {
		st->size *= 2;
		char *new_stack = realloc(st->stack, st->size);

		if (new_stack == NULL) {
			free(st->stack);
			perror("realloc");
			return -1;
		}

		st->stack = new_stack;
	}

	st->stack[st->ptr] = value;
	st->ptr++;

	return 0;
}

int stack_pop(struct stack *st) {
	if (stack_is_empty(st)) {
		return -1;
	}

	st->ptr--;
	return st->stack[st->ptr];
}

int jump(direction dir, int *program_ptr, char *program, int program_length) {
	int init_program_ptr = *program_ptr;

	struct stack st;
	st.size = 10;
	st.ptr = 0;

	char *stack = malloc(sizeof(char) * st.size);

	if (stack == NULL) {
		perror("malloc");
		return -1;
	}

	st.stack = stack;

	while (*program_ptr < program_length && *program_ptr > 0) {
		if (dir == BACK) {
			if (program[*program_ptr] == ']') {
				if (stack_push(&st, ']') == -1) {
					goto cleanup;
				}
			} else if (program[*program_ptr] == '[') {
				if (stack_pop(&st) == -1) {
					fprintf(stderr, "'[' at %d does not have matching ']'\n", *program_ptr);
					goto cleanup;
				}

				if (stack_is_empty(&st)) {
					free(stack);
					return 0;
				}
			}
		} else if (dir == FWD) {
			if (program[*program_ptr] == '[') {
				if (stack_push(&st, '[') == -1) {
					goto cleanup;
				}
			} else if (program[*program_ptr] == ']') {
				if (stack_pop(&st) == -1) {
					fprintf(stderr, "']' at %d does not have matching '['\n", *program_ptr);
					goto cleanup;
				}

				if (stack_is_empty(&st)) {
					free(stack);
					return 0;
				}
			}
		}
		*program_ptr += dir;
	}

	if (dir == FWD) {
		fprintf(stderr, "Wasn't able to find matching ']' for '[' at %d\n", init_program_ptr);
	} else {
		fprintf(stderr, "Wasn't able to find matching '[' for ']' at %d\n", init_program_ptr);
	}

cleanup:
	free(stack);
	return -1;
}

int execute_program(char *program, long program_length)
{
	int program_ptr = 0;
	long ptr = 0;

	char memory[MEMORY_SIZE];
	memset(memory, 0, program_length);

	while (program_ptr < program_length) {
		switch (program[program_ptr]) {
		case '>':
			ptr++;
			break;
		case '<':
			ptr--;
			break;
		case '+':
			memory[ptr]++;
			break;
		case '-':
			memory[ptr]--;
			break;
		case '.':
			putchar(memory[ptr]);
			fflush(stdout);
			break;
		case ',':
			if ((memory[ptr] = getchar()) == EOF) {
				return 0;
			}
			break;
		case '[':
			if (memory[ptr] == 0 && jump(FWD, &program_ptr, program, program_length) == -1) {
				return -1;
			}
			break;
		case ']':
			if (memory[ptr] != 0 && jump(BACK, &program_ptr, program, program_length) == -1) {
				return -1;
			}
			break;
		default:
			break;
		}

		program_ptr++;
	}

	return 0;
}

int main(int argc, char *argv[]) {

	char program[] =
			{'>', '+', '+', '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+', '+', '+', '+', '+', '>', '-', ']', '<', '.', '>', '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+', '>', '-', ']', '<', '+', '.', '+', '+', '+', '+', '+', '+', '+', '.', '.', '+', '+', '+', '.', '>', '>', '>', '+', '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+', '>', '-', ']', '<', '.', '>', '>', '>', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '[', '<', '+', '+', '+', '+', '+', '+', '+', '+', '+', '>', '-', ']', '<', '-', '-', '-', '.', '<', '<', '<', '<', '.', '+', '+', '+', '.', '-', '-', '-', '-', '-', '-', '.', '-', '-', '-', '-', '-', '-', '-', '-', '.', '>', '>', '+', '.', '>', '+', '+', '+', '+', '+', '+', '+', '+', '+', '+', '.'};
	int result = execute_program(program, 154);

	return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

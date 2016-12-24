#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

int temp = 0;
ucontext_t main_context, *gen_context, *return_context;


void generator(
    ucontext_t *return_context,
    ucontext_t *self_context,
    int *temp,
    int start
) {
    while (1) {
        *temp = start;
        start++;
        swapcontext(self_context, return_context);
    }
}

void check_divisibility(
    ucontext_t *return_context,
    ucontext_t *self_context,
    ucontext_t *input_context,
    int *temp,
    int number
) {
    while (1) {
        swapcontext(self_context, input_context);
        if (*temp % number != 0) {
            swapcontext(self_context, return_context);
        }
    }
}

ucontext_t *initialize_context() {
    ucontext_t *new_context = malloc(sizeof (ucontext_t));
    getcontext(new_context);
    new_context->uc_stack.ss_sp = malloc(sizeof (char) * SIGSTKSZ);
    new_context->uc_stack.ss_size = sizeof (char) * SIGSTKSZ;
    return new_context;
}

int main(int argc, char** argv) {
    return_context = initialize_context();
    gen_context = initialize_context();

    makecontext(gen_context, (void (*) (void)) generator, 4, return_context, gen_context, &temp, 2);

    while (1) {
        swapcontext(return_context, gen_context);
        printf("%d\n", temp);

        ucontext_t *temp_context = initialize_context();
        makecontext(return_context, (void (*) (void)) check_divisibility, 5, temp_context, return_context, gen_context, &temp, temp);

        gen_context = return_context;
        return_context = temp_context;
    }
}
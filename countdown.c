#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define NUM_COUNTERS 5

ucontext_t main_context, loop_context1, loop_context2;

ucontext_t contexts[NUM_COUNTERS];
int completions[NUM_COUNTERS];


void loop(ucontext_t *loop_context, ucontext_t *return_context, int* completion, int count_from) {
    for (int i = count_from; i > 0; i--) {
        printf("%d\n", i);
        swapcontext(loop_context, return_context);
    }
    *completion = 1;
}

int main(int argc, char** argv) {
    char countdown_stack[NUM_COUNTERS][SIGSTKSZ];

    for (int i = 0; i < NUM_COUNTERS; i++) {
        getcontext(&contexts[i]);
        contexts[i].uc_link = &main_context;
        contexts[i].uc_stack.ss_sp = &countdown_stack[i];
        contexts[i].uc_stack.ss_size = sizeof countdown_stack[0];

        makecontext(&contexts[i], (void (*)(void)) loop, 4, &contexts[i], &main_context, &completions[i], 20 - i*2);
    }

    while (1) {
        printf("Start going through countdown timers\n");
        int finished = 1;
        for (int i = 0; i < NUM_COUNTERS; i++) {
            if (!completions[i]) {
                swapcontext(&main_context, &contexts[i]);
                finished = 0;
            }
        }

        if (finished) {
            break;
        }
    }
}


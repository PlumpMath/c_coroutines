#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

ucontext_t main_context1, main_context2, loop_context;

void loop(ucontext_t *loop_context, ucontext_t *return_context, int count_from) {
    for (int i = 0; i < count_from; i++) {
        printf("%d\n", i);
        swapcontext(loop_context, return_context);
    }
}

int main(int argc, char** argv) {
    volatile int iterator_finished = 0;

    char countdown_stack[SIGSTKSZ];
    getcontext(&loop_context);
    loop_context.uc_link = &main_context1;
    loop_context.uc_stack.ss_sp = &countdown_stack;
    loop_context.uc_stack.ss_size = sizeof countdown_stack;

    makecontext(&loop_context, loop, 3, &loop_context, &main_context2, 17);

    getcontext(&main_context1);

    if (!iterator_finished) {
        iterator_finished = 1;
        while (1) {
            swapcontext(&main_context2, &loop_context);
        }
    }
}


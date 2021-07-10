#include <stdio.h>
#include <atomic>
#include <time.h>
#include "stdlib.h"
#include <unistd.h>

#include <fcontext/fcontext.h>

fcontext_t ctx;
fcontext_t ctx2;
static std::atomic<long long int> counter = {0};

static void doo(fcontext_transfer_t from) {
    fcontext_transfer_t to = from;
    while(1) {
      to = jump_fcontext(to.ctx, NULL);
    }
}

static void foo(fcontext_transfer_t from) {
    fcontext_transfer_t to;
    to.ctx = ctx2;
    while (counter.fetch_add(1) < 1000000000) {
      to = jump_fcontext(to.ctx, NULL);
    }
    jump_fcontext(from.ctx, NULL);
}

#include <iostream>
int main() {
    fcontext_stack_t s  = create_fcontext_stack(64 * 1024);
    fcontext_stack_t s2 = create_fcontext_stack(64 * 1024);
    std::cout << "real stack size:" << s2.ssize << std::endl;

    ctx  = make_fcontext(s.sptr, s.ssize, foo);
    ctx2 = make_fcontext(s2.sptr, s2.ssize, doo);

    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    jump_fcontext(ctx, NULL);

    clock_gettime(CLOCK_MONOTONIC, &tend);
    printf("take about %.5f seconds\n",
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
    destroy_fcontext_stack(&s);
    destroy_fcontext_stack(&s2);
    return 0;
}


#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#else
# include <time.h>
#endif

#include <stdio.h>
#include <fcontext/fcontext.h>

fcontext_t ctx;
fcontext_t ctx2;

static inline void  fsleep(uint32_t _ms)
{
#ifdef _WIN32
    Sleep(_ms);
#else
    struct timespec req = { (time_t)_ms / 1000, (long)((_ms % 1000) * 1000000) };
    struct timespec rem = { 0, 0 };
    nanosleep(&req, &rem);
#endif
}

static void doo(fcontext_transfer_t t)
{
    puts("DOO");
    fsleep(1000);
    jump_fcontext(t.ctx, NULL);
}

static void foo(fcontext_transfer_t t)
{
    puts("FOO");
    fsleep(1000);
    jump_fcontext(ctx2, NULL);
    puts("FOO 2");
    fsleep(1000);
    jump_fcontext(t.ctx, NULL);
}

fcontext_transfer_t f11(fcontext_transfer_t t_) {
  printf("f11, from.ctx:%p  data:%d\n", t_.ctx, *(int*)t_.data);
  return t_;
}

void f12(fcontext_transfer_t from) {
  printf("f12-1, from.ctx:%p \n", from.ctx);
  from = jump_fcontext(from.ctx, from.data);
  printf("f12-2, from.ctx:%p \n", from.ctx);
  from = jump_fcontext(from.ctx, from.data);
  printf("f12-leave, from.ctx:%p \n", from.ctx);
}

int main() {
    fcontext_stack_t s = create_fcontext_stack(0);
    fcontext_t ctx = make_fcontext(s.sptr, s.ssize, f12);
    printf("main-0, ctx:%p \n", ctx);
    ctx = jump_fcontext(ctx, 0).ctx;
    printf("main-1, ctx:%p \n", ctx);
    int v = 12;
    ctx = ontop_fcontext(ctx, &v, f11).ctx;
    printf("main-ontop back, ctx:%p\n", ctx);
    destroy_fcontext_stack(&s);
    return 0;
}

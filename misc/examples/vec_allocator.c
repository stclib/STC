#include <stc/cstr.h>
#include <stc/forward.h>


static bool invoked = false;

#define custom_malloc(sz)            malloc(c_i2u(sz))
#define custom_calloc(n, sz)         calloc(c_i2u(n), c_i2u(sz))
#define custom_realloc(p, sz, ctx)        ({ if (ctx != NULL) *(bool *)ctx = true; realloc(p, c_i2u(sz));})
#define custom_free(p, ...)               free(p)

#define i_val int
#define i_allocator custom
#define i_opt c_allocator_ctx
#define i_tag i32
#include <stc/cvec.h>

int main()
{
   cvec_i32 vec = cvec_i32_init(&invoked);
   cvec_i32_push(&vec, 123);
   return invoked ? 0 : 1;
}

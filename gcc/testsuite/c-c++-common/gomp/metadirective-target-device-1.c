/* { dg-do compile }  */
/* { dg-additional-options "-fdump-tree-optimized" } */
/* { dg-additional-options "-DDEVICE_ARCH=x86_64 -DDEVICE_ISA=mmx -mmmx" { target { x86 && lp64 } } } */

#include <stdlib.h>

static void
init (int n, double *a)
{
  for (int i = 0; i < n; i++)
    a[i] = (double) i;
}

static void
check (int n, double *a, double s)
{
  for (int i = 0; i < n; i++)
    if (a[i] != (double) i * s)
      abort ();
}

typedef void (transform_fn) (int, double *, double);

static void
doit (transform_fn *f, int n, double *a, double s)
{
  init (n, a);
  (*f) (n, a, s);
  check (n, a, s);
}

/* Check kind=host matches (with offloading disabled).  */
static void
f1 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={kind(host)}				\
	: parallel for)						\
  default (error at(execution) message("f1 match failed"))
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}

/* Check kind=nohost does not match (with offloading disabled).  */
static void
f2 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={kind(nohost)}				\
	: error at(execution) message("f2 match failed"))	\
  default (parallel for)
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}

/* Check arch.  Either DEVICE_ARCH is defined by command-line option, 
   or we know it is not x86_64.  */
#ifdef DEVICE_ARCH
static void
f3 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={arch(DEVICE_ARCH)}			\
	: parallel for)						\
  default (error at(execution) message("f3 match failed"))
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#else
static void
f3 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={arch("x86_64")}				\
	: error at(execution) message("f3 match failed"))	\
  default (parallel for)
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#endif

/* Check both kind and arch together.  */
#ifdef DEVICE_ARCH
static void
f4 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={arch(DEVICE_ARCH), kind(host)}		\
	: parallel for)						\
  default (error at(execution) message("f4 match failed"))
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#else
static void
f4 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={arch("x86_64"), kind(host)}		\
	: error at(execution) message("f4 match failed"))	\
  default (parallel for)
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#endif

/* Check kind, arch, and ISA together.  */
#if defined(DEVICE_ARCH) && defined(DEVICE_ISA)
static void
f5 (int n, double* a, double s)
{
#pragma omp metadirective						\
  when (target_device={arch(DEVICE_ARCH), kind(host), isa(DEVICE_ISA)}	\
	: parallel for)							\
  default (error at(execution) message("f5 match failed"))
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#else
static void
f5 (int n, double* a, double s)
{
#pragma omp metadirective					\
  when (target_device={arch("x86_64"), kind(host), isa("mmx")}	\
	: error at(execution) message("f5 match failed"))	\
  default (parallel for)
  for (int i = 0; i < n; i++)
    a[i] = a[i] * s;
}
#endif

#define N 10
#define S 2.0

int
main (void)
{
  double a[N];
  doit (f1, N, a, S);
  doit (f2, N, a, S);
  doit (f3, N, a, S);
  doit (f4, N, a, S);
  doit (f5, N, a, S);
}

/* The gimplifier emits all 5 alternatives.  In configurations without
   offloading, the dynamic selector can be completely resolved, otherwise
   the calls to GOMP_error must remain in the generated code pending
   runtime evaluation of the selectors.  */
/* { dg-final { scan-tree-dump-times "GOMP_error" 5 "optimized" { target offloading_enabled } } } */
/* { dg-final { scan-tree-dump-times "GOMP_error" 0 "optimized" { target { ! offloading_enabled } } } } */

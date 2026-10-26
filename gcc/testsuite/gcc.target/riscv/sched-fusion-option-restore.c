/* Verify that restoring function-specific optimization options preserves
   the global scheduling-fusion default.  */
/* { dg-do compile { target { rv64 } } } */
/* { dg-options "-march=rv64gc -mabi=lp64d -Os -mtune=xt-c9501fdvt" } */

__attribute__((optimize("O3"))) int
fusion_option_foo (int a)
{
  return a + 1;
}

__attribute__((optimize("O3"))) int
fusion_option_bar (int a)
{
  return a + 1;
}

/* Verify that frame-related pairs split before final and retain CFI notes.  */
/* { dg-do compile { target { rv64 } } } */
/* { dg-options "-march=rv64gc -mabi=lp64d -mtune=xt-c9501fdvt -O2 -mpair-fusion -fno-schedule-fusion -fno-schedule-insns2 -funwind-tables -fdump-rtl-riscv_pair_fusion-details" } */
/* { dg-final { scan-rtl-dump "fusing pair" "riscv_pair_fusion" } } */
/* { dg-final { scan-assembler {\.cfi_offset 8,} } } */
/* { dg-final { scan-assembler {\.cfi_offset 9,} } } */
/* { dg-final { scan-assembler {\.cfi_restore 8} } } */
/* { dg-final { scan-assembler {\.cfi_restore 9} } } */

extern void escape (void);

void __attribute__ ((noinline, noipa))
pair_backend_frame (void)
{
  asm volatile ("" ::: "s0", "s1");
  escape ();
  asm volatile ("" ::: "s0", "s1");
}

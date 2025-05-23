/* { dg-do compile { target { ! riscv_abi_e } } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" "-Os" "-Oz" } } */
/* { dg-options "-march=rv64gc -mtune=sifive-5-series -mbranch-cost=4 -mmovcc -fdump-rtl-ce1" { target { rv64 } } } */
/* { dg-options "-march=rv32gc -mtune=sifive-5-series -mbranch-cost=4 -mmovcc -fdump-rtl-ce1" { target { rv32 } } } */

typedef int __attribute__ ((mode (SI))) int_t;

int_t
movsifge (double w, double x, int_t y, int_t z)
{
  return w >= x ? y : z;
}

/* Expect branched assembly like:

	fge.d	a4,fa0,fa1
	mv	a5,a0
	mv	a0,a1
	beq	a4,zero,.L2
	mv	a0,a5
.L2:
 */

/* { dg-final { scan-rtl-dump-not "Conversion succeeded on pass \[0-9\]+\\." "ce1" } } */
/* { dg-final { scan-rtl-dump-not "if-conversion succeeded through" "ce1" } } */
/* { dg-final { scan-assembler-times "\\s(?:fge\\.d|fle\\.d)\\s" 1 } } */
/* { dg-final { scan-assembler-times "\\s(?:beq|bne)\\s" 1 } } */
/* { dg-final { scan-assembler-not "\\s(?:seqz|snez)\\s" } } */

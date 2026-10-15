/* { dg-do compile { target { rv64 } } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-O3" "-O[sgz]" "-flto" } } */
/* { dg-options "-march=rv64g -mabi=lp64d -mtune=xt-c9501fdvt -O2 -fno-schedule-fusion -fno-schedule-insns2 -mpair-fusion --param=riscv-pair-fusion-max-insns=2 -fdump-rtl-riscv_pair_fusion-details" } */
/* { dg-final { scan-rtl-dump-not "grouped scalar pair" "riscv_pair_fusion" } } */

long __RTL (startwith ("*leaf_regs"))
pair_span_limit (void)
{
(function "pair_span_limit"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (mem:DI (plus:DI (reg:DI a0)
                                                 (const_int 8)) [0 S8 A64])))
      (cinsn 6 (use (reg:DI a1)))
      (cinsn 7 (use (reg:DI a2)))
      (cinsn 8 (use (reg:DI a3)))
      (cjump_insn 9 (simple_return))
      (edge-to exit)
    )
    (cbarrier 10)
  )
  (crtl (return_rtx (reg/i:DI a0)))
)
}

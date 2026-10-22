/* Verify that dependencies and aliasing prevent composite pairs.  */
/* { dg-do compile { target { rv64 } } } */
/* { dg-skip-if "RTL functions do not support LTO" { *-*-* } { "-flto" } } */
/* { dg-options "-march=rv64gc -mabi=lp64d -mtune=xt-c9501fdvt -O2 -mpair-fusion -fno-schedule-fusion -fno-schedule-insns2 -fdump-rtl-riscv_pair_fusion-details" } */
/* { dg-final { scan-rtl-dump-not "fusing pair" "riscv_pair_fusion" } } */

long __RTL (startwith ("*leaf_regs"))
pair_backend_reg_hazard (void)
{
(function "pair_backend_reg_hazard"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a1) (reg:DI a2))))
      (cinsn 5 (set (reg:DI a2)
                    (mem:DI (plus:DI (reg:DI a0) (const_int 8))
                            [0 S8 A64])))
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

long __RTL (startwith ("*leaf_regs"))
pair_backend_memory_barrier (void)
{
(function "pair_backend_memory_barrier"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (mem:DI (reg:DI a0) [0 S8 A64]) (reg:DI a1)))
      (cinsn 4 (set (mem:DI (reg:DI a3) [0 S8 A64]) (reg:DI a4)))
      (cinsn 5 (set (mem:DI (plus:DI (reg:DI a0) (const_int 8))
                            [0 S8 A64]) (reg:DI a2)))
      (cinsn 6 (use (reg:DI a1)))
      (cinsn 7 (use (reg:DI a2)))
      (cjump_insn 8 (simple_return))
      (edge-to exit)
    )
    (cbarrier 9)
  )
  (crtl (return_rtx (reg/i:DI a0)))
)
}

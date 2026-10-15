/* { dg-do compile { target { rv64 } } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-O3" "-O[sgz]" "-flto" } } */
/* { dg-options "-march=rv64g -mabi=lp64d -mtune=xt-c9501fdvt -O2 -fno-schedule-fusion -fno-schedule-insns2 -mpair-fusion -fdump-rtl-riscv_pair_fusion-details" } */
/* { dg-final { scan-rtl-dump-not "grouped scalar pair" "riscv_pair_fusion" } } */

long __RTL (startwith ("*leaf_regs"))
pair_reg_hazard (void)
{
(function "pair_reg_hazard"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a2) (plus:DI (reg:DI a1) (reg:DI a2))))
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

long __RTL (startwith ("*leaf_regs"))
pair_alias_hazard (void)
{
(function "pair_alias_hazard"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (mem:DI (reg:DI a0) [0 S8 A64]) (reg:DI a1)))
      (cinsn 4 (set (mem:DI (reg:DI a3) [0 S8 A64]) (reg:DI a4)))
      (cinsn 5 (set (mem:DI (plus:DI (reg:DI a0) (const_int 8)) [0 S8 A64])
                    (reg:DI a2)))
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
pair_base_change (void)
{
(function "pair_base_change"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a0) (mem:DI (reg:DI a0) [0 S8 A64])))
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

long __RTL (startwith ("*leaf_regs"))
pair_wrong_width (void)
{
(function "pair_wrong_width"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (sign_extend:DI (mem:HI
                                                        (reg:DI a0) [0 S2 A16]))))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (sign_extend:DI (mem:HI (plus:DI (reg:DI a0)
                                                                 (const_int 2)) [0 S2 A16]))))
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
pair_zero_extend (void)
{
(function "pair_zero_extend"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (zero_extend:DI (mem:SI
                                                        (reg:DI a0) [0 S4 A32]))))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (zero_extend:DI (mem:SI (plus:DI (reg:DI a0)
                                                                 (const_int 4)) [0 S4 A32]))))
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
pair_volatile (void)
{
(function "pair_volatile"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem/v:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (mem/v:DI (plus:DI (reg:DI a0)
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

long __RTL (startwith ("*leaf_regs"))
pair_nonadjacent (void)
{
(function "pair_nonadjacent"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (mem:DI (plus:DI (reg:DI a0)
                                                 (const_int 16)) [0 S8 A64])))
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

long
pair_asm_barrier (const long *p)
{
  long a = p[0];
  __asm__ volatile ("" : : : "memory");
  return a + p[1];
}

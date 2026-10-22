/* { dg-do compile { target { rv64 } } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-O3" "-O[sgz]" "-flto" } } */
/* { dg-options "-march=rv64gc -mabi=lp64d -mtune=xt-c9501fdvt -O2 -fno-schedule-fusion -fno-schedule-insns2 -mpair-fusion -fdump-rtl-riscv_pair_fusion-details" } */
/* { dg-final { check-function-bodies "**" "" } } */
/* { dg-final { scan-rtl-dump-times "grouped scalar pair" 9 "riscv_pair_fusion" } } */

/*
**pair_load:
**	add	a3,a4,a5
**	ld	a2,8\(a0\)
**	ld	a1,0\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_load (void)
{
(function "pair_load"
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

/* The second load overwrites the base, so only ascending order is valid.  */
/*
**pair_load_ascending_only:
**	add	a3,a4,a5
**	ld	a1,0\(a0\)
**	ld	a0,8\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_load_ascending_only (void)
{
(function "pair_load_ascending_only"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a0)
                    (mem:DI (plus:DI (reg:DI a0) (const_int 8))
                            [0 S8 A64])))
      (cinsn 6 (use (reg:DI a1)))
      (cinsn 7 (use (reg:DI a0)))
      (cinsn 8 (use (reg:DI a3)))
      (cjump_insn 9 (simple_return))
      (edge-to exit)
    )
    (cbarrier 10)
  )
  (crtl (return_rtx (reg/i:DI a0)))
)
}

/*
**pair_store:
**	sd	a2,8\(a0\)
**	sd	a1,0\(a0\)
**	add	a3,a4,a5
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_store (void)
{
(function "pair_store"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (mem:DI (reg:DI a0) [0 S8 A64]) (reg:DI a1)))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
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

/*
**pair_sign_extend:
**	add	a3,a4,a5
**	lw	a2,4\(a0\)
**	lw	a1,0\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_sign_extend (void)
{
(function "pair_sign_extend"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (sign_extend:DI (mem:SI
                                                        (reg:DI a0) [0 S4 A32]))))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (sign_extend:DI (mem:SI (plus:DI (reg:DI a0)
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

/*
**pair_descending:
**	add	a3,a4,a5
**	ld	a1,-8\(a0\)
**	ld	a2,-16\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_descending (void)
{
(function "pair_descending"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem:DI (plus:DI (reg:DI a0)
                                                 (const_int -8)) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (mem:DI (plus:DI (reg:DI a0)
                                                 (const_int -16)) [0 S8 A64])))
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

/*
**pair_disjoint_memory:
**	sd	a2,8\(a0\)
**	sd	a1,0\(a0\)
**	ld	a3,32\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_disjoint_memory (void)
{
(function "pair_disjoint_memory"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (mem:DI (reg:DI a0) [0 S8 A64]) (reg:DI a1)))
      (cinsn 4 (set (reg:DI a3) (mem:DI (plus:DI (reg:DI a0)
                                                 (const_int 32)) [0 S8 A64])))
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

/*
**pair_two_regions:
**	sd	a2,8\(a0\)
**	sd	a1,0\(a0\)
**	add	a3,a4,a5
**	sd	a5,40\(a0\)
**	sd	a3,32\(a0\)
**	add	a6,a4,a5
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_two_regions (void)
{
(function "pair_two_regions"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (mem:DI (reg:DI a0) [0 S8 A64]) (reg:DI a1)))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (mem:DI (plus:DI (reg:DI a0) (const_int 8)) [0 S8 A64])
                    (reg:DI a2)))
      (cinsn 6 (set (mem:DI (plus:DI (reg:DI a0) (const_int 32)) [0 S8 A64])
                    (reg:DI a3)))
      (cinsn 7 (set (reg:DI a6) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 8 (set (mem:DI (plus:DI (reg:DI a0) (const_int 40)) [0 S8 A64])
                    (reg:DI a5)))
      (cinsn 9 (use (reg:DI a1)))
      (cinsn 10 (use (reg:DI a2)))
      (cinsn 11 (use (reg:DI a3)))
      (cjump_insn 12 (simple_return))
      (edge-to exit)
    )
    (cbarrier 13)
  )
  (crtl (return_rtx (reg/i:DI a0)))
)
}

/*
**pair_readonly:
**	add	a3,a4,a5
**	ld	a2,8\(a0\)
**	ld	a1,0\(a0\)
**	ret
*/
long __RTL (startwith ("*leaf_regs"))
pair_readonly (void)
{
(function "pair_readonly"
  (insn-chain
    (block 2
      (edge-from entry (flags "FALLTHRU"))
      (cnote 1 [bb 2] NOTE_INSN_BASIC_BLOCK)
      (cnote 2 NOTE_INSN_FUNCTION_BEG)
      (cinsn 3 (set (reg:DI a1) (mem/u:DI (reg:DI a0) [0 S8 A64])))
      (cinsn 4 (set (reg:DI a3) (plus:DI (reg:DI a4) (reg:DI a5))))
      (cinsn 5 (set (reg:DI a2) (mem/u:DI (plus:DI (reg:DI a0)
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

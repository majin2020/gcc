/* Scalar memory pair fusion for RISC-V.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "rtl.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "cfgrtl.h"
#include "tm_p.h"
#include "tree-pass.h"
#include "df.h"
#include "pair-fusion.h"

namespace {

/* Extract a hard-register base and a constant offset.  */

static bool
riscv_pair_address_p (rtx mem, rtx *base, HOST_WIDE_INT *offset)
{
  rtx addr = XEXP (mem, 0);
  if (REG_P (addr) && HARD_REGISTER_P (addr))
    {
      *base = addr;
      *offset = 0;
      return true;
    }

  if (GET_CODE (addr) != PLUS
      || !REG_P (XEXP (addr, 0))
      || !HARD_REGISTER_P (XEXP (addr, 0))
      || !CONST_INT_P (XEXP (addr, 1)))
    return false;

  *base = XEXP (addr, 0);
  *offset = INTVAL (XEXP (addr, 1));
  return true;
}

/* Check two scalar accesses in instruction order against the enabled pairs.  */

static bool
riscv_pair_patterns_p (rtx *pats, bool load_p)
{
  rtx mem[2], reg[2], base[2];
  HOST_WIDE_INT offset[2];
  bool fp[2];

  for (int i = 0; i < 2; ++i)
    {
      if (GET_CODE (pats[i]) != SET)
	return false;
      mem[i] = XEXP (pats[i], load_p);
      reg[i] = XEXP (pats[i], !load_p);
      if (!MEM_P (mem[i]) || MEM_VOLATILE_P (mem[i])
	  || !riscv_pair_address_p (mem[i], &base[i], &offset[i]))
	return false;

      if (REG_P (reg[i]) && HARD_REGISTER_P (reg[i]))
	fp[i] = FP_REG_RTX_P (reg[i]);
      else if (!load_p && reg[i] == const0_rtx)
	fp[i] = false;
      else
	return false;
    }

  machine_mode mode = GET_MODE (mem[0]);
  if (mode != GET_MODE (mem[1])
      || !rtx_equal_p (base[0], base[1])
      || fp[0] != fp[1])
    return false;

  poly_uint64 size = GET_MODE_SIZE (mode);
  if (!known_eq (size, 4U) && !known_eq (size, 8U))
    return false;

  if (fp[0] ? !SCALAR_FLOAT_MODE_P (mode) : !SCALAR_INT_MODE_P (mode))
    return false;

  for (int i = 0; i < 2; ++i)
    if (REG_P (reg[i]) && GET_MODE (reg[i]) != mode)
      return false;

  HOST_WIDE_INT access_size = size.to_constant ();
  unsigned HOST_WIDE_INT unsigned_size = access_size;
  bool inc_p = (offset[1] > offset[0]
		&& ((unsigned HOST_WIDE_INT) offset[1]
		    - (unsigned HOST_WIDE_INT) offset[0]) == unsigned_size);
  bool dec_p = (offset[0] > offset[1]
		&& ((unsigned HOST_WIDE_INT) offset[0]
		    - (unsigned HOST_WIDE_INT) offset[1]) == unsigned_size);
  unsigned HOST_WIDE_INT ops = riscv_get_fusible_ops ();
  enum riscv_fusion_pairs inc_op = fp[0]
    ? RISCV_FUSE_FLDFST_PAIR_INC : RISCV_FUSE_LDST_PAIR_INC;
  enum riscv_fusion_pairs dec_op = fp[0]
    ? RISCV_FUSE_FLDFST_PAIR_DEC : RISCV_FUSE_LDST_PAIR_DEC;

  if (load_p
      && (rtx_equal_p (reg[0], reg[1])
	  || rtx_equal_p (reg[0], base[0])))
    return false;

  if ((inc_p && (ops & inc_op)) || (dec_p && (ops & dec_op)))
    return true;

  return (!load_p && !fp[0] && (ops & RISCV_FUSE_ALIGNED_STD)
	  && (inc_p || dec_p)
	  && MIN (offset[0], offset[1]) % (2 * access_size) == 0);
}

/* Construct the scalar SETs represented by an MD pair pattern.  */

static bool
riscv_pair_operands_p (rtx *operands, bool load_p)
{
  rtx pats[2] = {
    gen_rtx_SET (operands[0], operands[1]),
    gen_rtx_SET (operands[2], operands[3])
  };
  return riscv_pair_patterns_p (pats, load_p);
}

class riscv_pair_fusion : public pair_fusion
{
public:
  bool fpsimd_op_p (rtx reg_op, machine_mode, bool) final override
  {
    return REG_P (reg_op) && FP_REG_RTX_P (reg_op);
  }

  bool pair_operand_mode_ok_p (machine_mode mode) final override
  {
    poly_uint64 size = GET_MODE_SIZE (mode);
    return (SCALAR_INT_MODE_P (mode) || SCALAR_FLOAT_MODE_P (mode))
	   && (known_eq (size, 4U) || known_eq (size, 8U));
  }

  bool pair_reg_operand_ok_p (bool load_p, rtx reg_op,
			      machine_mode) final override
  {
    return (REG_P (reg_op) && HARD_REGISTER_P (reg_op))
	   || (!load_p && reg_op == const0_rtx);
  }

  int pair_mem_alias_check_limit () final override
  {
    return riscv_pair_fusion_alias_check_limit;
  }

  bool should_handle_writeback (writeback_type) final override
  {
    return false;
  }

  bool pair_mem_ok_with_policy (rtx, bool) final override
  {
    return true;
  }

  bool pair_mem_insn_p (rtx_insn *, bool &) final override
  {
    return false;
  }

  bool pair_mem_in_range_p (HOST_WIDE_INT) final override
  {
    return true;
  }

  rtx gen_pair (rtx *pats, rtx writeback, bool load_p) final override
  {
    gcc_assert (!writeback);
    rtx ordered[2] = { pats[1], pats[0] };
    if (!riscv_pair_patterns_p (ordered, load_p))
      {
	ordered[0] = pats[0];
	ordered[1] = pats[1];
      }
    return gen_rtx_PARALLEL (VOIDmode,
			     gen_rtvec (2, ordered[0], ordered[1]));
  }

  rtx destructure_pair (rtx[2], rtx, bool) final override
  {
    gcc_unreachable ();
  }

  rtx gen_promote_writeback_pair (rtx, rtx, rtx[2], bool) final override
  {
    gcc_unreachable ();
  }
};

const pass_data pass_data_riscv_pair_fusion =
{
  RTL_PASS,
  "riscv_pair_fusion",
  OPTGROUP_NONE,
  TV_MACH_DEP,
  0, 0, 0, 0,
  TODO_df_finish,
};

class pass_riscv_pair_fusion : public rtl_opt_pass
{
public:
  pass_riscv_pair_fusion (gcc::context *ctxt)
    : rtl_opt_pass (pass_data_riscv_pair_fusion, ctxt)
  {}

  bool gate (function *) final override
  {
    return (optimize > 0 && !optimize_debug && flag_riscv_pair_fusion
	    && reload_completed && !flag_selective_scheduling2
	    && !cfun->can_throw_non_call_exceptions
	    && (riscv_get_fusible_ops ()
		& (RISCV_FUSE_LDST_PAIR_INC | RISCV_FUSE_LDST_PAIR_DEC
		   | RISCV_FUSE_FLDFST_PAIR_INC
		   | RISCV_FUSE_FLDFST_PAIR_DEC
		   | RISCV_FUSE_ALIGNED_STD)) != 0);
  }

  unsigned int execute (function *) final override
  {
    riscv_pair_fusion pass;
    pass.run ();
    return 0;
  }
};

/* Find the scalar instruction to which a frame note belongs.  */

static int
riscv_pair_note_arm (rtx note, rtx pats[2])
{
  if (GET_CODE (note) == SET)
    {
      for (int i = 0; i < 2; ++i)
	if (rtx_equal_p (SET_DEST (note), SET_DEST (pats[i])))
	  return i;
    }
  else if (REG_P (note))
    {
      for (int i = 0; i < 2; ++i)
	if (rtx_equal_p (note, SET_DEST (pats[i])))
	  return i;
    }

  return -1;
}

/* Attach the frame expression to the corresponding scalar instruction.  */

static void
riscv_split_pair_frame_expr (rtx expr, rtx pats[2], rtx_insn *insns[2])
{
  if (GET_CODE (expr) == PARALLEL)
    {
      for (int i = 0; i < XVECLEN (expr, 0); ++i)
	riscv_split_pair_frame_expr (XVECEXP (expr, 0, i), pats, insns);
      return;
    }

  int arm = riscv_pair_note_arm (expr, pats);
  gcc_assert (arm >= 0);
  RTX_FRAME_RELATED_P (insns[arm]) = 1;
  add_reg_note (insns[arm], REG_FRAME_RELATED_EXPR, copy_rtx (expr));
}

/* Restore scalar RTL and distribute the CFI notes kept by pair_fusion.  */

static void
riscv_split_pair (rtx_insn *insn)
{
  rtx pat = PATTERN (insn);
  rtx pats[2] = { XVECEXP (pat, 0, 0), XVECEXP (pat, 0, 1) };
  rtx_insn *split[2];

  for (int i = 0; i < 2; ++i)
    {
      split[i] = emit_insn_before (copy_rtx (pats[i]), insn);
      INSN_LOCATION (split[i]) = INSN_LOCATION (insn);
      BLOCK_FOR_INSN (split[i]) = BLOCK_FOR_INSN (insn);
    }

  for (rtx note = REG_NOTES (insn); note; note = XEXP (note, 1))
    {
      enum reg_note kind = REG_NOTE_KIND (note);
      rtx value = XEXP (note, 0);
      if (kind == REG_FRAME_RELATED_EXPR)
	{
	  riscv_split_pair_frame_expr (value, pats, split);
	  continue;
	}

      int arm = riscv_pair_note_arm (value, pats);
      if (arm < 0 && kind == REG_DEAD)
	{
	  for (int i = 1; i >= 0; --i)
	    if (reg_mentioned_p (value, pats[i]))
	      {
		arm = i;
		break;
	      }
	}
      if (arm < 0)
	arm = 1;

      if (kind == REG_CFA_DEF_CFA || kind == REG_CFA_OFFSET
	  || kind == REG_CFA_RESTORE)
	RTX_FRAME_RELATED_P (split[arm]) = 1;
      add_reg_note (split[arm], kind, copy_rtx (value));
    }

  for (int i = 0; i < 2; ++i)
    df_insn_rescan (split[i]);
  df_insn_delete (insn);
  delete_insn (insn);
}

const pass_data pass_data_riscv_pair_split =
{
  RTL_PASS,
  "riscv_pair_split",
  OPTGROUP_NONE,
  TV_MACH_DEP,
  0, 0, 0, 0, 0,
};

class pass_riscv_pair_split : public rtl_opt_pass
{
public:
  pass_riscv_pair_split (gcc::context *ctxt)
    : rtl_opt_pass (pass_data_riscv_pair_split, ctxt)
  {}

  unsigned int execute (function *) final override
  {
    for (rtx_insn *insn = get_insns (); insn;)
      {
	rtx_insn *next = NEXT_INSN (insn);
	if (NONDEBUG_INSN_P (insn))
	  {
	    rtx pat = PATTERN (insn);
	    if (GET_CODE (pat) == PARALLEL && XVECLEN (pat, 0) == 2)
	      {
		rtx pats[2] = { XVECEXP (pat, 0, 0),
				XVECEXP (pat, 0, 1) };
		if (GET_CODE (pats[0]) == SET)
		  {
		    bool load_p = MEM_P (SET_SRC (pats[0]));
		    if (riscv_pair_patterns_p (pats, load_p))
		      riscv_split_pair (insn);
		  }
	      }
	  }
	insn = next;
      }
    return 0;
  }
};

}

bool
riscv_pair_load_operands_p (rtx *operands)
{
  return riscv_pair_operands_p (operands, true);
}

bool
riscv_pair_store_operands_p (rtx *operands)
{
  return riscv_pair_operands_p (operands, false);
}

rtl_opt_pass *
make_pass_riscv_pair_fusion (gcc::context *ctxt)
{
  return new pass_riscv_pair_fusion (ctxt);
}

rtl_opt_pass *
make_pass_riscv_pair_split (gcc::context *ctxt)
{
  return new pass_riscv_pair_split (ctxt);
}

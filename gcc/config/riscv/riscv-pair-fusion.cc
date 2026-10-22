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
#include "tm_p.h"
#include "tree-pass.h"
#include "pair-fusion.h"

namespace {

class riscv_pair_fusion : public pair_fusion
{
public:
  bool keep_separate_p () final override
  {
    return true;
  }

  bool pair_mem_candidate_p (rtx_insn *insn) final override
  {
    return riscv_mem_pair_candidate_p (insn);
  }

  bool pair_fusible_p (rtx_insn *first, rtx_insn *second) final override
  {
    return riscv_mem_pair_p (first, second);
  }

  bool prefer_descending_p () final override
  {
    return true;
  }

  unsigned pair_reorder_limit () final override
  {
    return riscv_pair_fusion_max_insns;
  }

  int pair_mem_alias_check_limit () final override
  {
    return riscv_pair_fusion_alias_check_limit;
  }

  bool fpsimd_op_p (rtx reg_op, machine_mode, bool) final override
  {
    return FP_REG_RTX_P (reg_op);
  }

  bool pair_operand_mode_ok_p (machine_mode mode) final override
  {
    poly_uint64 size = GET_MODE_SIZE (mode);
    return (known_eq (size, 1U) || known_eq (size, 2U)
	    || known_eq (size, 4U) || known_eq (size, 8U));
  }

  bool pair_reg_operand_ok_p (bool load_p, rtx reg_op,
			      machine_mode) final override
  {
    return ((REG_P (reg_op) && HARD_REGISTER_P (reg_op))
	    || (!load_p && reg_op == CONST0_RTX (GET_MODE (reg_op))));
  }

  bool should_handle_writeback (writeback_type) final override
  {
    return false;
  }

  bool pair_mem_in_range_p (HOST_WIDE_INT) final override
  {
    return true;
  }
  bool pair_mem_ok_with_policy (rtx, bool) final override
  {
    return true;
  }
  bool pair_mem_insn_p (rtx_insn *, bool &) final override
  {
    return false;
  }

  rtx gen_pair (rtx *, rtx, bool) final override
  {
    gcc_unreachable ();
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
  0,
  0,
  0,
  0,
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
		& (RISCV_FUSE_ALIGNED_STD
		   | RISCV_FUSE_LDST_PAIR_INC | RISCV_FUSE_LDST_PAIR_DEC
		   | RISCV_FUSE_FLDFST_PAIR_INC
		   | RISCV_FUSE_FLDFST_PAIR_DEC)) != 0);
  }

  unsigned int execute (function *) final override
  {
    riscv_pair_fusion pass;
    pass.run ();
    return 0;
  }
};

}

rtl_opt_pass *
make_pass_riscv_pair_fusion (gcc::context *ctxt)
{
  return new pass_riscv_pair_fusion (ctxt);
}

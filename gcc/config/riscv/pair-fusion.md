;; Scalar load/store pairs stay together through sched2 and split before final.

(define_insn "*riscv_fused_load_pair"
  [(set (match_operand 0 "register_operand" "=r,f")
	(match_operand 1 "memory_operand" "m,m"))
   (set (match_operand 2 "register_operand" "=r,f")
	(match_operand 3 "memory_operand" "m,m"))]
  "riscv_pair_load_operands_p (operands)"
  "#"
  [(set_attr "type" "multi")
   (set_attr "length" "8")])

(define_insn "*riscv_fused_store_pair"
  [(set (match_operand 0 "memory_operand" "=m,m")
	(match_operand 1 "reg_or_0_operand" "rJ,f"))
   (set (match_operand 2 "memory_operand" "=m,m")
	(match_operand 3 "reg_or_0_operand" "rJ,f"))]
  "riscv_pair_store_operands_p (operands)"
  "#"
  [(set_attr "type" "multi")
   (set_attr "length" "8")])

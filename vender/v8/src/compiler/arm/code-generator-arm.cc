#include "src/v8.h"

#if V8_TARGET_ARCH_ARM

// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/code-generator.h"

#include "src/arm/macro-assembler-arm.h"
#include "src/compiler/code-generator-impl.h"
#include "src/compiler/gap-resolver.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/node-properties-inl.h"
#include "src/compiler/osr.h"
#include "src/scopes.h"

namespace v8 {
namespace internal {
namespace compiler {

#define __ masm()->


#define kScratchReg r9


// Adds Arm-specific methods to convert InstructionOperands.
class ArmOperandConverter FINAL : public InstructionOperandConverter {
 public:
  ArmOperandConverter(CodeGenerator* gen, Instruction* instr)
      : InstructionOperandConverter(gen, instr) {}

  SwVfpRegister OutputFloat32Register(int index = 0) {
    return ToFloat32Register(instr_->OutputAt(index));
  }

  SwVfpRegister InputFloat32Register(int index) {
    return ToFloat32Register(instr_->InputAt(index));
  }

  SwVfpRegister ToFloat32Register(InstructionOperand* op) {
    return ToFloat64Register(op).low();
  }

  LowDwVfpRegister OutputFloat64Register(int index = 0) {
    return ToFloat64Register(instr_->OutputAt(index));
  }

  LowDwVfpRegister InputFloat64Register(int index) {
    return ToFloat64Register(instr_->InputAt(index));
  }

  LowDwVfpRegister ToFloat64Register(InstructionOperand* op) {
    return LowDwVfpRegister::from_code(ToDoubleRegister(op).code());
  }

  SBit OutputSBit() const {
    switch (instr_->flags_mode()) {
      case kFlags_branch:
      case kFlags_set:
        return SetCC;
      case kFlags_none:
        return LeaveCC;
    }
    UNREACHABLE();
    return LeaveCC;
  }

  Operand InputImmediate(int index) {
    Constant constant = ToConstant(instr_->InputAt(index));
    switch (constant.type()) {
      case Constant::kInt32:
        return Operand(constant.ToInt32());
      case Constant::kFloat32:
        return Operand(
            isolate()->factory()->NewNumber(constant.ToFloat32(), TENURED));
      case Constant::kFloat64:
        return Operand(
            isolate()->factory()->NewNumber(constant.ToFloat64(), TENURED));
      case Constant::kInt64:
      case Constant::kExternalReference:
      case Constant::kHeapObject:
      case Constant::kRpoNumber:
        break;
    }
    UNREACHABLE();
    return Operand::Zero();
  }

  Operand InputOperand2(int first_index) {
    const int index = first_index;
    switch (AddressingModeField::decode(instr_->opcode())) {
      case kMode_None:
      case kMode_Offset_RI:
      case kMode_Offset_RR:
        break;
      case kMode_Operand2_I:
        return InputImmediate(index + 0);
      case kMode_Operand2_R:
        return Operand(InputRegister(index + 0));
      case kMode_Operand2_R_ASR_I:
        return Operand(InputRegister(index + 0), ASR, InputInt5(index + 1));
      case kMode_Operand2_R_ASR_R:
        return Operand(InputRegister(index + 0), ASR, InputRegister(index + 1));
      case kMode_Operand2_R_LSL_I:
        return Operand(InputRegister(index + 0), LSL, InputInt5(index + 1));
      case kMode_Operand2_R_LSL_R:
        return Operand(InputRegister(index + 0), LSL, InputRegister(index + 1));
      case kMode_Operand2_R_LSR_I:
        return Operand(InputRegister(index + 0), LSR, InputInt5(index + 1));
      case kMode_Operand2_R_LSR_R:
        return Operand(InputRegister(index + 0), LSR, InputRegister(index + 1));
      case kMode_Operand2_R_ROR_I:
        return Operand(InputRegister(index + 0), ROR, InputInt5(index + 1));
      case kMode_Operand2_R_ROR_R:
        return Operand(InputRegister(index + 0), ROR, InputRegister(index + 1));
    }
    UNREACHABLE();
    return Operand::Zero();
  }

  MemOperand InputOffset(int* first_index) {
    const int index = *first_index;
    switch (AddressingModeField::decode(instr_->opcode())) {
      case kMode_None:
      case kMode_Operand2_I:
      case kMode_Operand2_R:
      case kMode_Operand2_R_ASR_I:
      case kMode_Operand2_R_ASR_R:
      case kMode_Operand2_R_LSL_I:
      case kMode_Operand2_R_LSL_R:
      case kMode_Operand2_R_LSR_I:
      case kMode_Operand2_R_LSR_R:
      case kMode_Operand2_R_ROR_I:
      case kMode_Operand2_R_ROR_R:
        break;
      case kMode_Offset_RI:
        *first_index += 2;
        return MemOperand(InputRegister(index + 0), InputInt32(index + 1));
      case kMode_Offset_RR:
        *first_index += 2;
        return MemOperand(InputRegister(index + 0), InputRegister(index + 1));
    }
    UNREACHABLE();
    return MemOperand(r0);
  }

  MemOperand InputOffset(int first_index = 0) {
    return InputOffset(&first_index);
  }

  MemOperand ToMemOperand(InstructionOperand* op) const {
    DCHECK(op != NULL);
    DCHECK(!op->IsRegister());
    DCHECK(!op->IsDoubleRegister());
    DCHECK(op->IsStackSlot() || op->IsDoubleStackSlot());
    // The linkage computes where all spill slots are located.
    FrameOffset offset = linkage()->GetFrameOffset(op->index(), frame(), 0);
    return MemOperand(offset.from_stack_pointer() ? sp : fp, offset.offset());
  }
};


namespace {

class OutOfLineLoadFloat32 FINAL : public OutOfLineCode {
 public:
  OutOfLineLoadFloat32(CodeGenerator* gen, SwVfpRegister result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() FINAL {
    __ vmov(result_, std::numeric_limits<float>::quiet_NaN());
  }

 private:
  SwVfpRegister const result_;
};


class OutOfLineLoadFloat64 FINAL : public OutOfLineCode {
 public:
  OutOfLineLoadFloat64(CodeGenerator* gen, DwVfpRegister result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() FINAL {
    __ vmov(result_, std::numeric_limits<double>::quiet_NaN(), kScratchReg);
  }

 private:
  DwVfpRegister const result_;
};


class OutOfLineLoadInteger FINAL : public OutOfLineCode {
 public:
  OutOfLineLoadInteger(CodeGenerator* gen, Register result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() FINAL { __ mov(result_, Operand::Zero()); }

 private:
  Register const result_;
};


Condition FlagsConditionToCondition(FlagsCondition condition) {
  switch (condition) {
    case kEqual:
      return eq;
    case kNotEqual:
      return ne;
    case kSignedLessThan:
      return lt;
    case kSignedGreaterThanOrEqual:
      return ge;
    case kSignedLessThanOrEqual:
      return le;
    case kSignedGreaterThan:
      return gt;
    case kUnsignedLessThan:
      return lo;
    case kUnsignedGreaterThanOrEqual:
      return hs;
    case kUnsignedLessThanOrEqual:
      return ls;
    case kUnsignedGreaterThan:
      return hi;
    case kOverflow:
      return vs;
    case kNotOverflow:
      return vc;
    case kUnorderedEqual:
    case kUnorderedNotEqual:
      break;
  }
  UNREACHABLE();
  return kNoCondition;
}

}  // namespace


#define ASSEMBLE_CHECKED_LOAD_FLOAT(width)                           \
  do {                                                               \
    auto result = i.OutputFloat##width##Register();                  \
    auto offset = i.InputRegister(0);                                \
    if (instr->InputAt(1)->IsRegister()) {                           \
      __ cmp(offset, i.InputRegister(1));                            \
    } else {                                                         \
      __ cmp(offset, i.InputImmediate(1));                           \
    }                                                                \
    auto ool = new (zone()) OutOfLineLoadFloat##width(this, result); \
    __ b(hs, ool->entry());                                          \
    __ vldr(result, i.InputOffset(2));                               \
    __ bind(ool->exit());                                            \
    DCHECK_EQ(LeaveCC, i.OutputSBit());                              \
  } while (0)


#define ASSEMBLE_CHECKED_LOAD_INTEGER(asm_instr)                \
  do {                                                          \
    auto result = i.OutputRegister();                           \
    auto offset = i.InputRegister(0);                           \
    if (instr->InputAt(1)->IsRegister()) {                      \
      __ cmp(offset, i.InputRegister(1));                       \
    } else {                                                    \
      __ cmp(offset, i.InputImmediate(1));                      \
    }                                                           \
    auto ool = new (zone()) OutOfLineLoadInteger(this, result); \
    __ b(hs, ool->entry());                                     \
    __ asm_instr(result, i.InputOffset(2));                     \
    __ bind(ool->exit());                                       \
    DCHECK_EQ(LeaveCC, i.OutputSBit());                         \
  } while (0)


#define ASSEMBLE_CHECKED_STORE_FLOAT(width)        \
  do {                                             \
    auto offset = i.InputRegister(0);              \
    if (instr->InputAt(1)->IsRegister()) {         \
      __ cmp(offset, i.InputRegister(1));          \
    } else {                                       \
      __ cmp(offset, i.InputImmediate(1));         \
    }                                              \
    auto value = i.InputFloat##width##Register(2); \
    __ vstr(value, i.InputOffset(3), lo);          \
    DCHECK_EQ(LeaveCC, i.OutputSBit());            \
  } while (0)


#define ASSEMBLE_CHECKED_STORE_INTEGER(asm_instr) \
  do {                                            \
    auto offset = i.InputRegister(0);             \
    if (instr->InputAt(1)->IsRegister()) {        \
      __ cmp(offset, i.InputRegister(1));         \
    } else {                                      \
      __ cmp(offset, i.InputImmediate(1));        \
    }                                             \
    auto value = i.InputRegister(2);              \
    __ asm_instr(value, i.InputOffset(3), lo);    \
    DCHECK_EQ(LeaveCC, i.OutputSBit());           \
  } while (0)


// Assembles an instruction after register allocation, producing machine code.
void CodeGenerator::AssembleArchInstruction(Instruction* instr) {
  ArmOperandConverter i(this, instr);

  switch (ArchOpcodeField::decode(instr->opcode())) {
    case kArchCallCodeObject: {
      EnsureSpaceForLazyDeopt();
      if (instr->InputAt(0)->IsImmediate()) {
        __ Call(Handle<Code>::cast(i.InputHeapObject(0)),
                RelocInfo::CODE_TARGET);
      } else {
        __ add(ip, i.InputRegister(0),
               Operand(Code::kHeaderSize - kHeapObjectTag));
        __ Call(ip);
      }
      AddSafepointAndDeopt(instr);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArchCallJSFunction: {
      EnsureSpaceForLazyDeopt();
      Register func = i.InputRegister(0);
      if (FLAG_debug_code) {
        // Check the function's context matches the context argument.
        __ ldr(kScratchReg, FieldMemOperand(func, JSFunction::kContextOffset));
        __ cmp(cp, kScratchReg);
        __ Assert(eq, kWrongFunctionContext);
      }
      __ ldr(ip, FieldMemOperand(func, JSFunction::kCodeEntryOffset));
      __ Call(ip);
      AddSafepointAndDeopt(instr);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArchJmp:
      AssembleArchJump(i.InputRpo(0));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArchNop:
      // don't emit code for nops.
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArchRet:
      AssembleReturn();
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArchStackPointer:
      __ mov(i.OutputRegister(), sp);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArchTruncateDoubleToI:
      __ TruncateDoubleToI(i.OutputRegister(), i.InputFloat64Register(0));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmAdd:
      __ add(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmAnd:
      __ and_(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
              i.OutputSBit());
      break;
    case kArmBic:
      __ bic(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmMul:
      __ mul(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
             i.OutputSBit());
      break;
    case kArmMla:
      __ mla(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
             i.InputRegister(2), i.OutputSBit());
      break;
    case kArmMls: {
      CpuFeatureScope scope(masm(), MLS);
      __ mls(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
             i.InputRegister(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmSmmul:
      __ smmul(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmSmmla:
      __ smmla(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.InputRegister(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmUmull:
      __ umull(i.OutputRegister(0), i.OutputRegister(1), i.InputRegister(0),
               i.InputRegister(1), i.OutputSBit());
      break;
    case kArmSdiv: {
      CpuFeatureScope scope(masm(), SUDIV);
      __ sdiv(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmUdiv: {
      CpuFeatureScope scope(masm(), SUDIV);
      __ udiv(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmMov:
      __ Move(i.OutputRegister(), i.InputOperand2(0), i.OutputSBit());
      break;
    case kArmMvn:
      __ mvn(i.OutputRegister(), i.InputOperand2(0), i.OutputSBit());
      break;
    case kArmOrr:
      __ orr(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmEor:
      __ eor(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmSub:
      __ sub(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmRsb:
      __ rsb(i.OutputRegister(), i.InputRegister(0), i.InputOperand2(1),
             i.OutputSBit());
      break;
    case kArmBfc: {
      CpuFeatureScope scope(masm(), ARMv7);
      __ bfc(i.OutputRegister(), i.InputInt8(1), i.InputInt8(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmUbfx: {
      CpuFeatureScope scope(masm(), ARMv7);
      __ ubfx(i.OutputRegister(), i.InputRegister(0), i.InputInt8(1),
              i.InputInt8(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmSxtb:
      __ sxtb(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmSxth:
      __ sxth(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmSxtab:
      __ sxtab(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.InputInt32(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmSxtah:
      __ sxtah(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.InputInt32(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmUxtb:
      __ uxtb(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmUxth:
      __ uxth(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmUxtab:
      __ uxtab(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.InputInt32(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmUxtah:
      __ uxtah(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.InputInt32(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmCmp:
      __ cmp(i.InputRegister(0), i.InputOperand2(1));
      DCHECK_EQ(SetCC, i.OutputSBit());
      break;
    case kArmCmn:
      __ cmn(i.InputRegister(0), i.InputOperand2(1));
      DCHECK_EQ(SetCC, i.OutputSBit());
      break;
    case kArmTst:
      __ tst(i.InputRegister(0), i.InputOperand2(1));
      DCHECK_EQ(SetCC, i.OutputSBit());
      break;
    case kArmTeq:
      __ teq(i.InputRegister(0), i.InputOperand2(1));
      DCHECK_EQ(SetCC, i.OutputSBit());
      break;
    case kArmVcmpF64:
      if (instr->InputAt(1)->IsDoubleRegister()) {
        __ VFPCompareAndSetFlags(i.InputFloat64Register(0),
                                 i.InputFloat64Register(1));
      } else {
        DCHECK(instr->InputAt(1)->IsImmediate());
        // 0.0 is the only immediate supported by vcmp instructions.
        DCHECK(i.InputDouble(1) == 0.0);
        __ VFPCompareAndSetFlags(i.InputFloat64Register(0), i.InputDouble(1));
      }
      DCHECK_EQ(SetCC, i.OutputSBit());
      break;
    case kArmVaddF64:
      __ vadd(i.OutputFloat64Register(), i.InputFloat64Register(0),
              i.InputFloat64Register(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVsubF64:
      __ vsub(i.OutputFloat64Register(), i.InputFloat64Register(0),
              i.InputFloat64Register(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVmulF64:
      __ vmul(i.OutputFloat64Register(), i.InputFloat64Register(0),
              i.InputFloat64Register(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVmlaF64:
      __ vmla(i.OutputFloat64Register(), i.InputFloat64Register(1),
              i.InputFloat64Register(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVmlsF64:
      __ vmls(i.OutputFloat64Register(), i.InputFloat64Register(1),
              i.InputFloat64Register(2));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVdivF64:
      __ vdiv(i.OutputFloat64Register(), i.InputFloat64Register(0),
              i.InputFloat64Register(1));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVmodF64: {
      // TODO(bmeurer): We should really get rid of this special instruction,
      // and generate a CallAddress instruction instead.
      FrameScope scope(masm(), StackFrame::MANUAL);
      __ PrepareCallCFunction(0, 2, kScratchReg);
      __ MovToFloatParameters(i.InputFloat64Register(0),
                              i.InputFloat64Register(1));
      __ CallCFunction(ExternalReference::mod_two_doubles_operation(isolate()),
                       0, 2);
      // Move the result in the double result register.
      __ MovFromFloatResult(i.OutputFloat64Register());
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVsqrtF64:
      __ vsqrt(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVfloorF64:
      __ vrintm(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVceilF64:
      __ vrintp(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVroundTruncateF64:
      __ vrintz(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVroundTiesAwayF64:
      __ vrinta(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVnegF64:
      __ vneg(i.OutputFloat64Register(), i.InputFloat64Register(0));
      break;
    case kArmVcvtF32F64: {
      __ vcvt_f32_f64(i.OutputFloat32Register(), i.InputFloat64Register(0));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVcvtF64F32: {
      __ vcvt_f64_f32(i.OutputFloat64Register(), i.InputFloat32Register(0));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVcvtF64S32: {
      SwVfpRegister scratch = kScratchDoubleReg.low();
      __ vmov(scratch, i.InputRegister(0));
      __ vcvt_f64_s32(i.OutputFloat64Register(), scratch);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVcvtF64U32: {
      SwVfpRegister scratch = kScratchDoubleReg.low();
      __ vmov(scratch, i.InputRegister(0));
      __ vcvt_f64_u32(i.OutputFloat64Register(), scratch);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVcvtS32F64: {
      SwVfpRegister scratch = kScratchDoubleReg.low();
      __ vcvt_s32_f64(scratch, i.InputFloat64Register(0));
      __ vmov(i.OutputRegister(), scratch);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVcvtU32F64: {
      SwVfpRegister scratch = kScratchDoubleReg.low();
      __ vcvt_u32_f64(scratch, i.InputFloat64Register(0));
      __ vmov(i.OutputRegister(), scratch);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmLdrb:
      __ ldrb(i.OutputRegister(), i.InputOffset());
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmLdrsb:
      __ ldrsb(i.OutputRegister(), i.InputOffset());
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmStrb: {
      int index = 0;
      MemOperand operand = i.InputOffset(&index);
      __ strb(i.InputRegister(index), operand);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmLdrh:
      __ ldrh(i.OutputRegister(), i.InputOffset());
      break;
    case kArmLdrsh:
      __ ldrsh(i.OutputRegister(), i.InputOffset());
      break;
    case kArmStrh: {
      int index = 0;
      MemOperand operand = i.InputOffset(&index);
      __ strh(i.InputRegister(index), operand);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmLdr:
      __ ldr(i.OutputRegister(), i.InputOffset());
      break;
    case kArmStr: {
      int index = 0;
      MemOperand operand = i.InputOffset(&index);
      __ str(i.InputRegister(index), operand);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVldrF32: {
      __ vldr(i.OutputFloat32Register(), i.InputOffset());
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVstrF32: {
      int index = 0;
      MemOperand operand = i.InputOffset(&index);
      __ vstr(i.InputFloat32Register(index), operand);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmVldrF64:
      __ vldr(i.OutputFloat64Register(), i.InputOffset());
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmVstrF64: {
      int index = 0;
      MemOperand operand = i.InputOffset(&index);
      __ vstr(i.InputFloat64Register(index), operand);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kArmPush:
      __ Push(i.InputRegister(0));
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    case kArmStoreWriteBarrier: {
      Register object = i.InputRegister(0);
      Register index = i.InputRegister(1);
      Register value = i.InputRegister(2);
      __ add(index, object, index);
      __ str(value, MemOperand(index));
      SaveFPRegsMode mode =
          frame()->DidAllocateDoubleRegisters() ? kSaveFPRegs : kDontSaveFPRegs;
      LinkRegisterStatus lr_status = kLRHasNotBeenSaved;
      __ RecordWrite(object, index, value, lr_status, mode);
      DCHECK_EQ(LeaveCC, i.OutputSBit());
      break;
    }
    case kCheckedLoadInt8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(ldrsb);
      break;
    case kCheckedLoadUint8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(ldrb);
      break;
    case kCheckedLoadInt16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(ldrsh);
      break;
    case kCheckedLoadUint16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(ldrh);
      break;
    case kCheckedLoadWord32:
      ASSEMBLE_CHECKED_LOAD_INTEGER(ldr);
      break;
    case kCheckedLoadFloat32:
      ASSEMBLE_CHECKED_LOAD_FLOAT(32);
      break;
    case kCheckedLoadFloat64:
      ASSEMBLE_CHECKED_LOAD_FLOAT(64);
      break;
    case kCheckedStoreWord8:
      ASSEMBLE_CHECKED_STORE_INTEGER(strb);
      break;
    case kCheckedStoreWord16:
      ASSEMBLE_CHECKED_STORE_INTEGER(strh);
      break;
    case kCheckedStoreWord32:
      ASSEMBLE_CHECKED_STORE_INTEGER(str);
      break;
    case kCheckedStoreFloat32:
      ASSEMBLE_CHECKED_STORE_FLOAT(32);
      break;
    case kCheckedStoreFloat64:
      ASSEMBLE_CHECKED_STORE_FLOAT(64);
      break;
  }
}


// Assembles branches after an instruction.
void CodeGenerator::AssembleArchBranch(Instruction* instr, BranchInfo* branch) {
  ArmOperandConverter i(this, instr);
  Label* tlabel = branch->true_label;
  Label* flabel = branch->false_label;
  Condition cc = FlagsConditionToCondition(branch->condition);
  __ b(cc, tlabel);
  if (!branch->fallthru) __ b(flabel);  // no fallthru to flabel.
}


void CodeGenerator::AssembleArchJump(BasicBlock::RpoNumber target) {
  if (!IsNextInAssemblyOrder(target)) __ b(GetLabel(target));
}


// Assembles boolean materializations after an instruction.
void CodeGenerator::AssembleArchBoolean(Instruction* instr,
                                        FlagsCondition condition) {
  ArmOperandConverter i(this, instr);

  // Materialize a full 32-bit 1 or 0 value. The result register is always the
  // last output of the instruction.
  DCHECK_NE(0, instr->OutputCount());
  Register reg = i.OutputRegister(instr->OutputCount() - 1);
  Condition cc = FlagsConditionToCondition(condition);
  __ mov(reg, Operand(0));
  __ mov(reg, Operand(1), LeaveCC, cc);
}


void CodeGenerator::AssembleDeoptimizerCall(int deoptimization_id) {
  Address deopt_entry = Deoptimizer::GetDeoptimizationEntry(
      isolate(), deoptimization_id, Deoptimizer::LAZY);
  __ Call(deopt_entry, RelocInfo::RUNTIME_ENTRY);
}


void CodeGenerator::AssemblePrologue() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    bool saved_pp;
    if (FLAG_enable_ool_constant_pool) {
      __ Push(lr, fp, pp);
      // Adjust FP to point to saved FP.
      __ sub(fp, sp, Operand(StandardFrameConstants::kConstantPoolOffset));
      saved_pp = true;
    } else {
      __ Push(lr, fp);
      __ mov(fp, sp);
      saved_pp = false;
    }
    const RegList saves = descriptor->CalleeSavedRegisters();
    if (saves != 0 || saved_pp) {
      // Save callee-saved registers.
      int register_save_area_size = saved_pp ? kPointerSize : 0;
      for (int i = Register::kNumRegisters - 1; i >= 0; i--) {
        if (!((1 << i) & saves)) continue;
        register_save_area_size += kPointerSize;
      }
      frame()->SetRegisterSaveAreaSize(register_save_area_size);
      __ stm(db_w, sp, saves);
    }
  } else if (descriptor->IsJSFunctionCall()) {
    CompilationInfo* info = this->info();
    __ Prologue(info->IsCodePreAgingActive());
    frame()->SetRegisterSaveAreaSize(
        StandardFrameConstants::kFixedFrameSizeFromFp);
  } else {
    __ StubPrologue();
    frame()->SetRegisterSaveAreaSize(
        StandardFrameConstants::kFixedFrameSizeFromFp);
  }
  int stack_slots = frame()->GetSpillSlotCount();

  if (info()->is_osr()) {
    // TurboFan OSR-compiled functions cannot be entered directly.
    __ Abort(kShouldNotDirectlyEnterOsrFunction);

    // Unoptimized code jumps directly to this entrypoint while the unoptimized
    // frame is still on the stack. Optimized code uses OSR values directly from
    // the unoptimized frame. Thus, all that needs to be done is to allocate the
    // remaining stack slots.
    if (FLAG_code_comments) __ RecordComment("-- OSR entrypoint --");
    osr_pc_offset_ = __ pc_offset();
    int unoptimized_slots =
        static_cast<int>(OsrHelper(info()).UnoptimizedFrameSlots());
    DCHECK(stack_slots >= unoptimized_slots);
    stack_slots -= unoptimized_slots;
  }

  if (stack_slots > 0) {
    __ sub(sp, sp, Operand(stack_slots * kPointerSize));
  }
}


void CodeGenerator::AssembleReturn() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    if (frame()->GetRegisterSaveAreaSize() > 0) {
      // Remove this frame's spill slots first.
      int stack_slots = frame()->GetSpillSlotCount();
      if (stack_slots > 0) {
        __ add(sp, sp, Operand(stack_slots * kPointerSize));
      }
      // Restore registers.
      const RegList saves = descriptor->CalleeSavedRegisters();
      if (saves != 0) {
        __ ldm(ia_w, sp, saves);
      }
    }
    __ LeaveFrame(StackFrame::MANUAL);
    __ Ret();
  } else {
    __ LeaveFrame(StackFrame::MANUAL);
    int pop_count = descriptor->IsJSFunctionCall()
                        ? static_cast<int>(descriptor->JSParameterCount())
                        : 0;
    __ Drop(pop_count);
    __ Ret();
  }
}


void CodeGenerator::AssembleMove(InstructionOperand* source,
                                 InstructionOperand* destination) {
  ArmOperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    Register src = g.ToRegister(source);
    if (destination->IsRegister()) {
      __ mov(g.ToRegister(destination), src);
    } else {
      __ str(src, g.ToMemOperand(destination));
    }
  } else if (source->IsStackSlot()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    MemOperand src = g.ToMemOperand(source);
    if (destination->IsRegister()) {
      __ ldr(g.ToRegister(destination), src);
    } else {
      Register temp = kScratchReg;
      __ ldr(temp, src);
      __ str(temp, g.ToMemOperand(destination));
    }
  } else if (source->IsConstant()) {
    Constant src = g.ToConstant(source);
    if (destination->IsRegister() || destination->IsStackSlot()) {
      Register dst =
          destination->IsRegister() ? g.ToRegister(destination) : kScratchReg;
      switch (src.type()) {
        case Constant::kInt32:
          __ mov(dst, Operand(src.ToInt32()));
          break;
        case Constant::kInt64:
          UNREACHABLE();
          break;
        case Constant::kFloat32:
          __ Move(dst,
                  isolate()->factory()->NewNumber(src.ToFloat32(), TENURED));
          break;
        case Constant::kFloat64:
          __ Move(dst,
                  isolate()->factory()->NewNumber(src.ToFloat64(), TENURED));
          break;
        case Constant::kExternalReference:
          __ mov(dst, Operand(src.ToExternalReference()));
          break;
        case Constant::kHeapObject:
          __ Move(dst, src.ToHeapObject());
          break;
        case Constant::kRpoNumber:
          UNREACHABLE();  // TODO(dcarney): loading RPO constants on arm.
          break;
      }
      if (destination->IsStackSlot()) __ str(dst, g.ToMemOperand(destination));
    } else if (src.type() == Constant::kFloat32) {
      if (destination->IsDoubleStackSlot()) {
        MemOperand dst = g.ToMemOperand(destination);
        __ mov(ip, Operand(bit_cast<int32_t>(src.ToFloat32())));
        __ str(ip, dst);
      } else {
        SwVfpRegister dst = g.ToFloat32Register(destination);
        __ vmov(dst, src.ToFloat32());
      }
    } else {
      DCHECK_EQ(Constant::kFloat64, src.type());
      DwVfpRegister dst = destination->IsDoubleRegister()
                              ? g.ToFloat64Register(destination)
                              : kScratchDoubleReg;
      __ vmov(dst, src.ToFloat64(), kScratchReg);
      if (destination->IsDoubleStackSlot()) {
        __ vstr(dst, g.ToMemOperand(destination));
      }
    }
  } else if (source->IsDoubleRegister()) {
    DwVfpRegister src = g.ToDoubleRegister(source);
    if (destination->IsDoubleRegister()) {
      DwVfpRegister dst = g.ToDoubleRegister(destination);
      __ Move(dst, src);
    } else {
      DCHECK(destination->IsDoubleStackSlot());
      __ vstr(src, g.ToMemOperand(destination));
    }
  } else if (source->IsDoubleStackSlot()) {
    DCHECK(destination->IsDoubleRegister() || destination->IsDoubleStackSlot());
    MemOperand src = g.ToMemOperand(source);
    if (destination->IsDoubleRegister()) {
      __ vldr(g.ToDoubleRegister(destination), src);
    } else {
      DwVfpRegister temp = kScratchDoubleReg;
      __ vldr(temp, src);
      __ vstr(temp, g.ToMemOperand(destination));
    }
  } else {
    UNREACHABLE();
  }
}


void CodeGenerator::AssembleSwap(InstructionOperand* source,
                                 InstructionOperand* destination) {
  ArmOperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister()) {
    // Register-register.
    Register temp = kScratchReg;
    Register src = g.ToRegister(source);
    if (destination->IsRegister()) {
      Register dst = g.ToRegister(destination);
      __ Move(temp, src);
      __ Move(src, dst);
      __ Move(dst, temp);
    } else {
      DCHECK(destination->IsStackSlot());
      MemOperand dst = g.ToMemOperand(destination);
      __ mov(temp, src);
      __ ldr(src, dst);
      __ str(temp, dst);
    }
  } else if (source->IsStackSlot()) {
    DCHECK(destination->IsStackSlot());
    Register temp_0 = kScratchReg;
    SwVfpRegister temp_1 = kScratchDoubleReg.low();
    MemOperand src = g.ToMemOperand(source);
    MemOperand dst = g.ToMemOperand(destination);
    __ ldr(temp_0, src);
    __ vldr(temp_1, dst);
    __ str(temp_0, dst);
    __ vstr(temp_1, src);
  } else if (source->IsDoubleRegister()) {
    DwVfpRegister temp = kScratchDoubleReg;
    DwVfpRegister src = g.ToDoubleRegister(source);
    if (destination->IsDoubleRegister()) {
      DwVfpRegister dst = g.ToDoubleRegister(destination);
      __ Move(temp, src);
      __ Move(src, dst);
      __ Move(dst, temp);
    } else {
      DCHECK(destination->IsDoubleStackSlot());
      MemOperand dst = g.ToMemOperand(destination);
      __ Move(temp, src);
      __ vldr(src, dst);
      __ vstr(temp, dst);
    }
  } else if (source->IsDoubleStackSlot()) {
    DCHECK(destination->IsDoubleStackSlot());
    Register temp_0 = kScratchReg;
    DwVfpRegister temp_1 = kScratchDoubleReg;
    MemOperand src0 = g.ToMemOperand(source);
    MemOperand src1(src0.rn(), src0.offset() + kPointerSize);
    MemOperand dst0 = g.ToMemOperand(destination);
    MemOperand dst1(dst0.rn(), dst0.offset() + kPointerSize);
    __ vldr(temp_1, dst0);  // Save destination in temp_1.
    __ ldr(temp_0, src0);   // Then use temp_0 to copy source to destination.
    __ str(temp_0, dst0);
    __ ldr(temp_0, src1);
    __ str(temp_0, dst1);
    __ vstr(temp_1, src0);
  } else {
    // No other combinations are possible.
    UNREACHABLE();
  }
}


void CodeGenerator::AddNopForSmiCodeInlining() {
  // On 32-bit ARM we do not insert nops for inlined Smi code.
}


void CodeGenerator::EnsureSpaceForLazyDeopt() {
  int space_needed = Deoptimizer::patch_size();
  if (!info()->IsStub()) {
    // Ensure that we have enough space after the previous lazy-bailout
    // instruction for patching the code here.
    int current_pc = masm()->pc_offset();
    if (current_pc < last_lazy_deopt_pc_ + space_needed) {
      // Block literal pool emission for duration of padding.
      v8::internal::Assembler::BlockConstPoolScope block_const_pool(masm());
      int padding_size = last_lazy_deopt_pc_ + space_needed - current_pc;
      DCHECK_EQ(0, padding_size % v8::internal::Assembler::kInstrSize);
      while (padding_size > 0) {
        __ nop();
        padding_size -= v8::internal::Assembler::kInstrSize;
      }
    }
  }
  MarkLazyDeoptSite();
}

#undef __

}  // namespace compiler
}  // namespace internal
}  // namespace v8


#endif  // V8_TARGET_ARCH_ARM
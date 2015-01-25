#include "src/v8.h"

#if V8_TARGET_ARCH_PPC

// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#if V8_TARGET_ARCH_PPC

#include "src/assembler.h"
#include "src/frames.h"
#include "src/macro-assembler.h"

#include "src/ppc/assembler-ppc.h"
#include "src/ppc/assembler-ppc-inl.h"
#include "src/ppc/macro-assembler-ppc.h"

namespace v8 {
namespace internal {


Register JavaScriptFrame::fp_register() { return v8::internal::fp; }
Register JavaScriptFrame::context_register() { return cp; }
Register JavaScriptFrame::constant_pool_pointer_register() {
#if V8_OOL_CONSTANT_POOL
  DCHECK(FLAG_enable_ool_constant_pool);
  return kConstantPoolRegister;
#else
  UNREACHABLE();
  return no_reg;
#endif
}


Register StubFailureTrampolineFrame::fp_register() { return v8::internal::fp; }
Register StubFailureTrampolineFrame::context_register() { return cp; }
Register StubFailureTrampolineFrame::constant_pool_pointer_register() {
#if V8_OOL_CONSTANT_POOL
  DCHECK(FLAG_enable_ool_constant_pool);
  return kConstantPoolRegister;
#else
  UNREACHABLE();
  return no_reg;
#endif
}


Object*& ExitFrame::constant_pool_slot() const {
#if V8_OOL_CONSTANT_POOL
  DCHECK(FLAG_enable_ool_constant_pool);
  const int offset = ExitFrameConstants::kConstantPoolOffset;
  return Memory::Object_at(fp() + offset);
#else
  UNREACHABLE();
  return Memory::Object_at(NULL);
#endif
}
}
}  // namespace v8::internal

#endif  // V8_TARGET_ARCH_PPC


#endif  // V8_TARGET_ARCH_PPC
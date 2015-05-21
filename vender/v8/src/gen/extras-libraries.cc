// Copyright 2011 Google Inc. All Rights Reserved.

// This file was generated from .js source files by GYP.  If you
// want to make changes to this file you should either change the
// javascript source files or the GYP script.

#include "src/v8.h"
#include "src/snapshot/natives.h"
#include "src/utils.h"

namespace v8 {
namespace internal {

  static const char sources[] = { 10, 40, 102, 117, 110, 99, 116, 105, 111, 110, 32, 40, 103, 108, 111, 98, 97,
108, 44, 32, 101, 120, 112, 111, 114, 116, 115, 41, 32, 123, 10, 32, 32, 39,
117, 115, 101, 32, 115, 116, 114, 105, 99, 116, 39, 59, 10, 32, 32, 101, 120,
112, 111, 114, 116, 115, 46, 116, 101, 115, 116, 69, 120, 116, 114, 97, 83, 104,
111, 117, 108, 100, 82, 101, 116, 117, 114, 110, 70, 105, 118, 101, 32, 61, 32,
102, 117, 110, 99, 116, 105, 111, 110, 32, 40, 41, 32, 123, 10, 32, 32, 32, 32,
114, 101, 116, 117, 114, 110, 32, 53, 59, 10, 32, 32, 125, 59, 10, 125, 41, 10 };

  template <>
  int NativesCollection<EXTRAS>::GetBuiltinsCount() {
    return 1;
  }

  template <>
  int NativesCollection<EXTRAS>::GetDebuggerCount() {
    return 0;
  }

  template <>
  int NativesCollection<EXTRAS>::GetIndex(const char* name) {
    if (strcmp(name, "test-extra") == 0) return 0;
    return -1;
  }

  template <>
  Vector<const char> NativesCollection<EXTRAS>::GetScriptSource(int index) {
    if (index == 0) return Vector<const char>(sources + 0, 121);
    return Vector<const char>("", 0);
  }

  template <>
  Vector<const char> NativesCollection<EXTRAS>::GetScriptName(int index) {
    if (index == 0) return Vector<const char>("native test-extra.js", 20);
    return Vector<const char>("", 0);
  }

  template <>
  Vector<const char> NativesCollection<EXTRAS>::GetScriptsSource() {
    return Vector<const char>(sources, 121);
  }
}  // internal
}  // v8

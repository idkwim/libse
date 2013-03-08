// Copyright 2013, Alex Horn. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef LIBSE_CONCURRENT_VAR_H_
#define LIBSE_CONCURRENT_VAR_H_

#include <utility>
#include <memory>

#include "concurrent/event.h"
#include "concurrent/instr.h"
#include "concurrent/recorder.h"

namespace se {

template<typename T> class ConcurrentVar;

template<typename T>
std::unique_ptr<ReadInstr<T>> alloc_read_instr(const ConcurrentVar<T>&);

/// Concrete or symbolic shared variable whose type is `T`

/// The lifetime of a ConcurrentVar object must span all threads that
/// can access it. Therefore, ConcurrentVar objects should be generally
/// allocated statically. This use case justifies why a ConcurrentVar
/// object is always initialized to zero by the main thread unless
/// another initial value is explicitly given to the constructor.
/// Noteworthy, this value may also be symbolic.
///
/// \remark Usually a static variable
template<typename T>
class ConcurrentVar {
private:
  std::shared_ptr<WriteEvent<T>> m_event_ptr;

  static std::unique_ptr<ReadInstr<T>> zero_literal_ptr() {
    return std::unique_ptr<ReadInstr<T>>(new LiteralReadInstr<T>());
  }

public:
  ConcurrentVar(const MemoryAddr& addr) :
    m_event_ptr(new WriteEvent<T>(/* main thread */ 0, addr,
      zero_literal_ptr())) {}

  ConcurrentVar() :m_event_ptr(new WriteEvent<T>(/* main thread */ 0,
    reinterpret_cast<uintptr_t>(this), zero_literal_ptr())) {}

  const MemoryAddr& addr() const { return m_event_ptr->addr(); }
  const ReadInstr<T>& instr_ref() const { return m_event_ptr->instr_ref(); }

  ConcurrentVar<T>& operator=(const ConcurrentVar<T>& other) {
    return operator=(alloc_read_instr(other));
  }

  ConcurrentVar<T>& operator=(std::unique_ptr<ReadInstr<T>> instr_ptr) {
    m_event_ptr = recorder_ptr()->instr(addr(), std::move(instr_ptr));
    return *this;
  }
};

}

#endif

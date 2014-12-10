/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <folly/wangle/Try.h>
#include <folly/wangle/Future.h>

namespace folly { namespace wangle {

// forward declaration
template <class T> class Future;

template <class T>
class Promise {
public:
  Promise();
  ~Promise();

  // not copyable
  Promise(Promise const&) = delete;
  Promise& operator=(Promise const&) = delete;

  // movable
  Promise(Promise<T>&&);
  Promise& operator=(Promise<T>&&);

  /** Return a Future tied to the shared state. This can be called only
    once, thereafter Future already retrieved exception will be raised. */
  Future<T> getFuture();

  /** Fulfil the Promise with an exception_ptr, e.g.
    try {
      ...
    } catch (...) {
      p.setException(std::current_exception());
    }
    */
  void setException(std::exception_ptr const&);

  /** Fulfil the Promise with an exception type E, which can be passed to
    std::make_exception_ptr(). Useful for originating exceptions. If you
    caught an exception the exception_ptr form is more appropriate.
    */
  template <class E> void setException(E const&);

  /** Fulfil this Promise (only for Promise<void>) */
  void setValue();

  /** Set the value (use perfect forwarding for both move and copy) */
  template <class M>
  void setValue(M&& value);

  void fulfilTry(Try<T>&& t);

  /** Fulfil this Promise with the result of a function that takes no
    arguments and returns something implicitly convertible to T.
    Captures exceptions. e.g.

    p.fulfil([] { do something that may throw; return a T; });
  */
  template <class F>
  void fulfil(F&& func);

private:
  typedef typename Future<T>::statePtr statePtr;

  // Whether the Future has been retrieved (a one-time operation).
  bool retrieved_;

  // shared state object
  statePtr state_;

  void throwIfFulfilled();
  void throwIfRetrieved();
  void detach();
};

}}

#include <folly/wangle/Promise-inl.h>

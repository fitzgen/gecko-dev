/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef js_IdValuePair_h
#define js_IdValuePair_h

#include "js/GCVector.h"
#include "js/Id.h"
#include "js/Value.h"

class JS_PUBLIC_API JSTracer;

namespace JS {

struct IdValuePair {
  JS::Value value;
  jsid id;

  IdValuePair() : value(JS::UndefinedValue()), id(JSID_VOID) {}
  explicit IdValuePair(jsid idArg) : value(JS::UndefinedValue()), id(idArg) {}
  IdValuePair(jsid idArg, const Value& valueArg) : value(valueArg), id(idArg) {}

  void trace(JSTracer* trc);
};

using IdValueVector = JS::GCVector<IdValuePair, 8>;

} /* namespace JS */

#endif /* js_IdValuePair_h */

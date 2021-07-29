/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "jsapi-tests/tests.h"

using namespace JS;

static bool CALLED_FOO = false;
static bool CALLED_BAR = false;

static bool foo(JSContext* cx, unsigned argc, Value* vp) {
  MOZ_ASSERT(!CALLED_FOO);
  CALLED_FOO = true;
  return true;
}

static bool bar(JSContext* cx, unsigned argc, Value* vp) {
  MOZ_ASSERT(!CALLED_BAR);
  CALLED_BAR = true;
  return true;
}

BEGIN_TEST(testCreateModule) {
  // Create `foo` id and value.

  Rooted<JSString*> fooString(cx, JS_NewStringCopyZ(cx, "foo"));
  CHECK(fooString);
  Rooted<jsid> fooId(cx);
  CHECK(JS_StringToId(cx, fooString, &fooId));

  Rooted<JSFunction*> fooJsFunc(cx, JS_NewFunction(cx, foo, 0, 0, "foo"));
  CHECK(fooJsFunc);
  Rooted<JSObject*> fooObj(cx, JS_GetFunctionObject(fooJsFunc));
  CHECK(fooObj);

  // Create `bar` id and value.

  Rooted<JSString*> barString(cx, JS_NewStringCopyZ(cx, "bar"));
  CHECK(barString);
  Rooted<jsid> barId(cx);
  CHECK(JS_StringToId(cx, barString, &barId));

  Rooted<JSFunction*> barJsFunc(cx, JS_NewFunction(cx, bar, 0, 0, "bar"));
  CHECK(barJsFunc);
  Rooted<JSObject*> barObj(cx, JS_GetFunctionObject(barJsFunc));
  CHECK(barObj);

  // Create a module exporting `foo` and `bar`.

  Rooted<IdValueVector> exports(cx, cx);
  CHECK(exports.append(IdValuePair(fooId, ObjectValue(*fooObj))));
  CHECK(exports.append(IdValuePair(barId, ObjectValue(*barObj))));

  Rooted<JSObject*> module(cx, CreateModule(cx, exports));
  CHECK(module);

  CHECK(ModuleInstantiate(cx, module));

  Rooted<Value> rval(cx);
  CHECK(ModuleEvaluate(cx, module, &rval));

  // Read the `foo` and `bar` exports back out from the module.

  Rooted<JSAtom*> fooAtom(cx, AtomizeString(cx, fooString));
  CHECK(fooAtom);
  Rooted<Value> fooVal(cx);
  bool hasFoo = false;
  CHECK(GetModuleExport(cx, module, fooAtom, &fooVal, &hasFoo));
  CHECK(hasFoo);

  Rooted<JSAtom*> barAtom(cx, AtomizeString(cx, barString));
  CHECK(barAtom);
  Rooted<Value> barVal(cx);
  bool hasBar = false;
  CHECK(GetModuleExport(cx, module, barAtom, &barVal, &hasBar));
  CHECK(hasBar);

  // Call `foo` and `bar`.

  Rooted<JSFunction*> fooFunc(cx, &fooVal.toObject().as<JSFunction>());
  CHECK(JS::Call(cx, fooObj, fooFunc, JS::HandleValueArray::empty(), &rval));
  CHECK(CALLED_FOO);
  CHECK(!CALLED_BAR);

  Rooted<JSFunction*> barFunc(cx, &barVal.toObject().as<JSFunction>());
  CHECK(JS::Call(cx, barObj, barFunc, JS::HandleValueArray::empty(), &rval));
  CHECK(CALLED_FOO);
  CHECK(CALLED_BAR);

  return true;
}
END_TEST(testCreateModule)

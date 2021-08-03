/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "js/Modules.h"
#include "js/SourceText.h"
#include "jsapi-tests/tests.h"

using namespace JS;
using namespace js;

BEGIN_TEST(testGetModuleExport) {
  // Compile, instantiate, and eval a module.

  const char* moduleSource =
      "export var answer = 42;\n"
      "export function setAnswer(x) { answer = x; }\n";

  CompileOptions compileOpts(cx);
  SourceText<mozilla::Utf8Unit> sourceText;
  CHECK(sourceText.init(cx, moduleSource, strlen(moduleSource),
                        SourceOwnership::Borrowed));

  RootedObject module(cx, CompileModule(cx, compileOpts, sourceText));
  CHECK(module);

  CHECK(ModuleInstantiate(cx, module));

  RootedValue result(cx);
  CHECK(ModuleEvaluate(cx, module, &result));

  // Get the `answer` export.

  const char16_t* answerStr = u"answer";
  RootedAtom answerAtom(cx, AtomizeChars(cx, answerStr, 6));
  CHECK(answerAtom);

  RootedValue answer(cx);
  bool hasAnswer = false;
  CHECK(GetModuleExport(cx, module, answerAtom, &answer, &hasAnswer));
  CHECK(hasAnswer);
  CHECK(answer.isInt32());
  CHECK(answer.toInt32() == 42);

  // Get the `setAnswer` export.

  const char16_t* setAnswerStr = u"setAnswer";
  RootedAtom setAnswerAtom(cx, AtomizeChars(cx, setAnswerStr, 9));
  CHECK(setAnswerAtom);

  RootedValue setAnswer(cx);
  bool hasSetAnswer = false;
  CHECK(GetModuleExport(cx, module, setAnswerAtom, &setAnswer, &hasSetAnswer));
  CHECK(hasSetAnswer);

  RootedFunction setAnswerFn(cx, JS_ValueToFunction(cx, setAnswer));
  CHECK(setAnswerFn);

  // Update `answer`. Getting `answer` again should return new value.

  RootedValue arg(cx, Int32Value(36));
  CHECK(Call(cx, global, setAnswerFn, HandleValueArray(arg), &result));

  hasAnswer = false;
  CHECK(GetModuleExport(cx, module, answerAtom, &answer, &hasAnswer));
  CHECK(hasAnswer);
  CHECK(answer.isInt32());
  CHECK(answer.toInt32() == 36);

  // Getting something that isn't exported fails.

  const char16_t* notExportedStr = u"notExported";
  RootedAtom notExportedAtom(cx, AtomizeChars(cx, notExportedStr, 11));
  CHECK(notExportedAtom);

  RootedValue notExported(cx);
  bool hasNotExported = true;
  CHECK(GetModuleExport(cx, module, notExportedAtom, &notExported,
                        &hasNotExported));
  CHECK(!hasNotExported);
  CHECK(notExported.isUndefined());

  return true;
}
END_TEST(testGetModuleExport)

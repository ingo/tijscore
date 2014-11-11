/**
 * Appcelerator Titanium License
 * This source code and all modifications done by Appcelerator
 * are licensed under the Apache Public License (version 2) and
 * are Copyright (c) 2009-2014 by Appcelerator, Inc.
 */

/*
 * Copyright (C) 2012 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef JSWithScope_h
#define JSWithScope_h

#include "JSGlobalObject.h"

namespace TI {

class JSWithScope : public JSScope {
public:
    typedef JSScope Base;

    static JSWithScope* create(ExecState* exec, JSObject* object)
    {
        JSWithScope* withScope = new (NotNull, allocateCell<JSWithScope>(*exec->heap())) JSWithScope(exec, object);
        withScope->finishCreation(exec->vm());
        return withScope;
    }

    static JSWithScope* create(ExecState* exec, JSObject* object, JSScope* next)
    {
        JSWithScope* withScope = new (NotNull, allocateCell<JSWithScope>(*exec->heap())) JSWithScope(exec, object, next);
        withScope->finishCreation(exec->vm());
        return withScope;
    }

    JSObject* object() { return m_object.get(); }

    static void visitChildren(JSCell*, SlotVisitor&);

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, TiValue proto)
    {
        return Structure::create(vm, globalObject, proto, TypeInfo(WithScopeType, StructureFlags), info());
    }

    DECLARE_EXPORT_INFO;

protected:
    static const unsigned StructureFlags = OverridesVisitChildren | Base::StructureFlags;

private:
    JSWithScope(ExecState* exec, JSObject* object)
        : Base(
            exec->vm(),
            exec->lexicalGlobalObject()->withScopeStructure(),
            exec->scope()
        )
        , m_object(exec->vm(), this, object)
    {
    }

    JSWithScope(ExecState* exec, JSObject* object, JSScope* next)
        : Base(
            exec->vm(),
            exec->lexicalGlobalObject()->withScopeStructure(),
            next
        )
        , m_object(exec->vm(), this, object)
    {
    }

    WriteBarrier<JSObject> m_object;
};

} // namespace TI

#endif // JSWithScope_h
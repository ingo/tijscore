/**
 * Appcelerator Titanium License
 * This source code and all modifications done by Appcelerator
 * are licensed under the Apache Public License (version 2) and
 * are Copyright (c) 2009-2012 by Appcelerator, Inc.
 */

/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Contains the necessary helper structures to make possible expose
// C/C++ functions to Ti environment.

#include "config.h"

#include "qscriptfunction_p.h"

static void qt_NativeFunction_finalize(TiObjectRef object)
{
    void* priv = TiObjectGetPrivate(object);
    delete reinterpret_cast<QNativeFunctionData*>(priv);
}

static TiValueRef qt_NativeFunction_callAsFunction(TiContextRef context, TiObjectRef object, TiObjectRef thisObject, size_t argumentCount, const TiValueRef arguments[], TiValueRef* exception)
{
    QNativeFunctionData* data = reinterpret_cast<QNativeFunctionData*>(TiObjectGetPrivate(object));

    // TODO: build a QScriptContext and use it in the native call.
    QScriptContext* scriptContext = 0;
    Q_UNUSED(context);
    Q_UNUSED(thisObject);
    Q_UNUSED(argumentCount);
    Q_UNUSED(arguments);
    Q_UNUSED(exception);

    QScriptEnginePrivate* engine = data->engine;
    QScriptValuePrivate* result = QScriptValuePrivate::get(data->fun(scriptContext, QScriptEnginePrivate::get(engine)));
    if (!result->isValid()) {
        qWarning("Invalid value returned from native function, returning undefined value instead.");
        return engine->makeTiValue(QScriptValue::UndefinedValue);
    }

    // Make sure that the result will be assigned to the correct engine.
    if (!result->engine()) {
        Q_ASSERT(result->isValid());
        result->assignEngine(engine);
    } else if (result->engine() != engine) {
        qWarning("Value from different engine returned from native function, returning undefined value instead.");
        return engine->makeTiValue(QScriptValue::UndefinedValue);
    }

    return *result;
}

TiClassDefinition qt_NativeFunctionClass = {
    0,                                     // version
    kTiClassAttributeNoAutomaticPrototype, // attributes

    "",                         // className
    0,                          // parentClass

    0,                          // staticValues
    0,                          // staticFunctions

    0,                                // initialize
    qt_NativeFunction_finalize,       // finalize
    0,                                // hasProperty
    0,                                // getProperty
    0,                                // setProperty
    0,                                // deleteProperty
    0,                                // getPropertyNames
    qt_NativeFunction_callAsFunction, // callAsFunction
    0,                                // callAsConstructor
    0,                                // hasInstance
    0                                 // convertToType
};

static void qt_NativeFunctionWithArg_finalize(TiObjectRef object)
{
    void* priv = TiObjectGetPrivate(object);
    delete reinterpret_cast<QNativeFunctionWithArgData*>(priv);
}

static TiValueRef qt_NativeFunctionWithArg_callAsFunction(TiContextRef context, TiObjectRef object, TiObjectRef thisObject, size_t argumentCount, const TiValueRef arguments[], TiValueRef* exception)
{
    QNativeFunctionWithArgData* data = reinterpret_cast<QNativeFunctionWithArgData*>(TiObjectGetPrivate(object));

    // TODO: build a QScriptContext and use it in the native call.
    QScriptContext* scriptContext = 0;
    Q_UNUSED(context);
    Q_UNUSED(thisObject);
    Q_UNUSED(argumentCount);
    Q_UNUSED(arguments);
    Q_UNUSED(exception);

    QScriptEnginePrivate* engine = data->engine;
    QScriptValuePrivate* result = QScriptValuePrivate::get(data->fun(scriptContext, QScriptEnginePrivate::get(engine), data->arg));
    if (!result->isValid()) {
        qWarning("Invalid value returned from native function, returning undefined value instead.");
        return engine->makeTiValue(QScriptValue::UndefinedValue);
    }

    // Make sure that the result will be assigned to the correct engine.
    if (!result->engine()) {
        Q_ASSERT(result->isValid());
        result->assignEngine(engine);
    } else if (result->engine() != engine) {
        qWarning("Value from different engine returned from native function, returning undefined value instead.");
        return engine->makeTiValue(QScriptValue::UndefinedValue);
    }

    return *result;
}

TiClassDefinition qt_NativeFunctionWithArgClass = {
    0,                                     // version
    kTiClassAttributeNoAutomaticPrototype, // attributes

    "",                         // className
    0,                          // parentClass

    0,                          // staticValues
    0,                          // staticFunctions

    0,                                       // initialize
    qt_NativeFunctionWithArg_finalize,       // finalize
    0,                                       // hasProperty
    0,                                       // getProperty
    0,                                       // setProperty
    0,                                       // deleteProperty
    0,                                       // getPropertyNames
    qt_NativeFunctionWithArg_callAsFunction, // callAsFunction
    0,                                       // callAsConstructor
    0,                                       // hasInstance
    0                                        // convertToType
};

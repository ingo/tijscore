/**
 * Appcelerator Titanium License
 * This source code and all modifications done by Appcelerator
 * are licensed under the Apache Public License (version 2) and
 * are Copyright (c) 2009-2012 by Appcelerator, Inc.
 */

/*
 * Copyright (C) 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "TiPropertyNameIterator.h"

#include "TiGlobalObject.h"

namespace TI {

ASSERT_CLASS_FITS_IN_CELL(TiPropertyNameIterator);

const ClassInfo TiPropertyNameIterator::s_info = { "TiPropertyNameIterator", 0, 0, 0 };

inline TiPropertyNameIterator::TiPropertyNameIterator(TiExcState* exec, PropertyNameArrayData* propertyNameArrayData, size_t numCacheableSlots)
    : TiCell(exec->globalData(), exec->globalData().propertyNameIteratorStructure.get())
    , m_numCacheableSlots(numCacheableSlots)
    , m_jsStringsSize(propertyNameArrayData->propertyNameVector().size())
    , m_jsStrings(adoptArrayPtr(new WriteBarrier<Unknown>[m_jsStringsSize]))
{
    PropertyNameArrayData::PropertyNameVector& propertyNameVector = propertyNameArrayData->propertyNameVector();
    for (size_t i = 0; i < m_jsStringsSize; ++i)
        m_jsStrings[i].set(exec->globalData(), this, jsOwnedString(exec, propertyNameVector[i].ustring()));
}

TiPropertyNameIterator* TiPropertyNameIterator::create(TiExcState* exec, TiObject* o)
{
    ASSERT(!o->structure()->enumerationCache() ||
            o->structure()->enumerationCache()->cachedStructure() != o->structure() ||
            o->structure()->enumerationCache()->cachedPrototypeChain() != o->structure()->prototypeChain(exec));

    PropertyNameArray propertyNames(exec);
    o->getPropertyNames(exec, propertyNames);
    size_t numCacheableSlots = 0;
    if (!o->structure()->hasNonEnumerableProperties() && !o->structure()->hasAnonymousSlots() &&
        !o->structure()->hasGetterSetterProperties() && !o->structure()->isUncacheableDictionary() &&
        !o->structure()->typeInfo().overridesGetPropertyNames())
        numCacheableSlots = o->structure()->propertyStorageSize();

    TiPropertyNameIterator* jsPropertyNameIterator = new (exec) TiPropertyNameIterator(exec, propertyNames.data(), numCacheableSlots);

    if (o->structure()->isDictionary())
        return jsPropertyNameIterator;

    if (o->structure()->typeInfo().overridesGetPropertyNames())
        return jsPropertyNameIterator;
    
    size_t count = normalizePrototypeChain(exec, o);
    StructureChain* structureChain = o->structure()->prototypeChain(exec);
    WriteBarrier<Structure>* structure = structureChain->head();
    for (size_t i = 0; i < count; ++i) {
        if (structure[i]->typeInfo().overridesGetPropertyNames())
            return jsPropertyNameIterator;
    }

    jsPropertyNameIterator->setCachedPrototypeChain(exec->globalData(), structureChain);
    jsPropertyNameIterator->setCachedStructure(exec->globalData(), o->structure());
    o->structure()->setEnumerationCache(exec->globalData(), jsPropertyNameIterator);
    return jsPropertyNameIterator;
}

TiValue TiPropertyNameIterator::get(TiExcState* exec, TiObject* base, size_t i)
{
    TiValue identifier = m_jsStrings[i].get();
    if (m_cachedStructure.get() == base->structure() && m_cachedPrototypeChain.get() == base->structure()->prototypeChain(exec))
        return identifier;

    if (!base->hasProperty(exec, Identifier(exec, asString(identifier)->value(exec))))
        return TiValue();
    return identifier;
}

void TiPropertyNameIterator::visitChildren(SlotVisitor& visitor)
{
    ASSERT_GC_OBJECT_INHERITS(this, &s_info);
    ASSERT(structure()->typeInfo().overridesVisitChildren());
    visitor.appendValues(m_jsStrings.get(), m_jsStringsSize, MayContainNullValues);
    if (m_cachedPrototypeChain)
        visitor.append(&m_cachedPrototypeChain);
}

} // namespace TI

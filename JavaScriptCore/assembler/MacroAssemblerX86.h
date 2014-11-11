/**
 * Appcelerator Titanium License
 * This source code and all modifications done by Appcelerator
 * are licensed under the Apache Public License (version 2) and
 * are Copyright (c) 2009-2014 by Appcelerator, Inc.
 */

/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#ifndef MacroAssemblerX86_h
#define MacroAssemblerX86_h

#if ENABLE(ASSEMBLER) && CPU(X86)

#include "MacroAssemblerX86Common.h"

#if USE(MASM_PROBE)
#include <wtf/StdLibExtras.h>
#endif

namespace TI {

class MacroAssemblerX86 : public MacroAssemblerX86Common {
public:
    static const Scale ScalePtr = TimesFour;

    using MacroAssemblerX86Common::add32;
    using MacroAssemblerX86Common::and32;
    using MacroAssemblerX86Common::branchAdd32;
    using MacroAssemblerX86Common::branchSub32;
    using MacroAssemblerX86Common::sub32;
    using MacroAssemblerX86Common::or32;
    using MacroAssemblerX86Common::load32;
    using MacroAssemblerX86Common::load8;
    using MacroAssemblerX86Common::store32;
    using MacroAssemblerX86Common::store8;
    using MacroAssemblerX86Common::branch32;
    using MacroAssemblerX86Common::call;
    using MacroAssemblerX86Common::jump;
    using MacroAssemblerX86Common::addDouble;
    using MacroAssemblerX86Common::loadDouble;
    using MacroAssemblerX86Common::storeDouble;
    using MacroAssemblerX86Common::convertInt32ToDouble;
    using MacroAssemblerX86Common::branch8;
    using MacroAssemblerX86Common::branchTest8;

    void add32(TrustedImm32 imm, RegisterID src, RegisterID dest)
    {
        m_assembler.leal_mr(imm.m_value, src, dest);
    }

    void add32(TrustedImm32 imm, AbsoluteAddress address)
    {
        m_assembler.addl_im(imm.m_value, address.m_ptr);
    }
    
    void add32(AbsoluteAddress address, RegisterID dest)
    {
        m_assembler.addl_mr(address.m_ptr, dest);
    }
    
    void add64(TrustedImm32 imm, AbsoluteAddress address)
    {
        m_assembler.addl_im(imm.m_value, address.m_ptr);
        m_assembler.adcl_im(imm.m_value >> 31, reinterpret_cast<const char*>(address.m_ptr) + sizeof(int32_t));
    }

    void and32(TrustedImm32 imm, AbsoluteAddress address)
    {
        m_assembler.andl_im(imm.m_value, address.m_ptr);
    }
    
    void or32(TrustedImm32 imm, AbsoluteAddress address)
    {
        m_assembler.orl_im(imm.m_value, address.m_ptr);
    }
    
    void or32(RegisterID reg, AbsoluteAddress address)
    {
        m_assembler.orl_rm(reg, address.m_ptr);
    }
    
    void sub32(TrustedImm32 imm, AbsoluteAddress address)
    {
        m_assembler.subl_im(imm.m_value, address.m_ptr);
    }

    void load32(const void* address, RegisterID dest)
    {
        m_assembler.movl_mr(address, dest);
    }
    
    void load8(const void* address, RegisterID dest)
    {
        m_assembler.movzbl_mr(address, dest);
    }

    ConvertibleLoadLabel convertibleLoadPtr(Address address, RegisterID dest)
    {
        ConvertibleLoadLabel result = ConvertibleLoadLabel(this);
        m_assembler.movl_mr(address.offset, address.base, dest);
        return result;
    }

    void addDouble(AbsoluteAddress address, FPRegisterID dest)
    {
        m_assembler.addsd_mr(address.m_ptr, dest);
    }

    void storeDouble(FPRegisterID src, const void* address)
    {
        ASSERT(isSSE2Present());
        ASSERT(address);
        m_assembler.movsd_rm(src, address);
    }

    void convertInt32ToDouble(AbsoluteAddress src, FPRegisterID dest)
    {
        m_assembler.cvtsi2sd_mr(src.m_ptr, dest);
    }

    void store32(TrustedImm32 imm, void* address)
    {
        m_assembler.movl_i32m(imm.m_value, address);
    }

    void store32(RegisterID src, void* address)
    {
        m_assembler.movl_rm(src, address);
    }
    
    void store8(RegisterID src, void* address)
    {
        m_assembler.movb_rm(src, address);
    }

    void store8(TrustedImm32 imm, void* address)
    {
        ASSERT(-128 <= imm.m_value && imm.m_value < 128);
        m_assembler.movb_i8m(imm.m_value, address);
    }
    
    // Possibly clobbers src.
    void moveDoubleToInts(FPRegisterID src, RegisterID dest1, RegisterID dest2)
    {
        movePackedToInt32(src, dest1);
        rshiftPacked(TrustedImm32(32), src);
        movePackedToInt32(src, dest2);
    }

    void moveIntsToDouble(RegisterID src1, RegisterID src2, FPRegisterID dest, FPRegisterID scratch)
    {
        moveInt32ToPacked(src1, dest);
        moveInt32ToPacked(src2, scratch);
        lshiftPacked(TrustedImm32(32), scratch);
        orPacked(scratch, dest);
    }

    Jump branchAdd32(ResultCondition cond, TrustedImm32 imm, AbsoluteAddress dest)
    {
        m_assembler.addl_im(imm.m_value, dest.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branchSub32(ResultCondition cond, TrustedImm32 imm, AbsoluteAddress dest)
    {
        m_assembler.subl_im(imm.m_value, dest.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branch32(RelationalCondition cond, AbsoluteAddress left, RegisterID right)
    {
        m_assembler.cmpl_rm(right, left.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branch32(RelationalCondition cond, AbsoluteAddress left, TrustedImm32 right)
    {
        m_assembler.cmpl_im(right.m_value, left.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Call call()
    {
        return Call(m_assembler.call(), Call::Linkable);
    }

    // Address is a memory location containing the address to jump to
    void jump(AbsoluteAddress address)
    {
        m_assembler.jmp_m(address.m_ptr);
    }

    Call tailRecursiveCall()
    {
        return Call::fromTailJump(jump());
    }

    Call makeTailRecursiveCall(Jump oldJump)
    {
        return Call::fromTailJump(oldJump);
    }


    DataLabelPtr moveWithPatch(TrustedImmPtr initialValue, RegisterID dest)
    {
        padBeforePatch();
        m_assembler.movl_i32r(initialValue.asIntptr(), dest);
        return DataLabelPtr(this);
    }
    
    Jump branch8(RelationalCondition cond, AbsoluteAddress left, TrustedImm32 right)
    {
        m_assembler.cmpb_im(right.m_value, left.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branchTest8(ResultCondition cond, AbsoluteAddress address, TrustedImm32 mask = TrustedImm32(-1))
    {
        ASSERT(mask.m_value >= -128 && mask.m_value <= 255);
        if (mask.m_value == -1)
            m_assembler.cmpb_im(0, address.m_ptr);
        else
            m_assembler.testb_im(mask.m_value, address.m_ptr);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branchPtrWithPatch(RelationalCondition cond, RegisterID left, DataLabelPtr& dataLabel, TrustedImmPtr initialRightValue = TrustedImmPtr(0))
    {
        padBeforePatch();
        m_assembler.cmpl_ir_force32(initialRightValue.asIntptr(), left);
        dataLabel = DataLabelPtr(this);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    Jump branchPtrWithPatch(RelationalCondition cond, Address left, DataLabelPtr& dataLabel, TrustedImmPtr initialRightValue = TrustedImmPtr(0))
    {
        padBeforePatch();
        m_assembler.cmpl_im_force32(initialRightValue.asIntptr(), left.offset, left.base);
        dataLabel = DataLabelPtr(this);
        return Jump(m_assembler.jCC(x86Condition(cond)));
    }

    DataLabelPtr storePtrWithPatch(TrustedImmPtr initialValue, ImplicitAddress address)
    {
        padBeforePatch();
        m_assembler.movl_i32m(initialValue.asIntptr(), address.offset, address.base);
        return DataLabelPtr(this);
    }

    static bool supportsFloatingPoint() { return isSSE2Present(); }
    // See comment on MacroAssemblerARMv7::supportsFloatingPointTruncate()
    static bool supportsFloatingPointTruncate() { return isSSE2Present(); }
    static bool supportsFloatingPointSqrt() { return isSSE2Present(); }
    static bool supportsFloatingPointAbs() { return isSSE2Present(); }
    
    static FunctionPtr readCallTarget(CodeLocationCall call)
    {
        intptr_t offset = reinterpret_cast<int32_t*>(call.dataLocation())[-1];
        return FunctionPtr(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(call.dataLocation()) + offset));
    }

    static bool canJumpReplacePatchableBranchPtrWithPatch() { return true; }
    
    static CodeLocationLabel startOfBranchPtrWithPatchOnRegister(CodeLocationDataLabelPtr label)
    {
        const int opcodeBytes = 1;
        const int modRMBytes = 1;
        const int immediateBytes = 4;
        const int totalBytes = opcodeBytes + modRMBytes + immediateBytes;
        ASSERT(totalBytes >= maxJumpReplacementSize());
        return label.labelAtOffset(-totalBytes);
    }
    
    static CodeLocationLabel startOfPatchableBranchPtrWithPatchOnAddress(CodeLocationDataLabelPtr label)
    {
        const int opcodeBytes = 1;
        const int modRMBytes = 1;
        const int offsetBytes = 0;
        const int immediateBytes = 4;
        const int totalBytes = opcodeBytes + modRMBytes + offsetBytes + immediateBytes;
        ASSERT(totalBytes >= maxJumpReplacementSize());
        return label.labelAtOffset(-totalBytes);
    }
    
    static void revertJumpReplacementToBranchPtrWithPatch(CodeLocationLabel instructionStart, RegisterID reg, void* initialValue)
    {
        X86Assembler::revertJumpTo_cmpl_ir_force32(instructionStart.executableAddress(), reinterpret_cast<intptr_t>(initialValue), reg);
    }

    static void revertJumpReplacementToPatchableBranchPtrWithPatch(CodeLocationLabel instructionStart, Address address, void* initialValue)
    {
        ASSERT(!address.offset);
        X86Assembler::revertJumpTo_cmpl_im_force32(instructionStart.executableAddress(), reinterpret_cast<intptr_t>(initialValue), 0, address.base);
    }

#if USE(MASM_PROBE)
    // For details about probe(), see comment in MacroAssemblerX86_64.h.
    void probe(ProbeFunction, void* arg1 = 0, void* arg2 = 0);
#endif // USE(MASM_PROBE)

private:
    friend class LinkBuffer;
    friend class RepatchBuffer;

    static void linkCall(void* code, Call call, FunctionPtr function)
    {
        X86Assembler::linkCall(code, call.m_label, function.value());
    }

    static void repatchCall(CodeLocationCall call, CodeLocationLabel destination)
    {
        X86Assembler::relinkCall(call.dataLocation(), destination.executableAddress());
    }

    static void repatchCall(CodeLocationCall call, FunctionPtr destination)
    {
        X86Assembler::relinkCall(call.dataLocation(), destination.executableAddress());
    }

#if USE(MASM_PROBE)
    inline TrustedImm32 trustedImm32FromPtr(void* ptr)
    {
        return TrustedImm32(TrustedImmPtr(ptr));
    }

    inline TrustedImm32 trustedImm32FromPtr(ProbeFunction function)
    {
        return TrustedImm32(TrustedImmPtr(reinterpret_cast<void*>(function)));
    }

    inline TrustedImm32 trustedImm32FromPtr(void (*function)())
    {
        return TrustedImm32(TrustedImmPtr(reinterpret_cast<void*>(function)));
    }
#endif
};

#if USE(MASM_PROBE)

extern "C" void ctiMasmProbeTrampoline();

// For details on "What code is emitted for the probe?" and "What values are in
// the saved registers?", see comment for MacroAssemblerX86::probe() in
// MacroAssemblerX86_64.h.

inline void MacroAssemblerX86::probe(MacroAssemblerX86::ProbeFunction function, void* arg1, void* arg2)
{
    push(RegisterID::esp);
    push(RegisterID::eax);
    push(trustedImm32FromPtr(arg2));
    push(trustedImm32FromPtr(arg1));
    push(trustedImm32FromPtr(function));

    move(trustedImm32FromPtr(ctiMasmProbeTrampoline), RegisterID::eax);
    call(RegisterID::eax);
}
#endif // USE(MASM_PROBE)

} // namespace TI

#endif // ENABLE(ASSEMBLER)

#endif // MacroAssemblerX86_h
//===- ARMInstructionSplitting.h --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of ARMInstructionSplitting class for use
// by llvm-mctoll.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLVM_MCTOLL_ARM_ARMINSTRUCTIONSPLITTING_H
#define LLVM_TOOLS_LLVM_MCTOLL_ARM_ARMINSTRUCTIONSPLITTING_H

#include "ARMBaseInstrInfo.h"
#include "ARMRaiserBase.h"
#include "ARMSubtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

namespace llvm {
namespace mctoll {

/// Some instructions which their patterns include more than one operations,
/// like 'add r0, r1, r0, asr r1' or 'ldr r0, [r1, #4]', are splitted into
/// multiple MIs at here.
class ARMInstructionSplitting : public ARMRaiserBase {
public:
  static char ID;
  ARMInstructionSplitting(ARMModuleRaiser &MR);
  ~ARMInstructionSplitting() override;
  void init(MachineFunction *MF = nullptr, Function *RF = nullptr) override;
  bool split();
  bool runOnMachineFunction(MachineFunction &mf) override;

private:
  /// Check if the MI has shift pattern.
  unsigned checkisShifter(unsigned Opcode);
  /// Get the shift opcode in MI.
  unsigned getShiftOpcode(ARM_AM::ShiftOpc SOpc, unsigned OffSet);
  MachineInstr *splitLDRSTR(MachineBasicBlock &MBB, MachineInstr &MI);
  MachineInstr *splitLDRSTRPre(MachineBasicBlock &MBB, MachineInstr &MI);
  MachineInstr *splitLDRSTRPreImm(MachineBasicBlock &MBB, MachineInstr &MI);
  MachineInstr *splitLDRSTRImm(MachineBasicBlock &MBB, MachineInstr &MI);
  MachineInstr *splitCommon(MachineBasicBlock &MBB, MachineInstr &MI,
                            unsigned NewOpc);
  MachineInstr *splitS(MachineBasicBlock &MBB, MachineInstr &MI,
                       unsigned NewOpc, int Idx);
  MachineInstr *splitC(MachineBasicBlock &MBB, MachineInstr &MI,
                       unsigned NewOpc, int Idx);
  MachineInstr *splitCS(MachineBasicBlock &MBB, MachineInstr &MI,
                        unsigned NewOpc, int Idx);
  /// True if the ARM instruction performs Shift_C().
  bool isShift_C(unsigned Opcode); // NOLINT(readability-identifier-naming)
  /// No matter what pattern of Load/Store is, change the Opcode to xxxi12.
  unsigned getLoadStoreOpcode(unsigned Opcode);
  /// If the MI is load/store which needs wback, it will return true.
  bool isLDRSTRPre(unsigned Opcode);
  MachineInstrBuilder &addOperand(MachineInstrBuilder &MIB, MachineOperand &MO,
                                  bool IsDef = false);

  MachineRegisterInfo *MRI;
  const ARMBaseInstrInfo *TII;
  LLVMContext *CTX;
};

} // end namespace mctoll
} // end namespace llvm

#endif // LLVM_TOOLS_LLVM_MCTOLL_ARM_ARMINSTRUCTIONSPLITTING_H

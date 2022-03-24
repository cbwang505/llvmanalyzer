//===- llvm-build generated file --------------------------------*- C++ -*-===//
//
// Component Library Dependency Table
//
// Automatically generated file, do not edit!
//
//===----------------------------------------------------------------------===//

struct AvailableComponent {
  /// The name of the component.
  const char *Name;

  /// The name of the library for this component (or NULL).
  const char *Library;

  /// Whether the component is installed.
  bool IsInstalled;

  /// The list of libraries required when linking this component.
  const char *RequiredLibraries[12];
} AvailableComponents[33] = {
  { "aggressiveinstcombine", "LLVMAggressiveInstCombine", true, { "analysis", "core", "support", "transformutils" } },
  { "all", nullptr, true, { "engine", "passes", "nativecodegen", "tablegen", "all-targets", "symbolize" } },
  { "all-targets", nullptr, true, { "x86" } },
  { "analysis", "LLVMAnalysis", true, { "binaryformat", "core", "object", "support" } },
  { "asmparser", "LLVMAsmParser", true, { "binaryformat", "core", "support" } },
  { "binaryformat", "LLVMBinaryFormat", true, { "support" } },
  { "bitreader", "LLVMBitReader", true, { "core", "support" } },
  { "bitwriter", "LLVMBitWriter", true, { "analysis", "core", "mc", "object", "support" } },
  { "codegen", "LLVMCodeGen", true, { "analysis", "bitreader", "bitwriter", "core", "mc", "scalaropts", "support", "target", "transformutils" } },
  { "core", "LLVMCore", true, { "binaryformat", "support" } },
  { "debuginfocodeview", "LLVMDebugInfoCodeView", true, { "support", "debuginfomsf" } },
  { "debuginfodwarf", "LLVMDebugInfoDWARF", true, { "binaryformat", "object", "mc", "support" } },
  { "debuginfomsf", "LLVMDebugInfoMSF", true, { "support" } },
  { "debuginfopdb", "LLVMDebugInfoPDB", true, { "object", "support", "debuginfocodeview", "debuginfomsf" } },
  { "demangle", "LLVMDemangle", true, {  } },
  { "engine", nullptr, true, { "native" } },
  { "instcombine", "LLVMInstCombine", true, { "analysis", "core", "support", "transformutils" } },
  { "ipo", "LLVMipo", true, { "aggressiveinstcombine", "analysis", "bitreader", "bitwriter", "core", "instcombine", "irreader", "object", "scalaropts", "support", "transformutils" } },
  { "irreader", "LLVMIRReader", true, { "asmparser", "bitreader", "core", "support" } },
  { "mc", "LLVMMC", true, { "support", "binaryformat" } },
  { "mcparser", "LLVMMCParser", true, { "mc", "support" } },
  { "native", nullptr, true, { "x86" } },
  { "nativecodegen", nullptr, true, { "x86codegen" } },
  { "object", "LLVMObject", true, { "bitreader", "core", "mc", "binaryformat", "mcparser", "support" } },
  { "passes", "LLVMPasses", true, { "aggressiveinstcombine", "analysis", "codegen", "core", "ipo", "instcombine", "scalaropts", "support", "target", "transformutils" } },
  { "scalaropts", "LLVMScalarOpts", true, { "aggressiveinstcombine", "analysis", "core", "instcombine", "support", "transformutils" } },
  { "support", "LLVMSupport", true, { "demangle" } },
  { "symbolize", "LLVMSymbolize", true, { "debuginfodwarf", "debuginfopdb", "object", "support", "demangle" } },
  { "tablegen", "LLVMTableGen", true, { "support" } },
  { "target", "LLVMTarget", true, { "analysis", "core", "mc", "support" } },
  { "transformutils", "LLVMTransformUtils", true, { "analysis", "core", "support" } },
  { "x86", nullptr, true, { "x86codegen" } },
  { "x86codegen", "LLVMX86CodeGen", true, {  } },
};

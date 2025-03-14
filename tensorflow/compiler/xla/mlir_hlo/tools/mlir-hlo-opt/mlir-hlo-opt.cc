/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mlir-hlo/Dialect/gml_st/IR/gml_st_ops.h"
#include "mlir-hlo/Dialect/gml_st/transforms/passes.h"
#include "mlir-hlo/Dialect/gml_st/transforms/test_passes.h"
#include "mlir-hlo/Dialect/lhlo/IR/lhlo_ops.h"
#include "mlir-hlo/Dialect/lhlo/transforms/passes.h"
#include "mlir-hlo/Dialect/lhlo_gpu/IR/lhlo_gpu_ops.h"
#include "mlir-hlo/Dialect/mhlo/IR/register.h"
#include "mlir-hlo/Dialect/mhlo/transforms/passes.h"
#include "mlir-hlo/Dialect/thlo/IR/thlo_ops.h"
#include "mlir-hlo/Dialect/thlo/transforms/passes.h"
#include "mlir-hlo/Transforms/gpu_passes.h"
#include "mlir-hlo/Transforms/passes.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "stablehlo/dialect/Register.h"

using namespace mlir;

namespace {

}  // namespace

int main(int argc, char **argv) {
  mlir::registerAllPasses();
  mlir::hlo::registerLMHLOTransformsPasses();
  mlir::registerLMHLOGPUTransformsPasses();
  mlir::mhlo::registerAllMhloPasses();
  mlir::lmhlo::registerAllLmhloPasses();
  mlir::thlo::registerAllThloPasses();
  mlir::gml_st::registerGmlStPasses();
  mlir::gml_st::registerGmlStTestPasses();

  PassPipelineRegistration<HloToGpuPipelineOptions>(
      "hlo-to-gpu-pipeline",
      "Pipeline to transform HLO to LLVM + NVVM dialects.",
      createHloToGpuPipeline);

  mlir::DialectRegistry registry;
  mlir::registerAllDialects(registry);
  mlir::mhlo::registerAllMhloDialects(registry);
  mlir::stablehlo::registerAllDialects(registry);
  registry.insert<mlir::lmhlo::LmhloDialect, mlir::lmhlo_gpu::LmhloGpuDialect,
                  mlir::gml_st::GmlStDialect, mlir::thlo::THLODialect>();

  return failed(
      mlir::MlirOptMain(argc, argv, "MLIR HLO pass driver\n", registry));
}

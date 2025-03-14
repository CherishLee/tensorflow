load("//tensorflow/tsl/platform:rules_cc.bzl", "cc_library")
load("//tensorflow:tensorflow.default.bzl", "if_nccl")
load("@local_config_cuda//cuda:build_defs.bzl", "if_cuda")
load("@local_config_rocm//rocm:build_defs.bzl", "if_rocm")
load("//tensorflow:tensorflow.bzl", "tf_cc_test")

package(
    default_visibility = ["//tensorflow:internal"],
    licenses = ["notice"],
)

cc_library(
    name = "gpu_helpers",
    srcs = ["gpu_helpers.cc"],
    hdrs = ["gpu_helpers.h"],
    visibility = ["//tensorflow/compiler/xla/pjrt:friends"],
    deps = [
        "//tensorflow/compiler/xla:statusor",
        "//tensorflow/compiler/xla:types",
        "//tensorflow/compiler/xla:util",
        "//tensorflow/compiler/xla/client:client_library",
        "//tensorflow/compiler/xla/client:local_client",
        "//tensorflow/compiler/xla/service:platform_util",
        "//tensorflow/compiler/xla/stream_executor:device_mem_allocator",
        "//tensorflow/compiler/xla/stream_executor:kernel",
        "//tensorflow/core:gpu_runtime",
        "//tensorflow/tsl/util:env_var",
        "@com_google_absl//absl/types:span",
    ],
)

cc_library(
    name = "se_gpu_pjrt_client",
    srcs = ["se_gpu_pjrt_client.cc"],
    hdrs = ["se_gpu_pjrt_client.h"],
    defines = if_cuda(["GOOGLE_CUDA=1"]) + if_rocm(["TENSORFLOW_USE_ROCM=1"]),
    visibility = ["//tensorflow/compiler/xla/pjrt:friends"],
    deps = [
        ":gpu_helpers",
        "//tensorflow/compiler/xla/pjrt:pjrt_stream_executor_client",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "//tensorflow/compiler/xla/service/gpu:gpu_executable_run_options",
        "//tensorflow/compiler/xla:statusor",
        "//tensorflow/compiler/xla/client:client_library",
        "//tensorflow/compiler/xla/pjrt/distributed:client",
        "//tensorflow/compiler/xla/service:platform_util",
        "//tensorflow/compiler/xla:util",
        "//tensorflow/tsl/framework:bfc_allocator",
        "//tensorflow/tsl/framework:device_id",
        "//tensorflow/tsl/framework:device_id_impl",
        "//tensorflow/tsl/util:env_var",
        "//tensorflow/compiler/xla/stream_executor:device_mem_allocator",
        "//tensorflow/compiler/xla/stream_executor:device_memory",
        "//tensorflow/compiler/xla/stream_executor:tf_allocator_adapter",
    ] + if_cuda([
        ":nccl_id_store_cuda",
        "@local_config_cuda//cuda:cuda_headers",
        "//tensorflow/core/common_runtime/gpu:gpu_cudamallocasync_allocator",
        "//tensorflow/compiler/xla/stream_executor/cuda:cuda_activation_header",
    ]) + if_rocm([
        ":nccl_id_store_rocm",
        "@local_config_rocm//rocm:rocm_headers",
    ]),
)

# We actually wish we could write if_cuda(if_nccl(...)) in :gpu_device,
# but Bazel does not allow nested selects. We can work around the problem using
# an intermediate library that has the conditional NCCL pieces that is only
# itself included as a dependency if CUDA is enabled.
cc_library(
    name = "nccl_id_store_cuda",
    srcs = ["nccl_id_store.cc"],
    hdrs = ["nccl_id_store.h"],
    defines = if_nccl(["NCCL_ENABLED=1"]),
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/synchronization",
        "//tensorflow/compiler/xla/pjrt/distributed:client",
        "//tensorflow/compiler/xla:util",
        "//tensorflow/compiler/xla:statusor",
        "//tensorflow/compiler/xla/service:global_device_id",
        "//tensorflow/compiler/xla/service/gpu:gpu_executable_run_options",
    ] + if_nccl(["@local_config_nccl//:nccl"]),
)

cc_library(
    name = "nccl_id_store_rocm",
    srcs = ["nccl_id_store.cc"],
    hdrs = ["nccl_id_store.h"],
    defines = if_nccl(["NCCL_ENABLED=1"]),
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/synchronization",
        "//tensorflow/compiler/xla/pjrt/distributed:client",
        "//tensorflow/compiler/xla:util",
        "//tensorflow/compiler/xla:statusor",
        "//tensorflow/compiler/xla/service:global_device_id",
        "//tensorflow/compiler/xla/service/gpu:gpu_executable_run_options",
    ] + if_nccl(["@local_config_nccl//:nccl"]),
)

tf_cc_test(
    name = "pjrt_client_test_se_gpu",
    srcs = ["pjrt_client_test_se_gpu.cc"],
    tags = [
        "no_oss",
        "notap",
        "requires-gpu-nvidia:2",
    ],
    deps = [
        ":se_gpu_pjrt_client",
        "//tensorflow/compiler/xla/pjrt:pjrt_client_test_common",
        "//tensorflow/compiler/xla/service:gpu_plugin",
        "//tensorflow/tsl/platform:test_main",
    ],
)

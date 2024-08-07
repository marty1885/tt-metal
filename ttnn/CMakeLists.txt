set(TTNN_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/async_runtime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/op_library/to_layout/to_layout_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/conv2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/matmul.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/matmul_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/single_core/bmm_op_single_core_tilize_untilize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core/bmm_op_multi_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse/bmm_op_multi_core_reuse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse_padding/bmm_op_multi_core_reuse_padding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse_mcast_1d_optimized/bmm_op_multi_core_reuse_mcast_1d_optimized.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse_mcast_2d_optimized/bmm_op_multi_core_reuse_mcast_2d_optimized.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse_mcast_dram_sharded_optimized/bmm_op_multi_core_reuse_dram_sharded_optimized.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/matmul/device/multi_core_reuse_optimized/bmm_op_multi_core_reuse_optimized.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/reduction/argmax/device/argmax_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary/device/binary_device_operation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary/device/broadcast_height_and_width_multi_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary/device/broadcast_height_multi_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary/device/broadcast_width_multi_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary/device/element_wise_multi_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/binary_backward/device/binary_backward_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/ternary_backward/device/ternary_backward_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/complex_binary_backward/device/complex_binary_backward_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/upsample/upsample_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/upsample/device/upsample_op_multi_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/upsample/device/upsample_op_single_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/unary/device/unary_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/unary_backward/device/unary_backward_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/complex_unary/device/complex_unary_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/unary/device/unary_composite_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/eltwise/complex_unary_backward/device/complex_unary_backward_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/examples/example/device/example_device_operation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/examples/example/device/single_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/examples/example/device/multi_core_program_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/reduction/topk/device/topk_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/embedding/embedding/device/embeddings_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/data_movement/slice/device/slice_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/softmax/device/softmax_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/softmax/device/multi_core/softmax_op_multi_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/data_movement/pad/device/pad_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/data_movement/tilize/device/tilize_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/data_movement/tilize_with_val_padding/device/tilize_with_val_padding_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/layernorm/device/layernorm_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/layernorm/device/multi_core/layernorm_op_multi_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/data_movement/downsample/device/downsample_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/groupnorm/device/groupnorm_op.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/operations/normalization/groupnorm/device/multi_core/groupnorm_op_multi_core.cpp
)

### Setup TTNN_LIB as an Object library
# Save variables to reuse between ttnn_lib and ttnn
# Have to manually propagate because ttnn_lib is an OBJECT library
set(ttnn_compile_options -MP -Wno-int-to-pointer-cast -fno-var-tracking)
set(ttnn_link_libraries compiler_flags metal_header_directories metal_common_libs tt_metal)
set(ttnn_lib_include_dirs
    ${UMD_HOME}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/tt_metal
    ${CMAKE_CURRENT_SOURCE_DIR} # ${PROJECT_SOURCE_DIR}/ttnn
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/experimental # symlink to tt_eager; should become native folder once merge complete
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/experimental/tensor
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/experimental/queue
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
    ${PROJECT_SOURCE_DIR}/tt_metal/third_party/fmt
)

add_library(ttnn_lib OBJECT ${TTNN_SRCS})
target_compile_options(ttnn_lib PUBLIC ${ttnn_compile_options})
target_link_libraries(ttnn_lib PUBLIC ${ttnn_link_libraries})
target_include_directories(ttnn_lib PUBLIC ${ttnn_lib_include_dirs})
target_precompile_headers(ttnn_lib PRIVATE
    ${PROJECT_SOURCE_DIR}/tt_metal/third_party/magic_enum/magic_enum.hpp
    <functional>
    <map>
    <memory>
    <unordered_map>
    <variant>
    <vector>
)

### Setup TTNN as a shared library with optional Python bindings
set(TTNN_FINAL_SRC $<TARGET_OBJECTS:ttnn_lib> ${QUEUE_SRCS} ${TENSOR_SRCS} ${TT_DNN_SRCS})
set(TTNN_PUBLIC_INCLUDE_DIRS ${ttnn_lib_include_dirs})
set(TTNN_PUBLIC_LINK_LIBRARIES ${ttnn_link_libraries} linker_flags) # linker_flags = -rdynamic if tracy enabled
set(TTNN_PUBLIC_LINK_DIRS "")
if(WITH_PYTHON_BINDINGS)
    # TODO: should be using pybind11_add_module, but right now it introduces many build problems
    # pybinds will always be built as a shared library
    list(APPEND TTNN_FINAL_SRC
        ${PROJECT_SOURCE_DIR}/ttnn/cpp/pybind11/__init__.cpp
        ${TT_LIB_SRCS}) # TT_LIB_SRCS from tt_eager/tt_lib/CMakeLists.txt for python bindigns

    list(APPEND TTNN_PUBLIC_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ttnn/experimental/tt_lib
        ${PROJECT_SOURCE_DIR}/tt_metal/third_party/pybind11/include
        ${Python3_INCLUDE_DIRS})

    list(APPEND TTNN_PUBLIC_LINK_DIRS ${Python3_LIBRARY_DIRS})

    list(APPEND TTNN_PUBLIC_LINK_LIBRARIES pch_pybinds ${Python3_LIBRARIES})
endif()

add_library(ttnn SHARED ${TTNN_FINAL_SRC})
target_compile_options(ttnn PUBLIC ${ttnn_compile_options})
target_include_directories(ttnn PUBLIC ${TTNN_PUBLIC_INCLUDE_DIRS})
target_link_libraries(ttnn PUBLIC ${TTNN_PUBLIC_LINK_LIBRARIES})
target_link_directories(ttnn PUBLIC ${TTNN_PUBLIC_LINK_DIRS})

# Make sure library built is _ttnn.so and that it can find all it's linked libraries
# ttnn breaks if -fvisibility=hidden, so CXX_VISIBILITY_PRESET set to default
set_target_properties(ttnn PROPERTIES
    OUTPUT_NAME "_ttnn"
    PREFIX ""
    SUFFIX ".so"
    BUILD_RPATH "${PROJECT_BINARY_DIR}/tt_metal;${PROJECT_BINARY_DIR}/tt_eager;${PROJECT_BINARY_DIR}/ttnn"
    INSTALL_RPATH "${PROJECT_BINARY_DIR}/lib"
    CXX_VISIBILITY_PRESET "default"
    ADDITIONAL_CLEAN_FILES "${PROJECT_SOURCE_DIR}/ttnn/ttnn/_ttnn.so;${PROJECT_SOURCE_DIR}/ttnn/ttnn.egg-info"
)

// SPDX-FileCopyrightText: © 2023 Tenstorrent Inc.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <concepts>
#include <optional>
#include <tt_eager/tensor/tensor.hpp>

#include "third_party/magic_enum/magic_enum.hpp"
#include "tt_dnn/op_library/operation_history.hpp"
#include "tt_metal/third_party/tracy/public/tracy/Tracy.hpp"
#include "tt_metal/tools/profiler/op_profiler.hpp"
#include "tt_stl/concepts.hpp"
#include "tt_stl/reflection.hpp"
#include "tt_stl/unique_any.hpp"
#include <reflect>

namespace ttnn {

namespace device_operation {

template <typename program_attributes_t>
struct CachedProgram {
    tt::tt_metal::Program program;
    // Cached program needs to share program_attributes between create and override_runtime_arguments functions
    program_attributes_t program_attributes;

    CachedProgram(tt::tt_metal::Program&& program, program_attributes_t&& program_attributes) :
        program{std::move(program)}, program_attributes{program_attributes} {}
};

struct CachedProgramFactory {
    static constexpr auto MAX_SIZE = 896;
    static constexpr auto ALIGNMENT = 32;

    tt::stl::unique_any<MAX_SIZE, ALIGNMENT> cached_program;
    // program_factory_index is used to map a runtime value to a program factory type that is being used
    std::size_t program_factory_index;

    template <typename program_attributes_t>
    CachedProgramFactory(CachedProgram<program_attributes_t>&& cached_program, std::size_t program_factory_index) :
        cached_program{std::move(cached_program)}, program_factory_index{program_factory_index} {}
};

template <typename program_factory_t>
concept ProgramFactoryConcept = requires {
    [](const auto& operation_attributes, const auto& tensor_args, auto& tensor_return_value) {
        auto cached_program = program_factory_t::create(operation_attributes, tensor_args, tensor_return_value);
        program_factory_t::override_runtime_arguments(
            cached_program, operation_attributes, tensor_args, tensor_return_value);
    };
};

template <typename device_operation_t>
concept DeviceOperationConcept = requires {
    [](const typename device_operation_t::operation_attributes_t& operation_attributes,
       const typename device_operation_t::tensor_args_t& tensor_args) {
        device_operation_t::validate_on_program_cache_hit(operation_attributes, tensor_args);
        device_operation_t::validate_on_program_cache_miss(operation_attributes, tensor_args);

        using shape_return_value_t = typename device_operation_t::shape_return_value_t;
        static_assert(std::same_as<
                      decltype(device_operation_t::compute_output_shapes(operation_attributes, tensor_args)),
                      shape_return_value_t>);

        using tensor_return_value_t = typename device_operation_t::tensor_return_value_t;
        static_assert(std::same_as<
                      decltype(device_operation_t::create_output_tensors(operation_attributes, tensor_args)),
                      tensor_return_value_t>);

        const auto program_factory = device_operation_t::select_program_factory(operation_attributes, tensor_args);
        std::visit(
            [](auto&& program_factory) {
                using program_factory_t = std::decay_t<decltype(program_factory)>;
                static_assert(ProgramFactoryConcept<program_factory_t>);
            },
            program_factory);
    };
};

template <typename device_operation_t>
concept DeviceOperationWithCustomProgramCacheConcept = DeviceOperationConcept<device_operation_t> and requires {
    [](auto&& program_factory,
       const typename device_operation_t::operation_attributes_t& operation_attributes,
       const typename device_operation_t::tensor_args_t& tensor_args) {
        device_operation_t::compute_program_hash(operation_attributes, tensor_args);
    };
};

template <typename... Ts>
[[nodiscard]] std::variant<Ts...> constexpr map_index_to_variant(std::size_t i, std::variant<Ts...>) {
    assert(i < sizeof...(Ts));
    constexpr std::variant<Ts...> table[] = { Ts{ }... };
    return table[i];
}

template <typename T>
    requires std::same_as<std::decay_t<T>, Tensor>
constexpr auto get_first_tensor(T&& value) {
    return std::cref(value);
}

template <typename T>
constexpr auto get_first_tensor(const std::optional<T>& value) {
    if (value.has_value()) {
        const auto& tensor = value.value();
        return get_first_tensor(tensor);
    }
}

template <typename T>
constexpr auto get_first_tensor(const std::vector<T>& value) {
    for (auto& tensor : value) {
        return get_first_tensor(tensor);
    }
}

template <typename T, auto N>
constexpr auto get_first_tensor(const std::array<T, N>& value) {
    for (auto& tensor : value) {
        return get_first_tensor(tensor);
    }
}

template <typename... Ts>
constexpr auto get_first_tensor(const std::tuple<Ts...>& value) {
    constexpr auto num_attributes = sizeof...(Ts);
    return get_first_tensor(std::get<0>(value));
}

template <typename T>
    requires requires { std::decay_t<T>::attribute_names; } and (not std::same_as<std::decay_t<T>, Tensor>)
constexpr auto get_first_tensor(T&& object) {
    constexpr auto num_attributes = std::tuple_size_v<decltype(std::decay_t<T>::attribute_names)>;
    return get_first_tensor(object.attribute_values());
}

inline const auto USE_FAST_DISPATCH = std::getenv("TT_METAL_SLOW_DISPATCH_MODE") == nullptr;

template <typename device_operation_t>
inline auto compute_program_hash(
    const typename device_operation_t::operation_attributes_t& operation_attributes,
    const typename device_operation_t::tensor_args_t& tensor_args) {
    if constexpr (DeviceOperationWithCustomProgramCacheConcept<device_operation_t>) {
        ZoneScopedN("Compute custom program hash");
        return device_operation_t::compute_program_hash(operation_attributes, tensor_args);
    } else {
        ZoneScopedN("Compute default program hash");
        return tt::stl::hash::hash_objects_with_default_seed(
            tt::stl::hash::type_hash<device_operation_t>, operation_attributes, tensor_args);
    }
}

template <typename device_operation_t>
inline auto& create_or_get_program_from_cache(
    auto& program_cache,
    auto cache_hit,
    auto program_hash,
    const typename device_operation_t::operation_attributes_t& operation_attributes,
    const typename device_operation_t::tensor_args_t& tensor_args,
    typename device_operation_t::tensor_return_value_t& tensor_return_value) {
    if (not cache_hit) {
        ZoneScopedN("Program Cache Miss");
        auto program_factory = device_operation_t::select_program_factory(operation_attributes, tensor_args);

        auto& program = std::visit(
            [&program_cache,
             &program_hash,
             &operation_attributes,
             &tensor_args,
             &tensor_return_value,
             program_factory_index = program_factory.index()](auto&& program_factory) -> auto& {
                using program_factory_t = std::decay_t<decltype(program_factory)>;
                using cached_program_t =
                    decltype(program_factory_t::create(operation_attributes, tensor_args, tensor_return_value));
                program_cache.insert(
                    program_hash,
                    CachedProgramFactory{
                        program_factory_t::create(operation_attributes, tensor_args, tensor_return_value),
                        program_factory_index});
                auto& cached_program_factory = program_cache.template get<CachedProgramFactory>(program_hash);
                auto& cached_program = cached_program_factory.cached_program.template get<cached_program_t>();
                return cached_program.program;
            },
            program_factory);
        return program;
    } else {
        ZoneScopedN("Program Cache Hit");
        auto& cached_program_factory = program_cache.template get<CachedProgramFactory>(program_hash);
        auto program_factory_index = cached_program_factory.program_factory_index;

        using program_factory_variant_t =
            decltype(device_operation_t::select_program_factory(operation_attributes, tensor_args));
        auto program_factory = map_index_to_variant(program_factory_index, program_factory_variant_t{});

        auto& program = std::visit(
            [&cached_program_factory, &operation_attributes, &tensor_args, &tensor_return_value](
                auto&& program_factory) -> auto& {
                using program_factory_t = std::decay_t<decltype(program_factory)>;

                using cached_program_t =
                    decltype(program_factory_t::create(operation_attributes, tensor_args, tensor_return_value));
                auto& cached_program = cached_program_factory.cached_program.template get<cached_program_t>();

                program_factory_t::override_runtime_arguments(
                    cached_program, operation_attributes, tensor_args, tensor_return_value);

                return cached_program.program;
            },
            program_factory);
        return program;
    }
}

constexpr auto check_tensor_types = [](auto&& tensor) {
    static_assert(std::same_as<std::decay_t<decltype(tensor)>, Tensor>);
};

template <DeviceOperationConcept device_operation_t>
typename device_operation_t::tensor_return_value_t run(
    uint8_t cq_id,
    const typename device_operation_t::operation_attributes_t& operation_attributes,
    const typename device_operation_t::tensor_args_t& tensor_args) {
    ZoneScopedN("TT_DNN_DEVICE_OP");
    auto operation_id = assign_operation_id();

    tt::stl::reflection::visit_object_of_type<Tensor>(check_tensor_types, tensor_args);

    using tensor_return_value_t = typename device_operation_t::tensor_return_value_t;
    static_assert(not std::same_as<tensor_return_value_t, void>, "Operation cannot return type cannot be void");

    auto device = get_first_tensor(tensor_args).get().device();
    auto& program_cache = device->program_cache;

    auto program_hash = compute_program_hash<device_operation_t>(operation_attributes, tensor_args);
    auto cache_hit = program_cache.contains(program_hash);

    if (cache_hit) {
        ZoneScopedN("Validate on Program Cache Hit");
        device_operation_t::validate_on_program_cache_hit(operation_attributes, tensor_args);
    } else {
        ZoneScopedN("Validate on Program Cache Miss");
        device_operation_t::validate_on_program_cache_miss(operation_attributes, tensor_args);
    }
    auto tensor_return_value = [&operation_attributes, &tensor_args]() {
        ZoneScopedN("Create Output Tensors");
        return device_operation_t::create_output_tensors(operation_attributes, tensor_args);
    }();

    tt::stl::reflection::visit_object_of_type<Tensor>(check_tensor_types, tensor_return_value);

    auto& program = create_or_get_program_from_cache<device_operation_t>(
        program_cache, cache_hit, program_hash, operation_attributes, tensor_args, tensor_return_value);

    if (USE_FAST_DISPATCH) {
        ZoneScopedN("EnqueueProgram");
        auto& queue = device->command_queue(cq_id);
        // Program will temporarily own the input buffers. This is required, since with Async command
        // queues, the input tensor can preemptively be deallocted on device, unless program maintains
        // explicit ownership. This invocation of the program wicll give up ownership once its enqueued.
        auto assign_global_buffer_to_program = [&program](auto&& tensor) {
            AssignGlobalBufferToProgram(tensor.device_buffer(), program);
        };
        tt::stl::reflection::visit_object_of_type<Tensor>(assign_global_buffer_to_program, tensor_args);
        tt::tt_metal::EnqueueProgram(queue, program, false);
    } else {
        ZoneScopedN("LaunchProgram");
        ::detail::LaunchProgram(device, program);
    }

    // TODO: update this to work properly take program cache info, as well as tensors
    TracyOpTNNNDeviceV2(
        device_operation_t{},
        operation_id,
        device->id(),
        program,
        program_hash,
        operation_attributes,
        tensor_args,
        tensor_return_value);

    return tensor_return_value;
}

}  // namespace device_operation

}  // namespace ttnn

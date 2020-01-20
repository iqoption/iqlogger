//
// TypeTraits.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <array>
#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace iqlogger::utils::types {

template<typename Default, typename... Pairs>
auto make_switch(Default&& def, Pairs... ps) {
  using InputType = std::common_type_t<typename Pairs::first_type...>;
  using ReturnType = std::common_type_t<std::decay_t<decltype(def())>, decltype(ps.second())...>;

  if constexpr (std::is_void_v<ReturnType>) {
    return [=](InputType x) {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif
      ((x == ps.first && (void(ps.second()), 1)) || ...) || (void(def()), 1);
    };
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  } else {
    return [=](InputType x) {
      std::optional<ReturnType> ret = {};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif
      ((x == ps.first && (void(ret.emplace(std::move(ps.second()))), 1)) || ...) ||
          (void(ret.emplace(std::move(def()))), 1);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
      return std::move(ret.value());
    };
  }
}

}  // namespace iqlogger::utils::types

#ifndef RFL_REMOVEFIELDS_HPP_
#define RFL_REMOVEFIELDS_HPP_

#include <algorithm>
#include <tuple>
#include <type_traits>

#include "rfl/internal/StringLiteral.hpp"
#include "rfl/internal/remove_fields.hpp"

namespace rfl {

/// Recursively removes all of the fields signified by _names from the
/// NamedTupleType.
template <class NamedTupleType, internal::StringLiteral... _names>
using remove_fields_t =
    typename internal::remove_fields<std::decay_t<NamedTupleType>,
                                     _names...>::type;

}  // namespace rfl

#endif  // RFL_REMOVEFIELDS_HPP_

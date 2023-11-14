#ifndef RFL_INTERNAL_TOPTRFIELD_HPP_
#define RFL_INTERNAL_TOPTRFIELD_HPP_

#include <type_traits>

#include "rfl/Field.hpp"
#include "rfl/Flatten.hpp"
#include "rfl/always_false.hpp"
#include "rfl/internal/StringLiteral.hpp"

namespace rfl {
namespace internal {

template <internal::StringLiteral _name, class Type>
inline auto to_ptr_field(Field<_name, Type>& _field) {
  if constexpr (std::is_pointer_v<Type>) {
    static_assert(always_false_v<Type>,
                  "Writing from raw pointers is dangerous and "
                  "therefore unsupported. "
                  "Please consider using std::unique_ptr, rfl::Box, "
                  "std::shared_ptr, rfl::Ref or std::optional instead.");
  }
  using T = std::remove_reference_t<Type>;
  return Field<_name, T*>(&_field.value_);
}

template <internal::StringLiteral _name, class Type>
inline auto to_ptr_field(const Field<_name, Type>& _field) {
  if constexpr (std::is_pointer_v<Type>) {
    static_assert(always_false_v<Type>,
                  "Writing from raw pointers is dangerous and "
                  "therefore unsupported. "
                  "Please consider using std::unique_ptr, rfl::Box, "
                  "std::shared_ptr, rfl::Ref or std::optional instead.");
  }
  using T = std::remove_cvref_t<Type>;
  return Field<_name, const T*>(&_field.value_);
}

template <class Type>
inline auto to_ptr_field(Flatten<Type>& _field) {
  if constexpr (std::is_pointer_v<Type>) {
    static_assert(always_false_v<Type>,
                  "Writing from raw pointers is dangerous and "
                  "therefore unsupported. "
                  "Please consider using std::unique_ptr, rfl::Box, "
                  "std::shared_ptr, rfl::Ref or std::optional instead.");
  }
  using T = std::remove_reference_t<Type>;
  return Flatten<T*>(&_field.value_);
}

template <class Type>
inline auto to_ptr_field(const Flatten<Type>& _field) {
  using T = std::remove_cvref_t<Type>;
  return Flatten<const T*>(&_field.value_);
}

}  // namespace internal
}  // namespace rfl

#endif

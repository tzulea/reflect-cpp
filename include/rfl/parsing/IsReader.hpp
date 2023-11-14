#ifndef RFL_PARSING_ISREADER_HPP_
#define RFL_PARSING_ISREADER_HPP_

#include <array>
#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "rfl/Result.hpp"

namespace rfl {
namespace parsing {

template <class R, class T>
concept IsReader = requires(R r, std::string name,
                            std::function<std::int16_t(std::string_view)> fct,
                            typename R::InputArrayType arr,
                            typename R::InputObjectType obj,
                            typename R::InputVarType var) {
  /// Any Reader needs to define the following:
  ///
  /// 1) An InputArrayType, which must be an array-like data structure.
  /// 2) An InputObjectType, which must contain key-value pairs.
  /// 3) An InputVarType, which must be able to represent either
  ///    InputArrayType, InputObjectType or a basic type (bool, integral,
  ///    floating point, std::string).
  /// 4) A static constexpr bool has_custom_constructor, that determines
  ///    whether the class in question as a custom constructor, which might
  ///    be called something like from_json_obj(...).

  /// Retrieves a particular field from an object.
  {
    r.get_field(name, obj)
    } -> std::same_as<rfl::Result<typename R::InputVarType>>;

  /// Determines whether a variable is empty (the NULL type).
  { r.is_empty(var) } -> std::same_as<bool>;

  /// Transforms var to a basic type (bool, integral,
  /// floating point, std::string)
  { r.template to_basic_type<T>(var) } -> std::same_as<rfl::Result<T>>;

  /// fct is a function that turns the field name into the field index of the
  /// struct. It returns -1, if the fields does not exist on the struct. This
  /// returns an std::array that can be used to build up the struct.
  {
    r.template to_fields_array<6>(fct, obj)
    } -> std::same_as<std::array<std::optional<typename R::InputVarType>, 6>>;

  /// Casts var as an InputArrayType.
  { r.to_array(var) } -> std::same_as<rfl::Result<typename R::InputArrayType>>;

  /// Iterates through an object and writes the contained key-value pairs into
  /// a vector.
  {
    r.to_map(obj)
    } -> std::same_as<
        std::vector<std::pair<std::string, typename R::InputVarType>>>;

  /// Casts var as an InputObjectType.
  {
    r.to_object(var)
    } -> std::same_as<rfl::Result<typename R::InputObjectType>>;

  /// Iterates through an array and writes the contained vars into
  /// a vector.
  { r.to_vec(arr) } -> std::same_as<std::vector<typename R::InputVarType>>;

  /// Uses the custom constructor, if it has been determined that T has one
  /// (see above).
  { r.template use_custom_constructor<T>(var) } -> std::same_as<rfl::Result<T>>;
};

}  // namespace parsing
}  // namespace rfl

#endif

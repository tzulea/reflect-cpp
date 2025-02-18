#ifndef RFL_LITERAL_HPP_
#define RFL_LITERAL_HPP_

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>

#include "rfl/Result.hpp"
#include "rfl/internal/StringLiteral.hpp"
#include "rfl/internal/VisitTree.hpp"

namespace rfl {

template <internal::StringLiteral _field>
struct LiteralHelper {
    constexpr static internal::StringLiteral field_ = _field;
};

template <internal::StringLiteral... fields_>
class Literal {
    using FieldsType = std::tuple<LiteralHelper<fields_>...>;

   public:
    using ValueType =
        std::conditional_t<sizeof...(fields_) <=
                               std::numeric_limits<std::uint8_t>::max(),
                           std::uint8_t, std::uint16_t>;

    /// The number of different fields or different options that the literal
    /// can assume.
    static constexpr ValueType num_fields_ = sizeof...(fields_);

    using ReflectionType = std::string;

    /// Constructs a Literal from another literal.
    Literal(const Literal<fields_...>& _other) = default;

    /// Constructs a Literal from another literal.
    Literal(Literal<fields_...>&& _other) noexcept = default;

    Literal(const std::string& _str) : value_(find_value(_str).value()) {}

    /// A single-field literal is special because it
    /// can also have a default constructor.
    template <ValueType num_fields = num_fields_,
              typename = std::enable_if_t<num_fields == 1>>
    Literal() : value_(0) {}

    ~Literal() = default;

    /// Determines whether the literal contains the string.
    static bool contains(const std::string& _str) { return has_value(_str); }

    /// Determines whether the literal contains the string at compile time.
    template <internal::StringLiteral _name>
    static constexpr bool contains() {
        return find_value_of<_name>() != -1;
    }

    /// Determines whether the literal contains any of the strings in the other
    /// literal at compile time.
    template <class OtherLiteralType, int _i = 0>
    static constexpr bool contains_any() {
        if constexpr (_i == num_fields_) {
            return false;
        } else {
            constexpr auto name = find_name_within_own_fields<_i>();
            return OtherLiteralType::template contains<name>() ||
                   contains_any<OtherLiteralType, _i + 1>();
        }
    }

    /// Determines whether the literal contains all of the strings in the other
    /// literal at compile time.
    template <class OtherLiteralType, int _i = 0, int _n_found = 0>
    static constexpr bool contains_all() {
        if constexpr (_i == num_fields_) {
            return _n_found == OtherLiteralType::num_fields_;
        } else {
            constexpr auto name = find_name_within_own_fields<_i>();
            if constexpr (OtherLiteralType::template contains<name>()) {
                return contains_all<OtherLiteralType, _i + 1, _n_found + 1>();
            } else {
                return contains_all<OtherLiteralType, _i + 1, _n_found>();
            }
        }
    }

    /// Determines whether the literal has duplicate strings at compile time.
    /// These is useful for checking collections of strings in other contexts.
    static constexpr bool has_duplicates() { return has_duplicate_strings(); }

    /// Constructs a Literal from a string. Returns an error if the string
    /// cannot be found.
    static Result<Literal> from_string(const std::string& _str) {
        const auto to_literal = [](const auto& _v) {
            return Literal<fields_...>(_v);
        };
        return find_value(_str).transform(to_literal);
    };

    /// Constructs a new Literal.
    template <internal::StringLiteral _name>
    static Literal<fields_...> make() {
        return Literal(Literal<fields_...>::template value_of<_name>());
    }

    /// The name defined by the Literal.
    std::string name() const { return find_name(); }

    /// Helper function to retrieve a name at compile time.
    template <int _value>
    constexpr static auto name_of() {
        constexpr auto name = find_name_within_own_fields<_value>();
        return Literal<name>();
    }

    /// Assigns from another literal.
    Literal<fields_...>& operator=(const Literal<fields_...>& _other) = default;

    /// Assigns from another literal.
    Literal<fields_...>& operator=(Literal<fields_...>&& _other) noexcept =
        default;

    /// Assigns the literal from a string
    Literal<fields_...>& operator=(const std::string& _str) {
        value_ = find_value(_str);
        return *this;
    }

    /// Equality operator other Literals.
    bool operator==(const Literal<fields_...>& _other) const {
        return value() == _other.value();
    }

    /// Equality operator other Literals with different fields.
    template <internal::StringLiteral... other_fields>
    bool operator==(const Literal<other_fields...>& _other) const {
        return name() == _other.name();
    }

    /// Inequality operator for other Literals.
    template <internal::StringLiteral... other_fields>
    bool operator!=(const Literal<other_fields...>& _other) const {
        return !(*this == _other);
    }

    /// Equality operator for strings.
    bool operator==(const std::string& _str) const { return name() == _str; }

    /// Inequality operator for strings.
    bool operator!=(const std::string& _str) const { return name() != _str; }

    /// Alias for .name().
    std::string reflection() const { return name(); }

    /// Alias for .name().
    std::string str() const { return name(); }

    /// Returns the value actually contained in the Literal.
    ValueType value() const { return value_; }

    /// Returns the value of the string literal in the template.
    template <internal::StringLiteral _name>
    static constexpr ValueType value_of() {
        constexpr auto value = find_value_of<_name>();
        static_assert(value >= 0, "String not supported.");
        return value;
    }

   private:
    /// Only make is allowed to use this constructor.
    Literal(const ValueType _value) : value_(_value) {}

    /// Returns all of the allowed fields.
    template <int _i = 0>
    static std::string allowed_strings(const std::string& _values = "") {
        using FieldType = typename std::tuple_element<_i, FieldsType>::type;
        const auto head = "'" + FieldType::field_.str() + "'";
        const auto values = _values.size() == 0 ? head : _values + ", " + head;
        if constexpr (_i + 1 < num_fields_) {
            return allowed_strings<_i + 1>(values);
        } else {
            return values;
        }
    }

    /// Whether the Literal contains duplicate strings.
    template <int _i = 1>
    constexpr static bool has_duplicate_strings() {
        if constexpr (_i >= num_fields_) {
            return false;
        } else {
            return is_duplicate<_i>() || has_duplicate_strings<_i + 1>();
        }
    }

    template <int _i, int _j = _i - 1>
    constexpr static bool is_duplicate() {
        using FieldType1 = typename std::tuple_element<_i, FieldsType>::type;
        using FieldType2 = typename std::tuple_element<_j, FieldsType>::type;
        if constexpr (FieldType1::field_ == FieldType2::field_) {
            return true;
        } else if constexpr (_j > 0) {
            return is_duplicate<_i, _j - 1>();
        } else {
            return false;
        }
    }

    /// Finds the correct index associated with
    /// the string at run time.
    template <int _i = 0>
    std::string find_name() const {
        if (_i == value_) {
            using FieldType = typename std::tuple_element<_i, FieldsType>::type;
            return FieldType::field_.str();
        }
        if constexpr (_i + 1 == num_fields_) {
            return "";
        } else {
            return find_name<_i + 1>();
        }
    }

    /// Finds the correct index associated with
    /// the string at compile time within the Literal's own fields.
    template <int _i>
    constexpr static auto find_name_within_own_fields() {
        using FieldType = typename std::tuple_element<_i, FieldsType>::type;
        return FieldType::field_;
    }

    /// Finds the correct value associated with
    /// the string at run time.
    template <int _i = 0>
    static Result<int> find_value(const std::string& _str) {
        using FieldType = typename std::tuple_element<_i, FieldsType>::type;
        if (FieldType::field_.str() == _str) {
            return _i;
        }
        if constexpr (_i + 1 == num_fields_) {
            return Error("Literal does not support string '" + _str +
                         "'. The following strings are supported: " +
                         allowed_strings() + ".");
        } else {
            return find_value<_i + 1>(_str);
        }
    }

    /// Finds the value of a string literal at compile time.
    template <internal::StringLiteral _name, int _i = 0>
    static constexpr int find_value_of() {
        using FieldType = typename std::tuple_element<_i, FieldsType>::type;
        if constexpr (FieldType::field_ == _name) {
            return _i;
        } else if constexpr (_i + 1 < num_fields_) {
            return find_value_of<_name, _i + 1>();
        } else {
            return -1;
        }
    }

    /// Whether the literal contains this string.
    template <int _i = 0>
    static bool has_value(const std::string& _str) {
        using FieldType = typename std::tuple_element<_i, FieldsType>::type;
        if (FieldType::field_.str() == _str) {
            return true;
        }
        if constexpr (_i + 1 == num_fields_) {
            return false;
        } else {
            return has_value<_i + 1>(_str);
        }
    }

    static_assert(sizeof...(fields_) > 0,
                  "There must be at least one field in a Literal.");

    static_assert(sizeof...(fields_) <= std::numeric_limits<ValueType>::max(),
                  "Too many fields.");

    static_assert(!has_duplicates(),
                  "Duplicate strings are not allowed in a Literal.");

   private:
    /// The underlying value.
    ValueType value_;
};

/// Helper function to retrieve a name at compile time.
template <class LiteralType, int _value>
inline constexpr auto name_of() {
    return LiteralType::template name_of<_value>();
}

/// Helper function to retrieve a value at compile time.
template <class LiteralType, internal::StringLiteral _name>
inline constexpr auto value_of() {
    return LiteralType::template value_of<_name>();
}

}  // namespace rfl

#endif  // RFL_LITERAL_HPP_

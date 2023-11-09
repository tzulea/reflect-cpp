# Composing validators

Sometimes the conditions you want to impose are more complex than a simple minimum or a simple regex pattern. That is why reflect-cpp allows you to compose them using the operators `rfl::AnyOf`, `rfl::AllOf` and `rfl::OneOf`.

- `rfl::AnyOf` requires that *at least one* of the conditions contained therein be true. It thus works like an OR operator.
- `rfl::AllOf` requires that *all* of the conditions contained therein be true. It thus works like an AND operator.
- `rfl::OneOf` requires that *exactly one* of the conditions contained therein be true. It thus works like a XOR operator.

For instance, if you want to ensure that a person's age is a reasonable number, you can impose two conditions as follows:

```cpp
using Age = rfl::Validator<int,
                           rfl::AllOf<rfl::Minimum<0>, rfl::Maximum<130>>>;

struct Person {
  rfl::Field<"firstName", std::string> first_name;
  rfl::Field<"lastName", std::string> last_name;
  rfl::Field<"age", Age> age;
};
```

You can also nest composed validators. For instance, in this case the age must either be 0-10 or 40-130.

```cpp
using Age = rfl::Validator<
    int,
    rfl::AnyOf<rfl::AllOf<rfl::Minimum<0>, rfl::Maximum<10>>,
               rfl::AllOf<rfl::Minimum<40>, rfl::Maximum<130>>>>;
```

You can also compose regex patterns:

```cpp
using TableOrColumnName = rfl::Validator<
    std::string,
    rfl::AnyOf<rfl::PatternValidator<R"(^[A-Z]+(?:_[A-Z]+)*$)", "TableName">,
               rfl::PatternValidator<R"(^[a-z]+(?:_[a-z]+)*$)", "ColumnName">>>;
```

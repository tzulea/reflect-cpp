# `rfl::as`

`rfl::as` allows you to cast structs as other structs. This is useful if the fields in `B` are a subset of the fields in `A`
or we need the fields in `A` and `B` to produce `C`.

For example:

```cpp
struct A {
    rfl::Field<"f1", std::string> f1;
    rfl::Field<"f2", std::string> f2;
};

struct B {
    rfl::Field<"f3", std::string> f3;
    rfl::Field<"f4", std::string> f4;
};

struct C {
    rfl::Field<"f1", std::string> f1;
    rfl::Field<"f2", std::string> f2;
    rfl::Field<"f4", std::string> f4;
};

const auto a = A{.f1 = "Hello", .f2 = "World"};

const auto b = B{.f3 = "Hello", .f4 = "World"};

const auto c = rfl::as<C>(a, b);
```

In this case, `f1`, `f2` and `f4` are now copied into the newly created `c` of type `C`.

Much like `rfl::replace`, `rfl::as` supports move semantics:

```cpp
auto a = A{.f1 = "Hello", .f2 = "World"};

auto b = B{.f3 = "Hello", .f4 = "World"};

const auto c = rfl::as<C>(std::move(a), std::move(b));
```

In this case, `f1`, `f2` and `f4` are now moved into the newly created `c` of type `C`. This is a zero-copy operation. `f3` is discarded.

`rfl::as` also supports `rfl::Flatten`. For instance:

```cpp
struct A {
    rfl::Field<"f1", std::string> f1;
    rfl::Field<"f2", std::string> f2;
};

struct B {
    rfl::Field<"f3", std::string> f3;
    rfl::Field<"f4", std::string> f4;
};

struct C {
    rfl::Flatten<A> a;
    rfl::Flatten<B> b;
    rfl::Field<"f5", int> f5;
};

const auto a = A{.f1 = "Hello", .f2 = "World"};

const auto b = B{.f3 = "Hello", .f4 = "World"};

const auto c =
    rfl::as<C>(a, b, rfl::make_field<"f5">(5));
```

In this particular instance, `rfl::as<C>(...)` is basically equivalent to this:

```cpp
const auto c = C{.a = a, .b = b, .f5 = rfl::make_field<"f5">(5)};
```

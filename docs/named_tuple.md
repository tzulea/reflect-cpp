# `rfl::NamedTuple`

`rfl::NamedTuple` is very similar to `std::tuple`, but unlike `std::tuple` the fields have names.

In other words, consider the following struct:

```cpp
struct Person {
    rfl::Field<"firstName", std::string> first_name;
    rfl::Field<"lastName", std::string> last_name;
    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">> birthday;
};
```

You might as well define the following `rfl::NamedTuple`:

```cpp
using Person = rfl::NamedTuple<
    rfl::Field<"firstName", std::string>,
    rfl::Field<"lastName", std::string>,
    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">>>;
```

From the point-of-view of serialization/deserialization, the two definitions are equivalent. 
The resulting JSON strings (or any other format) will be the same.


## Structural typing

From the point-of-view of programming, there is an important difference: Structs are *nominally typed*
and named tuples are *structurally typed* (confusingly, structs are not structurally typed).

In plain language, that means that the compiler will regard this as absolutely equivalent to `Person`, the named tuple:

```cpp
using Person2 = rfl::NamedTuple<
    rfl::Field<"firstName", std::string>,
    rfl::Field<"lastName", std::string>,
    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">>>;
```

However, this will be seen as a type that is different from `Person`, the struct:

```cpp
struct Person2 {
    rfl::Field<"firstName", std::string> first_name;
    rfl::Field<"lastName", std::string> last_name;
    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">> birthday;
};
```

Structural typing also means that you can declare new types on-the-fly. For instance, in order
to create a `Person` named tuple, you don't actually have to declare it at all. The following will do:

```cpp
const auto person = rfl::Field<"firstName", std::string>("Homer") *
                    rfl::Field<"lastName", std::string>("Simpson") *
                    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">>("1987-04-19");
```

The type of `person` will now be equivalent to the definition of `Person`, the named tuple, 
regardless of whether you have actually declared it anywhere.

On the other hand, structural typing also means that recursive definitions are impossible.
For instance, consider something like this:

```cpp
struct Person {
    rfl::Field<"firstName", std::string> first_name;
    rfl::Field<"lastName", std::string> last_name;
    rfl::Field<"children", std::vector<Person>> children;
};
```

In this example, `Person` is recursively defined (because of the field `children`). 
This is impossible to accomplish using structural typing and `rfl::NamedTuple`, just like it is impossible to have a recursively defined lambda function.

## Accessing fields

Fields inside the named tuple can be accessed using `rfl::get` or the `.get` method:

```cpp
const auto person = rfl::Field<"firstName", std::string>("Homer") *
                    rfl::Field<"lastName", std::string>("Simpson") *
                    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">>("1987-04-19");

// OK in most circumstances (there are restrictions
// due to the way C++ templates work).
const auto first_name = person.get<"firstName">();

// Always OK
const auto first_name = person.template get<"firstName">();

// Always OK
const auto first_name = rfl::get<"firstName">(person);
```

## Similarity to structs

Most of the functionality we have described also work for `rfl::NamedTuple`. 
It should be noted that you are free to mix `rfl::NamedTuple` and structs.
For instance, you can build a `rfl::TaggedUnion` from a combination of 
`rfl::NamedTuple` and structs or you could construct a struct from named tuples using
`rfl::as`.

### `std::variant` and `rfl::TaggedUnion`

`rfl::NamedTuple`s can be inserted into `std::variant`:

```cpp
using Circle = rfl::NamedTuple<rfl::Field<"radius", double>>;

using Rectangle = rfl::NamedTuple<
    rfl::Field<"height", double>,
    rfl::Field<"width", double>>;

using Square rfl::NamedTuple<rfl::Field<"width", double>>;

// Untagged variant - OK
using Shapes = std::variant<Circle, Rectangle, Square>;

// Externally tagged variant - also OK:
using Shapes = std::variant<rfl::Field<"circle", Circle>,
                            rfl::Field<"rectangle", Rectangle>,
                            rfl::Field<"square", Square>>;
```

They can also be inserted into `rfl::TaggedUnion`:

```cpp
using Circle = rfl::NamedTuple<
    rfl::Field<"shape", rfl::Literal<"Circle">>,
    rfl::Field<"radius", double>>;

using Rectangle = rfl::NamedTuple<
    rfl::Field<"shape", rfl::Literal<"Rectangle">>,
    rfl::Field<"height", double>,
    rfl::Field<"width", double>>;

using Square rfl::NamedTuple<
    rfl::Field<"shape", rfl::Literal<"Square">>,
    rfl::Field<"width", double>>;

// Tell rfl::TaggedUnion that you want it to look for the field "shape".
using Shapes = rfl::TaggedUnion<"shape", Circle, Square, Rectangle>;
```

### `rfl::replace`

`rfl::replace` works for `rfl::NamedTuple` as well:

```cpp
const auto lisa = rfl::Field<"firstName", std::string>("Lisa") *
                  rfl::Field<"lastName", std::string>("Simpson");

// Returns a deep copy of the original object,
// replacing first_name.
const auto maggie =
    rfl::replace(lisa, rfl::make_field<"firstName">(std::string("Maggie")));

// Also OK
const auto bart = lisa.replace(rfl::make_field<"firstName">(std::string("Bart")));
```

### `rfl::as`

So does `rfl::as`:

```cpp
using C = rfl::NamedTuple<
    rfl::Field<"f1", std::string>,
    rfl::Field<"f2", std::string>,
    rfl::Field<"f4", std::string>>;

const auto a = rfl::Field<"f1", std::string>("Hello") *  
               rfl::Field<"f2", std::string>("World");

const auto b = rfl::Field<"f3", std::string>("Hello") *  
               rfl::Field<"f4", std::string>("World");

const auto c = rfl::as<C>(a, b);
```

However, you do not really have to use `rfl::as` here. This will work as well:

```cpp
// Same as rfl::as<C>(a, b)
const auto c = C(a * b);
```

(in fact, this is how `rfl::as` is implemented in the first place).

## Defining named tuples using other named tuples

`rfl::Flatten` is not supported inside named tuples. Instead, you can use `rfl::define_named_tuple_t<...>`
to achieve the same goal:

```cpp
using Person = rfl::NamedTuple<
    rfl::Field<"firstName", std::string>,
    rfl::Field<"lastName", std::string>,
    rfl::Field<"age", int>>;

using Employee = rfl::define_named_tuple_t<
    Person,
    rfl::Field<"salary", float>>;

const auto employee = Employee(
    rfl::Field<"firstName", std::string>("Homer"),
    rfl::Field<"lastName", std::string>("Simpson"),
    rfl::make_field<"age">(45),
    rfl::make_field<"salary">(60000.0));
```

## Transforming structs to named tuples and vice versa

You can transform structs to named tuples and vice versa:

```cpp
auto bart = Person{.first_name = "Bart",
                   .last_name = "Simpson",
                   .birthday = "1987-04-19"};

// bart_nt is a named tuple
const auto bart_nt = rfl::to_named_tuple(bart);

// You can also retrieve the equivalent named tuple
// type to a struct:
using PersonNamedTuple = rfl::named_tuple_t<Person>;

// rfl::to_named_tuple also supports move semantics
PersonNamedTuple bart_nt = rfl::to_named_tuple(std::move(bart_nt));

// You can also go the other way
const auto bart_struct = rfl::from_named_tuple<Person>(bart_nt);
const auto bart_struct = rfl::from_named_tuple<Person>(std::move(bart_nt));
```

## rfl::NamedTuple is central to reflect-cpp

`rfl::NamedTuple` is basically at the heart of how the reflect-cpp reflection
is actually implemented. If you look at how reflect-cpp is implemented you will find that 
structs are transformed from and to `rfl::NamedTuple`s
all the time (using zero-copy operations wherever possible). 
That is because `rfl::NamedTuple` actually support reflection,
where as structs don't.





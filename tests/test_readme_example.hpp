#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>
#include <vector>

#include "write_and_read.hpp"

// Makes sure that the example in the README works as expected.
// It would be embarrassing if it didn't...
void test_readme_example() {
  std::cout << "test_readme_example" << std::endl;

  // Age must be a plausible number, between 0 and 130. This will
  // be validated automatically.
  using Age = rfl::Validator<unsigned int,
                             rfl::AllOf<rfl::Minimum<0>, rfl::Maximum<130>>>;

  // "firstName", "lastName" and "children" are the field names
  // as they will appear in the JSON. The C++ standard is
  // snake case, the JSON standard is camel case, so the names
  // will not always be identical.
  struct Person {
    rfl::Field<"firstName", std::string> first_name;
    rfl::Field<"lastName", std::string> last_name;
    rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">> birthday;
    rfl::Field<"age", Age> age;
    rfl::Field<"email", rfl::Email> email;
    rfl::Field<"children", std::vector<Person>> children;
  };

  const auto bart = Person{.first_name = "Bart",
                           .last_name = "Simpson",
                           .birthday = "1987-04-19",
                           .age = 10,
                           .email = "bart@simpson.com",
                           .children = std::vector<Person>()};

  const auto lisa = Person{
      .first_name = "Lisa",
      .last_name = "Simpson",
      .birthday = "1987-04-19",
      .age = 8,
      .email = "lisa@simpson.com",
      .children = rfl::default_value  // same as std::vector<Person>()
  };

  // Returns a deep copy of the original object,
  // replacing first_name.
  const auto maggie =
      rfl::replace(lisa, rfl::make_field<"firstName">(std::string("Maggie")),
                   rfl::make_field<"email">(std::string("maggie@simpson.com")),
                   rfl::make_field<"age">(0));

  const auto homer =
      Person{.first_name = "Homer",
             .last_name = "Simpson",
             .birthday = "1987-04-19",
             .age = 45,
             .email = "homer@simpson.com",
             .children = std::vector<Person>({bart, lisa, maggie})};

  write_and_read(
      homer,
      R"({"firstName":"Homer","lastName":"Simpson","birthday":"1987-04-19","age":45,"email":"homer@simpson.com","children":[{"firstName":"Bart","lastName":"Simpson","birthday":"1987-04-19","age":10,"email":"bart@simpson.com","children":[]},{"firstName":"Lisa","lastName":"Simpson","birthday":"1987-04-19","age":8,"email":"lisa@simpson.com","children":[]},{"firstName":"Maggie","lastName":"Simpson","birthday":"1987-04-19","age":0,"email":"maggie@simpson.com","children":[]}]})");
}

#include <cassert>
#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>
#include <vector>

#include "write_and_read.hpp"

void test_replace() {
    std::cout << "test_replace" << std::endl;

    struct Person {
        rfl::Field<"firstName", std::string> first_name;
        rfl::Field<"lastName", std::string> last_name;
        rfl::Field<"children", rfl::Box<std::vector<Person>>> children;
    };

    auto lisa = Person{.first_name = "Lisa",
                       .last_name = "Simpson",
                       .children = rfl::make_box<std::vector<Person>>()};

    const auto maggie = rfl::replace(
        std::move(lisa), rfl::make_field<"firstName">(std::string("Maggie")));

    write_and_read(
        maggie, R"({"firstName":"Maggie","lastName":"Simpson","children":[]})");
}

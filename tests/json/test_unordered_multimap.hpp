#include <iostream>
#include <map>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>

#include "write_and_read.hpp"

void test_unordered_multimap() {
    std::cout << "test_unordered_multimap" << std::endl;

    struct Person {
        rfl::Field<"firstName", std::string> first_name;
        rfl::Field<"lastName", std::string> last_name = "Simpson";
        rfl::Field<"children",
                   std::unique_ptr<std::unordered_multimap<int, Person>>>
            children = rfl::default_value;
    };

    auto children = std::make_unique<std::unordered_multimap<int, Person>>();
    children->insert(std::make_pair(1, Person{.first_name = "Bart"}));
    children->insert(std::make_pair(2, Person{.first_name = "Lisa"}));
    children->insert(std::make_pair(3, Person{.first_name = "Maggie"}));

    const auto homer =
        Person{.first_name = "Homer", .children = std::move(children)};

    // Unordered maps are unpredictable. We therefore only make sure that this
    // compiles.
    const auto json_string = rfl::json::write(homer);
    const auto homer2 = rfl::json::read<Person>(json_string);
    std::cout << "OK" << std::endl << std::endl;
}

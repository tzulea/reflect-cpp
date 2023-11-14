#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>
#include <vector>

#include "write_and_read.hpp"

void test_error_messages() {
    std::cout << "test_error_messages" << std::endl;

    struct Person {
        rfl::Field<"firstName", std::string> first_name;
        rfl::Field<"lastName", std::string> last_name;
        rfl::Field<"birthday", rfl::Timestamp<"%Y-%m-%d">> birthday;
        rfl::Field<"children", std::vector<Person>> children;
    };

    const std::string faulty_string =
        R"({"firstName":"Homer","lastName":12345,"birthday":"04/19/1987"})";

    const auto result = rfl::json::read<Person>(faulty_string);

    const std::string expected = R"(Found 3 errors:
1) Failed to parse field 'lastName': Could not cast to string.
2) Failed to parse field 'birthday': String '04/19/1987' did not match format '%Y-%m-%d'.
3) Field named 'children' not found.)";

    if (!result.error()) {
        std::cout << "Expected an error, got none." << std::endl << std::endl;
    }

    if (result.error().value().what() != expected) {
        std::cout << "Expected: " << std::endl
                  << expected << std::endl
                  << "Got:" << std::endl
                  << result.error().value().what() << std::endl
                  << std::endl;
    }

    std::cout << "OK" << std::endl << std::endl;
}

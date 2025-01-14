#include <cassert>
#include <iostream>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>
#include <vector>

#include "write_and_read.hpp"

void test_replace_flatten() {
    std::cout << "test_replace_flatten" << std::endl;

    struct Person {
        rfl::Field<"firstName", std::string> first_name;
        rfl::Field<"lastName", rfl::Box<std::string>> last_name;
        rfl::Field<"age", int> age;
    };

    struct Employee {
        rfl::Flatten<Person> person;
        rfl::Field<"employer", rfl::Box<std::string>> employer;
        rfl::Field<"salary", float> salary;
    };

    auto employee = Employee{
        .person = Person{.first_name = "Homer",
                         .last_name = rfl::make_box<std::string>("Simpson"),
                         .age = 45},
        .employer = rfl::make_box<std::string>("Mr. Burns"),
        .salary = 60000.0};

    auto employee2 =
        rfl::replace(std::move(employee), rfl::make_field<"salary">(70000.0),
                     rfl::make_field<"age">(46));

    write_and_read(
        employee2,
        R"({"firstName":"Homer","lastName":"Simpson","age":46,"employer":"Mr. Burns","salary":70000.0})");
}

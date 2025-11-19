#include <iostream>
#include <string>
#include "include/forward_list.h"
#include "include/forward_list_memory_resource.h"

struct Employee {
    std::string name;
    int id;
    double salary;
    std::string department;

    Employee(const std::string& n, int i, double s, const std::string& d)
        : name(n), id(i), salary(s), department(d) {}

    friend std::ostream& operator<<(std::ostream& os, const Employee& emp) {
        os << "Employee{name: \"" << emp.name 
           << "\", id: " << emp.id 
           << ", salary: " << emp.salary 
           << ", department: \"" << emp.department << "\"}";
        return os;
    }
};

int main() {
    std::cout << "=== ДЕМОНСТРАЦИЯ FORWARD_LIST ===" << std::endl;
    
    // Демонстрация с int - используем node_size() вместо прямого доступа к Node
    {
        forward_list_memory_resource mr(forward_list<int>::node_size(), 10);
        forward_list<int> numbers(&mr);

        for (int i = 1; i <= 5; ++i) {
            numbers.push_front(i * 10);
        }

        std::cout << "Integer list: ";
        for (const auto& num : numbers) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    // Демонстрация со структурой
    {
        forward_list_memory_resource mr(forward_list<Employee>::node_size(), 5);
        forward_list<Employee> employees(&mr);

        employees.push_front(Employee("Alice", 1001, 50000.0, "Engineering"));
        employees.push_front(Employee("Bob", 1002, 45000.0, "Marketing"));

        std::cout << "Employee list:" << std::endl;
        for (const auto& emp : employees) {
            std::cout << "  " << emp << std::endl;
        }
    }

    return 0;
}
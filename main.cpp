#include <iostream>
#include <chrono>
#include <optional>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "poly.h"

std::optional<double> poly_test(polynomial& p1,
                                polynomial& p2,
                                std::vector<std::pair<power, coeff>> solution)

{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    polynomial p3 = p1 * p2;

    auto p3_can_form = p3.canonical_form();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (p3_can_form != solution)
    {
        return std::nullopt;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

void given_test() {
    /** We're doing (x+1)^2, so solution is x^2 + 2x + 1*/
    std::vector<std::pair<power, coeff>> solution = {{2,1}, {1,2}, {0,1}};

    /** This holds (x+1), which we'll pass to each polynomial */
    std::vector<std::pair<power, coeff>> poly_input = {{1,1}, {0,1}};


    polynomial p1(poly_input.begin(), poly_input.end());
    polynomial p2(poly_input.begin(), poly_input.end());

    std::optional<double> result = poly_test(p1, p2, solution);

    if (result.has_value())
    {
        std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
    } 
    else 
    {
        std::cout << "Failed test" << std::endl;
    }

    std::vector<std::pair<power, coeff>> dividend_terms = {{2, 1}, {1, 2}, {0, 1}};
    std::vector<std::pair<power, coeff>> divisor_terms = {{1, 1}, {0, 1}};
    std::vector<std::pair<power, coeff>> expected_remainder = {{0, 0}};

    polynomial dividend(dividend_terms.begin(), dividend_terms.end());
    polynomial divisor(divisor_terms.begin(), divisor_terms.end());

    polynomial remainder = dividend % divisor;

    auto result_modulo = remainder.canonical_form();

    remainder.print();

    if (result_modulo == expected_remainder) {
        std::cout << "Modulo test passed." << std::endl;
    } else {
        std::cout << "Modulo test failed." << std::endl;
    }
}

// power stringToPower(const std::string& str) {
//     try {
//         power result = std::stoull(str); // Use std::stoull for unsigned long long
//         // Optional: Add a check to ensure the result fits within power's range
//         if (result > std::numeric_limits<power>::max()) {
//             throw std::out_of_range("Value exceeds the range of size_t");
//         }
//         return result;
//     } catch (const std::invalid_argument& e) {
//         std::cerr << "Invalid argument: " << e.what() << std::endl;
//         throw; // Re-throw the exception if you want the caller to handle it
//     } catch (const std::out_of_range& e) {
//         std::cerr << "Out of range: " << e.what() << std::endl;
//         throw; // Re-throw the exception
//     }
// }

void read_txt(std::string input_file, std::string expected_file) {
    std::ifstream f(input_file);

    // String variable to store the read data
    std::vector<std::string> lines;
    std::string s;

    // make polynomials from given txt
    std::vector<polynomial> poly;
    std::vector<std::pair<power, coeff>> input;
    while (getline(f, s)) {
        // lines.push_back(s);
        if (s == ";") {
            polynomial p(input.begin(), input.end());
            poly.push_back(p);
            input.clear();
            continue;
        }

        size_t split_idx = s.find("x^");
        power p = std::stoull(s.substr(split_idx+2));
        coeff c = std::stoi(s.substr(0, split_idx));
        // std::cout << "Power: " << p << " | Coeff: " << c << std::endl;
        input.push_back(std::make_pair(p, c));
    }
    f.close();

    // read expected result
    std::vector<std::pair<power, coeff>> expected;
    std::ifstream f2(expected_file);
    while (getline(f2, s)) {
        // lines.push_back(s);
        if (s == ";") {
            break;
        }

        size_t split_idx = s.find("x^");
        power p = std::stoull(s.substr(split_idx+2));
        coeff c = std::stoi(s.substr(0, split_idx));
        // std::cout << "Power: " << p << " | Coeff: " << c << std::endl;
        expected.push_back(std::make_pair(p, c));
    }
    f.close();

    polynomial expected_poly(expected.begin(), expected.end());

    // get product
    std::cout << "Calculating products..." << std::endl;

    polynomial p1 = poly[0];
    polynomial p2 = poly[1];

    std::optional<double> result = poly_test(p1, p2, expected_poly.canonical_form());

    if (result.has_value())
    {
        std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
    } 
    else 
    {
        std::cout << "Failed test" << std::endl;
    }

}

int main()
{
    // given_test();
    read_txt("simple_poly.txt", "result.txt");

}
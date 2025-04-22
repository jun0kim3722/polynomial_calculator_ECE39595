#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <optional>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include "poly.h"

std::vector<std::pair<power, coeff>> parse_polynomial(std::ifstream& file) {
    std::vector<std::pair<power, coeff>> result;
    std::string line;
    while (std::getline(file, line)) {
        if (line == ";") break;
        std::istringstream iss(line);
        int coeff;
        char x, caret;
        int power;
        if (iss >> coeff >> x >> caret >> power) {
            result.emplace_back(power, coeff);
        }
    }
    return result;
}

bool compare_files(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1);
    std::ifstream f2(file2);
    
    std::string line1, line2;
    std::vector<std::string> contents1, contents2;
    
    while (std::getline(f1, line1)) {
        if (!line1.empty()) {
            contents1.push_back(line1);
        }
    }
    
    while (std::getline(f2, line2)) {
        if (!line2.empty()) {
            contents2.push_back(line2);
        }
    }
    
    if (contents1.size() != contents2.size()) {
        std::cout << "Files have different number of lines: " 
                  << contents1.size() << " vs " << contents2.size() << std::endl;
        return false;
    }
    
    std::vector<std::pair<power, coeff>> poly1, poly2;
    for (const auto& line : contents1) {
        if (line == ";") continue;
        
        std::istringstream iss(line);
        int coeff;
        char x, caret;
        int power;
        if (iss >> coeff >> x >> caret >> power) {
            poly1.emplace_back(power, coeff);
        }
    }
    
    for (const auto& line : contents2) {
        if (line == ";") continue;
        
        std::istringstream iss(line);
        int coeff;
        char x, caret;
        int power;
        if (iss >> coeff >> x >> caret >> power) {
            poly2.emplace_back(power, coeff);
        }
    }
    std::sort(poly1.begin(), poly1.end(), 
              [](const auto& a, const auto& b) { return a.first > b.first; });
    std::sort(poly2.begin(), poly2.end(), 
              [](const auto& a, const auto& b) { return a.first > b.first; });

    if (poly1.size() != poly2.size()) {
        std::cout << "Different number of terms after parsing: " 
                  << poly1.size() << " vs " << poly2.size() << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < poly1.size(); ++i) {
        if (poly1[i].first != poly2[i].first || poly1[i].second != poly2[i].second) {
            std::cout << "Mismatch at term " << i << ": " 
                      << poly1[i].second << "x^" << poly1[i].first << " vs "
                      << poly2[i].second << "x^" << poly2[i].first << std::endl;
            return false;
        }
    }
    
    return true;
}

int main() {
    std::ifstream poly_file("simple_poly.txt");
    if (!poly_file.is_open()) {
        std::cerr << "Failed to open simple_poly.txt\n";
        return 1;
    }
    
    // Read two polynomials
    auto poly1_terms = parse_polynomial(poly_file);
    auto poly2_terms = parse_polynomial(poly_file);
    poly_file.close();
    
    // Create polynomials
    polynomial p1(poly1_terms.begin(), poly1_terms.end());
    polynomial p2(poly2_terms.begin(), poly2_terms.end());
    
    // Report polynomial sizes
    std::cout << "Polynomial 1: " << poly1_terms.size() << " terms\n";
    std::cout << "Polynomial 2: " << poly2_terms.size() << " terms\n";
    
    // Time the multiplication
    auto start = std::chrono::high_resolution_clock::now();
    polynomial product = p1 * p2;
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculate duration in different units
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto duration_s = duration_ms / 1000.0;
    
    // Report timing
    std::cout << "Multiplication completed in " << duration_ms << " milliseconds (" << duration_s << " seconds)\n";
    std::ofstream out("output.txt");
    
    auto result_terms = product.canonical_form();
    
    for (auto it = result_terms.rbegin(); it != result_terms.rend(); ++it) {
        out << it->second << "x^" << it->first << std::endl;
    }
    out << ";" << std::endl;
    out.close();
    
    std::cout << "\nComparing output.txt with result.txt...\n";
    if (compare_files("output.txt", "result.txt")) {
        std::cout << "Files match.\n";
    }
    else {
        std::cout << "Files do not match.\n";
    }
    return 0;

}
// std::optional<double> poly_test(polynomial& p1,
//                                 polynomial& p2,
//                                 std::vector<std::pair<power, coeff>> solution)

// {
//     std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

//     polynomial p3 = p1 * p2;

//     auto p3_can_form = p3.canonical_form();

//     std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

//     if (p3_can_form != solution)
//     {
//         return std::nullopt;
//     }

//     return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
// }

// void given_test() {
//     /** We're doing (x+1)^2, so solution is x^2 + 2x + 1*/
//     std::vector<std::pair<power, coeff>> solution = {{2,1}, {1,2}, {0,1}};

//     /** This holds (x+1), which we'll pass to each polynomial */
//     std::vector<std::pair<power, coeff>> poly_input = {{1,1}, {0,1}};


//     polynomial p1(poly_input.begin(), poly_input.end());
//     polynomial p2(poly_input.begin(), poly_input.end());

//     std::optional<double> result = poly_test(p1, p2, solution);

//     if (result.has_value())
//     {
//         std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
//     } 
//     else 
//     {
//         std::cout << "Failed test" << std::endl;
//     }

//     std::vector<std::pair<power, coeff>> dividend_terms = {{2, 1}, {1, 2}, {0, 1}};
//     std::vector<std::pair<power, coeff>> divisor_terms = {{1, 1}, {0, 1}};
//     std::vector<std::pair<power, coeff>> expected_remainder = {{0, 0}};

//     polynomial dividend(dividend_terms.begin(), dividend_terms.end());
//     polynomial divisor(divisor_terms.begin(), divisor_terms.end());

//     polynomial remainder = dividend % divisor;

//     auto result_modulo = remainder.canonical_form();

//     remainder.print();

//     if (result_modulo == expected_remainder) {
//         std::cout << "Modulo test passed." << std::endl;
//     } else {
//         std::cout << "Modulo test failed." << std::endl;
//     }
//     return 0;
// }

// // power stringToPower(const std::string& str) {
// //     try {
// //         power result = std::stoull(str); // Use std::stoull for unsigned long long
// //         // Optional: Add a check to ensure the result fits within power's range
// //         if (result > std::numeric_limits<power>::max()) {
// //             throw std::out_of_range("Value exceeds the range of size_t");
// //         }
// //         return result;
// //     } catch (const std::invalid_argument& e) {
// //         std::cerr << "Invalid argument: " << e.what() << std::endl;
// //         throw; // Re-throw the exception if you want the caller to handle it
// //     } catch (const std::out_of_range& e) {
// //         std::cerr << "Out of range: " << e.what() << std::endl;
// //         throw; // Re-throw the exception
// //     }
// // }

// void read_txt(std::string input_file, std::string expected_file) {
//     std::ifstream f(input_file);

//     // String variable to store the read data
//     std::vector<std::string> lines;
//     std::string s;

//     // make polynomials from given txt
//     std::vector<polynomial> poly;
//     std::vector<std::pair<power, coeff>> input;
//     while (getline(f, s)) {
//         // lines.push_back(s);
//         if (s == ";") {
//             polynomial p(input.begin(), input.end());
//             poly.push_back(p);
//             input.clear();
//             continue;
//         }

//         size_t split_idx = s.find("x^");
//         power p = std::stoull(s.substr(split_idx+2));
//         coeff c = std::stoi(s.substr(0, split_idx));
//         // std::cout << "Power: " << p << " | Coeff: " << c << std::endl;
//         input.push_back(std::make_pair(p, c));
//     }
//     f.close();

//     // read expected result
//     std::vector<std::pair<power, coeff>> expected;
//     std::ifstream f2(expected_file);
//     while (getline(f2, s)) {
//         // lines.push_back(s);
//         if (s == ";") {
//             break;
//         }

//         size_t split_idx = s.find("x^");
//         power p = std::stoull(s.substr(split_idx+2));
//         coeff c = std::stoi(s.substr(0, split_idx));
//         // std::cout << "Power: " << p << " | Coeff: " << c << std::endl;
//         expected.push_back(std::make_pair(p, c));
//     }
//     f.close();

//     polynomial expected_poly(expected.begin(), expected.end());

//     // get product
//     std::cout << "Calculating products..." << std::endl;

//     polynomial p1 = poly[0];
//     polynomial p2 = poly[1];

//     std::optional<double> result = poly_test(p1, p2, expected_poly.canonical_form());

//     if (result.has_value())
//     {
//         std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
//     } 
//     else 
//     {
//         std::cout << "Failed test" << std::endl;
//     }

// }

// int main()
// {
//     // given_test();
//     read_txt("simple_poly.txt", "result.txt");

// }
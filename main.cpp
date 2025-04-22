#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
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
    } else {
        std::cout << "Files do not match.\n";
    }
    return 0;
}
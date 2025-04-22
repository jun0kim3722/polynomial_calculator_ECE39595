#include "poly.h"

polynomial::polynomial() {
    coeff_map[0] = 0;
    degree = 0;
}

polynomial::polynomial(const polynomial &other) {
    coeff_map = other.coeff_map;
    degree = other.degree;
}

polynomial &polynomial::operator=(const polynomial &other) {
    if (this != &other) {
        coeff_map = other.coeff_map;
        degree = other.degree;
    }
    return *this;
}

polynomial polynomial::operator+(const polynomial &other) const {
    polynomial result(other);
    power max_degree = std::max(degree, other.degree);
    result.degree = max_degree;

    for (const auto& [p, c] : coeff_map) {
        result.coeff_map[p] += c;
    }

    return result;
}

polynomial polynomial::operator+(const int val) const {
    polynomial result(*this);
    result.coeff_map[0] += val;

    return result;
}

polynomial operator+(const int val, const polynomial &other) {
    polynomial temp(other);
    return temp + val;
}

void polynomial::multiply_range(const polynomial& other, 
                               std::map<power, coeff>& result_map,
                               std::mutex& result_mutex,
                               size_t start, 
                               size_t end,
                               const std::vector<std::pair<power, coeff>>& coeffs1) const {

    std::map<power, coeff> local_result;
    
    for (size_t i = start; i < end && i < coeffs1.size(); ++i) {
        const auto& [power1, coeff1] = coeffs1[i];
        
        for (const auto& [power2, coeff2] : other.coeff_map) {
            power new_power = power1 + power2;
            coeff new_coeff = coeff1 * coeff2;
            
            local_result[new_power] += new_coeff;
        }
    }
    std::lock_guard<std::mutex> lock(result_mutex);
    for (const auto& [p, c] : local_result) {
        result_map[p] += c;
    }
}

polynomial polynomial::operator*(const polynomial &other) const {
    if (coeff_map.empty() || other.coeff_map.empty()) {
        polynomial result;
        return result;
    }
    polynomial result;
    std::mutex result_mutex;
    std::vector<std::thread> threads;
    
    std::vector<std::pair<power, coeff>> coeffs1;
    for (const auto& term : coeff_map) {
        coeffs1.push_back(term);
    }
    size_t chunk_size = (coeffs1.size() + NUM_THREADS - 1) / NUM_THREADS;
    
    for (size_t i = 0; i < NUM_THREADS; ++i) {
        size_t start = i * chunk_size;
        size_t end = std::min(start + chunk_size, coeffs1.size());
        
        if (start >= coeffs1.size()) break;
        
        threads.emplace_back(&polynomial::multiply_range, this, 
                            std::ref(other), 
                            std::ref(result.coeff_map), 
                            std::ref(result_mutex),
                            start, end, std::ref(coeffs1));
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    for (auto it = result.coeff_map.begin(); it != result.coeff_map.end();) {
        if (it->second == 0) {
            it = result.coeff_map.erase(it);
        } else {
            ++it;
        }
    }
    
    result.degree = 0;
    for (const auto& [p, c] : result.coeff_map) {
        if (c != 0) {
            result.degree = std::max(result.degree, p);
        }
    }
    
    return result;
}

polynomial polynomial::operator*(const int val) const {
    polynomial result(*this);

    for (auto& [p, c] : result.coeff_map) {
        result.coeff_map[p] *= val;
    }

    return result;
}

polynomial operator*(const int val, const polynomial &other) {
    polynomial temp(other);
    return temp * val;
}

polynomial polynomial::operator%(const polynomial &other) const {
    if (other.degree > degree) {
        return *this;
    }
    
    if (other.coeff_map.empty() || (other.coeff_map.size() == 1 && other.coeff_map.at(0) == 0)) {
        return *this;
    }
    
    polynomial result(*this);
    power divisor_degree = other.degree;
    coeff divisor_leading_coeff = 0;
    
    for (const auto& [p, c] : other.coeff_map) {
        if (p == divisor_degree) {
            divisor_leading_coeff = c;
            break;
        }
    }

    while (!result.coeff_map.empty() && result.degree >= divisor_degree) {
        power remainder_degree = result.degree;
        coeff remainder_leading_coeff = 0;
        
        for (const auto& [p, c] : result.coeff_map) {
            if (p == remainder_degree) {
                remainder_leading_coeff = c;
                break;
            }
        }
        if (remainder_leading_coeff == 0) {
            result.coeff_map.erase(remainder_degree);
            result.degree = 0;
            for (const auto& [p, c] : result.coeff_map) {
                if (p > result.degree) {
                    result.degree = p;
                }
            }
            continue;
        }
        power quotient_power = remainder_degree - divisor_degree;
        coeff quotient_coeff = remainder_leading_coeff / divisor_leading_coeff;
        
        if (remainder_leading_coeff % divisor_leading_coeff != 0) {
            break;
        }
        for (const auto& [p, c] : other.coeff_map) {
            result.coeff_map[p + quotient_power] -= c * quotient_coeff;
            
            if (result.coeff_map[p + quotient_power] == 0) {
                result.coeff_map.erase(p + quotient_power);
            }
        }
        
        result.degree = 0;
        for (const auto& [p, c] : result.coeff_map) {
            if (p > result.degree) {
                result.degree = p;
            }
        }
    }
    
    return result;
}

void polynomial::print() const {
    std::cout << "Degree: " << degree << std::endl;
    for (const auto& [p, c] : coeff_map) {
        std::cout << c;
        if (p == 0) {
            std::cout << " + ";
        }
        else {
            std::cout << "x^"  << p << " + ";
        }
    }
    std::cout << std::endl << std::endl;
}

size_t polynomial::find_degree_of() {
    return degree;
}

std::vector<std::pair<power, coeff>> polynomial::canonical_form() const {
    std::vector<std::pair<power, coeff>> result;

    for (const auto& [p, c] : coeff_map) {
        if (c != 0) {
            result.emplace_back(p, c);
        }
    }

    if (result.empty()) {
        return {std::make_pair(0, 0)};
    }

    for (size_t i = 1; i < result.size(); ++i) {
        auto key = result[i];
        size_t j = i;
        while (j > 0 && result[j - 1].first < key.first) {
            result[j] = result[j - 1];
            --j;
        }
        result[j] = key;
    }

    return result;
}

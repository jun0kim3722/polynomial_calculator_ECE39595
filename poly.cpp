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

polynomial polynomial::operator*(const polynomial &other) const {
    polynomial result;
    for (const auto& [power1, coeff1] : coeff_map) {
        for (const auto& [power2, coeff2] : other.coeff_map) {
            power new_power = power1 + power2;
            int new_coeff = coeff1 * coeff2;

            result.coeff_map[new_power] += new_coeff;
            result.degree = std::max(new_power, result.degree);
        }
    }

    return result;
}

polynomial polynomial::operator*(const int val) const {
    polynomial result(*this);

    for (const auto& [p, c] : result.coeff_map) {
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

    polynomial result(*this);
    polynomial divisor(other);

    while (divisor.degree <= result.degree) {
        power diff = result.degree - divisor.degree;
        std::vector<std::pair<power, coeff>> temp = {{diff, 1}};
        polynomial temp_poly(temp.begin(), temp.end());

        polynomial subtraction = temp_poly * divisor * -1;
        result = result + subtraction;

        for (auto it = result.coeff_map.begin(); it != result.coeff_map.end();) {
            if (it->second == 0) {
                it = result.coeff_map.erase(it);
            } else {
                ++it;
            }
        }

        if (result.coeff_map.empty()) {
            result.degree = 0;
            result.coeff_map[0] = 0;
        }
        else {
            result.degree = 0;
            for (const auto& [p, c] : result.coeff_map) {
                if (c != 0) {
                    result.degree = std::max(result.degree, p);
                }
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
    if (degree == 0) {
        return {std::make_pair(0, coeff_map.at(0))};
    }

    std::vector<std::pair<power, coeff>> result;
    for (const auto& [p, c] : coeff_map) {
        if (c > 0) {
            result.push_back(std::make_pair(p, c));
        }
    }

    for (size_t i = 1; i < result.size(); ++i) {
        std::pair<power, coeff> new_pair = result[i];
        size_t j = i;
    
        while (j > 0 && result[j - 1].first < new_pair.first) {
            result[j] = result[j - 1];
            --j;
        }
    
        result[j] = new_pair;
    }

    return result;
}


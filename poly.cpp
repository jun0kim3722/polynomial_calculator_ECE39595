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

void fft(std::vector<std::complex<double>> &a, bool is_invert) {
    power n = a.size();
    if (n <= 1) return;

    std::vector<std::complex<double>> a0(n / 2), a1(n / 2);
    for (power i = 0; i * 2 < n; i++) {
        a0[i] = a[i*2];
        a1[i] = a[i*2 + 1];
    }

    fft(a0, is_invert);
    fft(a1, is_invert);

    for (power i = 0; i * 2 < n; i++) {
        double dir = is_invert ? -1.0 : 1.0;
        double angle = (2 * PI * i / n) * dir;
        std::complex<double> w(cos(angle), sin(angle));
        a[i] = a0[i] + w * a1[i];
        a[i + n/2] = a0[i] - w * a1[i];
    }

    if (is_invert) {
        for (std::complex<double>& x : a) {
            x /= 2;
        }
    }
}

std::vector<std::complex<double>> convert2complex(const std::map<power, coeff> &m, size_t size) {
    std::vector<std::complex<double>> vec(size);
    for (const auto& [p, c] : m) {
        vec[p] = std::complex<double>(c, 0);
    }
    return vec;
}

bool polynomial::is_sparse(double threshold) const {
    if (degree == 0) return true;
    if (coeff_map.size() < 100) return true; 
    double density = static_cast<double>(coeff_map.size()) / (degree + 1);
    return density < threshold;
}

polynomial polynomial::operator*(const polynomial &other) const {
    power sum_deg = degree + other.degree;

    if (is_sparse() || other.is_sparse()) {
        // normal mul
        polynomial result;
        for (const auto& [power1, coeff1] : coeff_map) {
            for (const auto& [power2, coeff2] : other.coeff_map) {
                power new_power = power1 + power2;
                int new_coeff = coeff1 * coeff2;
    
                result.coeff_map[new_power] += new_coeff;
            }
        }
        result.degree = result.coeff_map.rbegin() -> first;
        return result;
    }

    // Start FFT
    size_t n = 1;
    while (n <= sum_deg) {
        n <<= 1;
    }
    n <<= 1;

    std::vector<std::complex<double>> A = convert2complex(coeff_map, n);
    std::vector<std::complex<double>> B = convert2complex(other.coeff_map, n);

    std::thread t1(fft, std::ref(A), false);
    std::thread t2(fft, std::ref(B), false);
    t1.join();
    t2.join();

    std::vector<std::complex<double>> C(n);
    for (size_t i = 0; i < n; ++i) {
        C[i] = A[i] * B[i];
    }

    fft(C, true); // inverse

    std::map<power, coeff> result_map;

    for (size_t i = 0; i <= sum_deg; ++i) {
        // int val = std::round(C[i].real());
        // int val = (std::abs(C[i].real()) < 1e-3) ? 0 : std::round(C[i].real());
        double d_val = static_cast<int64_t>(std::round(C[i].real()));
        int val;
        if (d_val > 2147483647 || d_val < -2147483647) {
            val = static_cast<int32_t>(d_val);
        }
        else {
            val = static_cast<coeff>(d_val);
        }

        if (val != 0) {
            result_map[i] = val;
        }
    }

    polynomial result;
    result.coeff_map = std::move(result_map);
    result.degree = result.coeff_map.rbegin() -> first;
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
    for (auto i = coeff_map.rbegin(); i != coeff_map.rend(); i++) {
        power p = i -> first;
        coeff c = i -> second;

        if (c != 0) {
            degree = p;
            break;
        }
    }
    // degree = coeff_map.rbegin() -> first;
    return degree;
}

std::vector<std::pair<power, coeff>> polynomial::canonical_form() const {
    std::vector<std::pair<power, coeff>> result;

    for (auto i = coeff_map.rbegin(); i != coeff_map.rend(); i++) {
        power p = i -> first;
        coeff c = i -> second;
        if (c != 0) {
            result.emplace_back(p, c);
        }
    }

    if (result.empty()) {
        return {std::make_pair(0, 0)};
    }

    return result;
}

#pragma once
#include <vector>
#include <string>
#include <variant>
#include <cstdint> // for uintptr_t
// A variant holding supported C++ types

struct PyClass {
    uintptr_t ptr;

    explicit PyClass(uintptr_t p) : ptr(p) {}
};

using ParamVariant = std::variant<int, float, bool, std::string, uintptr_t, PyClass>;
class Pars {
public:
    std::vector<ParamVariant> params;

    Pars() = default;

    // Variadic constructor
    template<typename... Args>
    Pars(Args&&... args) {
        AddParams(std::forward<Args>(args)...);
    }

    void AddInt(int val) {
        params.emplace_back(val);
    }

    void AddFloat(float val) {
        params.emplace_back(val);
    }

    void AddBool(bool val) {
        params.emplace_back(val);
    }

    void AddString(const std::string& val) {
        params.emplace_back(val);
    }

    void AddPointer(uintptr_t val) {
        params.emplace_back(val);
    }

    // Variadic AddParams method
    template<typename T, typename... Rest>
    void AddParams(T&& first, Rest&&... rest) {
        AddOne(std::forward<T>(first));
        AddParams(std::forward<Rest>(rest)...);
    }

    void AddParams() {} // End recursion

private:
    void AddOne(int val) {
        AddInt(val);
    }

    void AddOne(float val) {
        AddFloat(val);
    }

    void AddOne(bool val) {
        AddBool(val);
    }

    void AddOne(const std::string& val) {
        AddString(val);
    }

    void AddOne(const char* val) {
        AddString(std::string(val));
    }

    void AddOne(uintptr_t val) {
        AddPointer(val);
    }

    void AddOne(PyClass val) {
        params.emplace_back(val);
    }

    // Catch unsupported types
    template<typename T>
    void AddOne(T&&) {
        static_assert(sizeof(T) == 0, "Unsupported type in Pars::AddParams");
    }
};
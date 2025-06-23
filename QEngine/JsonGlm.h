#pragma once
#include "json.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using json = nlohmann::json;
// This namespace contains all the helper functions that teach the nlohmann::json
// library how to properly serialize and deserialize GLM's vector and matrix types.

namespace glm {
    inline void to_json(json& j, const glm::vec2& v) {
        j = { v.x, v.y };
    }
    inline void from_json(const json& j, glm::vec2& v) {
        j.at(0).get_to(v.x);
        j.at(1).get_to(v.y);
    }

    inline void to_json(json& j, const glm::vec3& v) {
        j = { v.x, v.y, v.z };
    }
    inline void from_json(const json& j, glm::vec3& v) {
        j.at(0).get_to(v.x);
        j.at(1).get_to(v.y);
        j.at(2).get_to(v.z);
    }

    inline void to_json(json& j, const glm::vec4& v) {
        j = { v.x, v.y, v.z, v.w };
    }
    inline void from_json(const json& j, glm::vec4& v) {
        j.at(0).get_to(v.x);
        j.at(1).get_to(v.y);
        j.at(2).get_to(v.z);
        j.at(3).get_to(v.w);
    }

    inline void to_json(json& j, const glm::mat4& m) {
        j = {
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]
        };
    }
    inline void from_json(const json& j, glm::mat4& m) {
        for (int i = 0; i < 4; ++i)
            for (int k = 0; k < 4; ++k)
                j.at(i * 4 + k).get_to(m[i][k]);
    }
}
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <cstdint> // For uint32_t
#include <glm/glm.hpp>

// FreeType Headers
#include "ft2build.h"
#include FT_FREETYPE_H

#include "Texture2D.h"
#include "Draw2D.h"

class GraphNode;

// Holds the data for a single character glyph
struct Character
{
    std::unique_ptr<Q3D::Engine::Texture::Texture2D> texture;
    std::vector<float> pixelData;
    int width = 0;
    int height = 0;
    int bearingX = 0;
    int bearingY = 0;
    long advance = 0;
};

// Holds the generated texture and layout data for a cached string
struct CachedText
{
    std::unique_ptr<Q3D::Engine::Texture::Texture2D> texture;
    glm::vec2 size;
    int ascent;
    float bearingX; // For correct horizontal placement
};


class GameFont
{
public:
    GameFont(const std::string& fontPath, int fontSize, GraphNode* camera);
    ~GameFont();
    void SetDraw(Draw2D* draw) {
        m_drawer.reset(draw);
    }
    GameFont(const GameFont&) = delete;
    GameFont& operator=(const GameFont&) = delete;

    // Renders text by creating a single texture for the entire string and caching it.
    // This is the recommended, high-performance method.
    void DrawTextAsTexture(const std::string& text, glm::vec2 position, float scale, glm::vec4 color);

    // Renders text by drawing each character's texture individually. Slower and not cached.
    void DrawText(const std::string& text, glm::vec2 position, float scale, glm::vec4 color);

    void ClearTextCache();
    int GetTextWidth(const std::string& text, float scale);
    int GetTextHeight(const std::string& text, float scale);

private:
    // Decodes a UTF-8 string into a vector of 32-bit Unicode code points.
    std::vector<uint32_t> DecodeUTF8(const std::string& text) const;

    // Gets a character from the cache or loads it from the font file if it's new.
    Character& GetOrLoadCharacter(uint32_t charCode);

    FT_Library m_ft;
    FT_Face m_face; // We keep the font face loaded for on-demand glyph loading

    // The character map is now keyed by uint32_t to support all of Unicode
    std::unordered_map<uint32_t, Character> m_characters;
    std::unique_ptr<Draw2D> m_drawer;

    // Cache for previously generated text textures
    std::unordered_map<std::string, CachedText> m_textCache;
};
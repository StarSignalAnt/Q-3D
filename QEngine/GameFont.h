#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
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
    std::unique_ptr<Texture2D> texture;
    std::vector<float> pixelData;
    int width;
    int height;
    int bearingX;
    int bearingY;
    long advance;
};

// Holds the generated texture and layout data for a cached string
struct CachedText
{
    std::unique_ptr<Texture2D> texture;
    glm::vec2 size;
    int ascent; // Stored to correctly calculate render position
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


    void DrawText(const std::string& text, glm::vec2 position, float scale, glm::vec4 color);

  
    void DrawTextAsTexture(const std::string& text, glm::vec2 position, float scale, glm::vec4 color);


    void ClearTextCache();
    int GetTextWidth(const std::string& text, float scale);

    int GetTextHeight(const std::string& text, float scale);
    Character& GetCharacter(unsigned char c);

private:
    FT_Library m_ft;
    FT_Face m_face;

    std::unordered_map<unsigned char, Character> m_characters;
    std::unique_ptr<Draw2D> m_drawer;

    // Cache for previously generated text textures
    std::unordered_map<std::string, CachedText> m_textCache;
};
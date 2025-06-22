#include "GameFont.h"
#include <iostream>
#include <algorithm>
#include <sstream>  // For generating cache keys
#include <iomanip>  // For float precision in cache keys

#include "GraphNode.h"

// ... GameFont Constructor and Destructor remain the same as the previous version ...
GameFont::GameFont(const std::string& fontPath, int fontSize, GraphNode* camera)
    : m_ft(nullptr), m_face(nullptr)
{
    // This constructor implementation is unchanged from the previous step.
    // It correctly loads glyphs and populates the character pixelData.
    if (FT_Init_FreeType(&m_ft) || FT_New_Face(m_ft, fontPath.c_str(), 0, &m_face))
    {
        std::cerr << "ERROR::FREETYPE: Font loading failed." << std::endl;
        if (m_ft) FT_Done_FreeType(m_ft);
        return;
    }

    FT_Set_Pixel_Sizes(m_face, 0, fontSize);

    for (unsigned char c = 0; c < 255; ++c)
    {
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            continue;
        }

        FT_GlyphSlot glyph = m_face->glyph;
        FT_Bitmap* bitmap = &glyph->bitmap;
        int width = bitmap->width;
        int height = bitmap->rows;

        std::unique_ptr<Texture2D> characterTexture = nullptr;
        std::vector<float> pixelBuffer;

        if (width > 0 && height > 0)
        {
            pixelBuffer.resize(width * height * 4);
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    unsigned char alphaValue = bitmap->buffer[y * bitmap->pitch + x];
                    float normalizedAlpha = alphaValue / 255.0f;
                    int index = (y * width + x) * 4;
                    pixelBuffer[index + 0] = normalizedAlpha;
                    pixelBuffer[index + 1] = normalizedAlpha;
                    pixelBuffer[index + 2] = normalizedAlpha;
                    pixelBuffer[index + 3] = normalizedAlpha;
                }
            }
            characterTexture = std::make_unique<Texture2D>(width, height, pixelBuffer.data(), 4);
        }

        Character character = {
            std::move(characterTexture),
            std::move(pixelBuffer),
            width, height,
            glyph->bitmap_left, glyph->bitmap_top,
            glyph->advance.x >> 6
        };
        m_characters.emplace(c, std::move(character));
    }

    FT_Done_Face(m_face);
    m_face = nullptr;
    m_drawer = std::make_unique<Draw2D>(camera);
}

GameFont::~GameFont()
{
    if (m_ft) {
        FT_Done_FreeType(m_ft);
    }
}


void GameFont::DrawTextAsTexture(const std::string& text, glm::vec2 position, float scale, glm::vec4 color)
{
    if (!m_drawer || text.empty()) return;

    std::stringstream ss;
    ss << text << "_s:" << std::fixed << std::setprecision(3) << scale;
    std::string cacheKey = ss.str();

    auto it = m_textCache.find(cacheKey);
    if (it != m_textCache.end())
    {
        CachedText& cached = it->second;
        glm::vec2 renderPos = { position.x, position.y - cached.ascent };
        m_drawer->Rect(cached.texture.get(), renderPos, cached.size, color);
        return;
    }

    int totalWidth = 0;
    int maxAscent = 0;
    int maxDescent = 0;
    for (const char& c : text) {
        Character& ch = m_characters.at(c);
        totalWidth += ch.advance;
        maxAscent = std::max(maxAscent, ch.bearingY);
        maxDescent = std::max(maxDescent, ch.height - ch.bearingY);
    }
    int totalHeight = maxAscent + maxDescent;
    totalWidth = static_cast<int>(totalWidth * scale);
    totalHeight = static_cast<int>(totalHeight * scale);
    int scaledAscent = static_cast<int>(maxAscent * scale);
    if (totalWidth == 0 || totalHeight == 0) return;

    std::vector<float> finalBuffer(totalWidth * totalHeight * 4, 0.0f);
    glm::vec2 cursor = { 0.0f, 0.0f };
    for (const char& c : text) {
        Character& ch = m_characters.at(c);
        if (ch.pixelData.empty()) {
            cursor.x += ch.advance * scale;
            continue;
        }
        int targetX = static_cast<int>(cursor.x + ch.bearingX * scale);
        int targetY = static_cast<int>(scaledAscent - ch.bearingY * scale);
        int glyphW = static_cast<int>(ch.width * scale);
        int glyphH = static_cast<int>(ch.height * scale);
        for (int y = 0; y < glyphH; ++y) {
            for (int x = 0; x < glyphW; ++x) {
                int destX = targetX + x;
                int destY = targetY + y;
                if (destX >= 0 && destX < totalWidth && destY >= 0 && destY < totalHeight) {
                    int srcX = static_cast<int>((float)x / scale);
                    int srcY = static_cast<int>((float)y / scale);
                    int srcIndex = (srcY * ch.width + srcX) * 4;
                    int destIndex = (destY * totalWidth + destX) * 4;

                    if (srcIndex + 3 < ch.pixelData.size()) {
                        // *** THE FIX IS HERE ***
                        // We now copy all four RGBA channels, not just Alpha.
                        finalBuffer[destIndex + 0] = ch.pixelData[srcIndex + 0]; // R
                        finalBuffer[destIndex + 1] = ch.pixelData[srcIndex + 1]; // G
                        finalBuffer[destIndex + 2] = ch.pixelData[srcIndex + 2]; // B
                        finalBuffer[destIndex + 3] = ch.pixelData[srcIndex + 3]; // A
                    }
                }
            }
        }
        cursor.x += ch.advance * scale;
    }

    auto newTexture = std::make_unique<Texture2D>(totalWidth, totalHeight, finalBuffer.data(), 4);
    glm::vec2 renderPos = { position.x, position.y - scaledAscent };
    m_drawer->Rect(newTexture.get(), renderPos, { (float)totalWidth, (float)totalHeight }, color);

    CachedText cacheEntry;
    cacheEntry.texture = std::move(newTexture);
    cacheEntry.size = { (float)totalWidth, (float)totalHeight };
    cacheEntry.ascent = scaledAscent;
    m_textCache[cacheKey] = std::move(cacheEntry);
}

void GameFont::ClearTextCache()
{
    m_textCache.clear();
}

// DrawText (char-by-char) and GetCharacter methods are unchanged
void GameFont::DrawText(const std::string& text, glm::vec2 position, float scale, glm::vec4 color)
{
    if (!m_drawer) return;
    glm::vec2 cursor = position;
    for (const char& c : text) {
        Character& ch = m_characters.at(c);
        float xpos = cursor.x + ch.bearingX * scale;
        float ypos = cursor.y - ch.bearingY * scale;
        float w = ch.width * scale;
        float h = ch.height * scale;
        if (ch.texture) {
            m_drawer->Rect(ch.texture.get(), { xpos, ypos }, { w, h }, color);
        }
        cursor.x += ch.advance * scale;
    }
}

Character& GameFont::GetCharacter(unsigned char c)
{
    return m_characters.at(c);
}


int GameFont::GetTextWidth(const std::string& text, float scale)
{
    if (text.empty())
    {
        return 0;
    }

    float totalWidth = 0;
    for (const char& c : text)
    {
        // Add the horizontal advance of each character
        totalWidth += m_characters.at(c).advance;
    }

    return static_cast<int>(totalWidth * scale);
}

int GameFont::GetTextHeight(const std::string& text, float scale)
{
    if (text.empty())
    {
        return 0;
    }

    int maxAscent = 0;
    int maxDescent = 0;

    // Find the highest point above the baseline (ascent) and the
    // lowest point below the baseline (descent) for all characters in the string.
    for (const char& c : text)
    {
        const Character& ch = m_characters.at(c);
        maxAscent = std::max(maxAscent, ch.bearingY);
        maxDescent = std::max(maxDescent, ch.height - ch.bearingY);
    }

    // The total height is the sum of the highest ascent and the lowest descent.
    return static_cast<int>((maxAscent + maxDescent) * scale);
}

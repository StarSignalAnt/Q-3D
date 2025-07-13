#include "GameFont.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "GraphNode.h"

// Constructor: Initializes FreeType and loads the font face, but does not pre-load any characters.
GameFont::GameFont(const std::string& fontPath, int fontSize, GraphNode* camera)
    : m_ft(nullptr), m_face(nullptr)
{
    if (FT_Init_FreeType(&m_ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(m_ft, fontPath.c_str(), 0, &m_face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
        // Clean up already initialized FreeType library
        FT_Done_FreeType(m_ft);
        m_ft = nullptr;
        return;
    }

    FT_Set_Pixel_Sizes(m_face, 0, fontSize);
    m_drawer = std::make_unique<Draw2D>(camera);
}

// Destructor: Correctly cleans up the FreeType face and library.
GameFont::~GameFont()
{
    if (m_face) {
        FT_Done_Face(m_face);
    }
    if (m_ft) {
        FT_Done_FreeType(m_ft);
    }
}

// Decodes a UTF-8 string to a vector of Unicode code points.
std::vector<uint32_t> GameFont::DecodeUTF8(const std::string& text) const
{
    std::vector<uint32_t> result;
    result.reserve(text.length());
    try {
        for (size_t i = 0; i < text.length();) {
            uint32_t charCode = 0;
            unsigned char c1 = text.at(i++);
            if (c1 < 0x80) { // 1-byte sequence (ASCII)
                charCode = c1;
            }
            else if ((c1 & 0xE0) == 0xC0) { // 2-byte sequence
                unsigned char c2 = text.at(i++);
                charCode = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
            }
            else if ((c1 & 0xF0) == 0xE0) { // 3-byte sequence
                unsigned char c2 = text.at(i++);
                unsigned char c3 = text.at(i++);
                charCode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
            }
            else if ((c1 & 0xF8) == 0xF0) { // 4-byte sequence
                unsigned char c2 = text.at(i++);
                unsigned char c3 = text.at(i++);
                unsigned char c4 = text.at(i++);
                charCode = ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
            }
            result.push_back(charCode);
        }
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Error: Malformed UTF-8 sequence in text string." << std::endl;
    }
    return result;
}


// Lazy-loading function: Gets a character from the cache or loads it from the font file.
Character& GameFont::GetOrLoadCharacter(uint32_t charCode)
{
    // Return character if it's already in our map.
    if (m_characters.count(charCode)) {
        return m_characters.at(charCode);
    }

    // If not found, try to load the glyph from the font face.
    if (FT_Load_Char(m_face, charCode, FT_LOAD_RENDER)) {
        std::cerr << "Warning: Failed to load glyph for character code: " << charCode << " (U+" << std::hex << charCode << ")" << std::dec << std::endl;
        // If loading fails, try to return a placeholder '?' character.
        // This prevents crashing if a font is missing a character.
        if (charCode != '?') {
            return GetOrLoadCharacter('?');
        }
        else {
            // If '?' itself fails, emplace an empty character to prevent trying again.
            return m_characters.emplace(charCode, Character{}).first->second;
        }
    }

    FT_GlyphSlot glyph = m_face->glyph;
    FT_Bitmap* bitmap = &glyph->bitmap;
    int width = bitmap->width;
    int height = bitmap->rows;

    std::unique_ptr<Q3D::Engine::Texture::Texture2D> characterTexture = nullptr;
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
        characterTexture = std::make_unique<Q3D::Engine::Texture::Texture2D>(width, height, pixelBuffer.data(), 4);
    }

    Character character = {
        std::move(characterTexture),
        std::move(pixelBuffer),
        width, height,
        glyph->bitmap_left, glyph->bitmap_top,
        (long)(glyph->advance.x >> 6)
    };

    // Insert the newly loaded character into the map and return a reference to it.
    return m_characters.emplace(charCode, std::move(character)).first->second;
}

void GameFont::DrawTextAsTexture(const std::string& text, glm::vec2 position, float scale, glm::vec4 color)
{
    if (!m_drawer || text.empty()) return;

    std::stringstream ss;
    ss << text << "_s:" << std::fixed << std::setprecision(3) << scale;
    std::string cacheKey = ss.str();
    auto it = m_textCache.find(cacheKey);
    if (it != m_textCache.end()) {
        CachedText& cached = it->second;
        glm::vec2 renderPos = { position.x + cached.bearingX, position.y - cached.ascent };
        m_drawer->Rect(cached.texture.get(), renderPos, cached.size, color);
        return;
    }

    std::vector<uint32_t> unicodeText = DecodeUTF8(text);
    if (unicodeText.empty()) return;

    int minX = 0, maxX = 0;
    int maxAscent = 0, maxDescent = 0;
    int penX = 0;
    for (uint32_t charCode : unicodeText) {
        Character& ch = GetOrLoadCharacter(charCode);
        int leftEdge = penX + ch.bearingX;
        int rightEdge = leftEdge + ch.width;
        minX = std::min(minX, leftEdge);
        maxX = std::max(maxX, rightEdge);
        maxAscent = std::max(maxAscent, ch.bearingY);
        maxDescent = std::max(maxDescent, ch.height - ch.bearingY);
        penX += ch.advance;
    }

    int totalWidth = (maxX - minX);
    int totalHeight = (maxAscent + maxDescent);

    int scaledWidth = static_cast<int>(totalWidth * scale);
    int scaledHeight = static_cast<int>(totalHeight * scale);
    int scaledAscent = static_cast<int>(maxAscent * scale);
    float initialBearingX = minX * scale;

    if (scaledWidth == 0 || scaledHeight == 0) return;

    std::vector<float> finalBuffer(scaledWidth * scaledHeight * 4, 0.0f);
    glm::vec2 cursor = { -initialBearingX, 0.0f };

    for (uint32_t charCode : unicodeText) {
        Character& ch = GetOrLoadCharacter(charCode);
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
                if (destX >= 0 && destX < scaledWidth && destY >= 0 && destY < scaledHeight) {
                    int srcX = static_cast<int>((float)x / scale);
                    int srcY = static_cast<int>((float)y / scale);
                    if (srcX < ch.width && srcY < ch.height) {
                        int srcIndex = (srcY * ch.width + srcX) * 4;
                        int destIndex = (destY * scaledWidth + destX) * 4;
                        finalBuffer[destIndex + 0] = ch.pixelData[srcIndex + 0];
                        finalBuffer[destIndex + 1] = ch.pixelData[srcIndex + 1];
                        finalBuffer[destIndex + 2] = ch.pixelData[srcIndex + 2];
                        finalBuffer[destIndex + 3] = ch.pixelData[srcIndex + 3];
                    }
                }
            }
        }
        cursor.x += ch.advance * scale;
    }

    auto newTexture = std::make_unique<Q3D::Engine::Texture::Texture2D>(scaledWidth, scaledHeight, finalBuffer.data(), 4);
    glm::vec2 renderPos = { position.x + initialBearingX, position.y - scaledAscent };
    m_drawer->Rect(newTexture.get(), renderPos, { (float)scaledWidth, (float)scaledHeight }, color);

    CachedText cacheEntry;
    cacheEntry.texture = std::move(newTexture);
    cacheEntry.size = { (float)scaledWidth, (float)scaledHeight };
    cacheEntry.ascent = scaledAscent;
    cacheEntry.bearingX = initialBearingX;
    m_textCache[cacheKey] = std::move(cacheEntry);
}

// Slower, non-cached rendering method. Also updated to handle UTF-8.
void GameFont::DrawText(const std::string& text, glm::vec2 position, float scale, glm::vec4 color)
{
    if (!m_drawer) return;

    std::vector<uint32_t> unicodeText = DecodeUTF8(text);
    glm::vec2 cursor = position;

    for (uint32_t charCode : unicodeText) {
        Character& ch = GetOrLoadCharacter(charCode);
        float xpos = cursor.x + ch.bearingX * scale;
        float ypos = cursor.y - (ch.height - ch.bearingY) * scale; // Adjusted for proper baseline alignment
        float w = ch.width * scale;
        float h = ch.height * scale;
        if (ch.texture) {
            m_drawer->Rect(ch.texture.get(), { xpos, ypos }, { w, h }, color);
        }
        cursor.x += ch.advance * scale;
    }
}

void GameFont::ClearTextCache()
{
    m_textCache.clear();
}

int GameFont::GetTextWidth(const std::string& text, float scale)
{
    if (text.empty()) return 0;
    std::vector<uint32_t> unicodeText = DecodeUTF8(text);

    int minX = 0, maxX = 0;
    int penX = 0;
    for (uint32_t charCode : unicodeText) {
        Character& ch = GetOrLoadCharacter(charCode);
        int leftEdge = penX + ch.bearingX;
        int rightEdge = leftEdge + ch.width;
        minX = std::min(minX, leftEdge);
        maxX = std::max(maxX, rightEdge);
        penX += ch.advance;
    }
    return static_cast<int>((maxX - minX) * scale);
}

int GameFont::GetTextHeight(const std::string& text, float scale)
{
    if (text.empty()) return 0;
    std::vector<uint32_t> unicodeText = DecodeUTF8(text);

    int maxAscent = 0;
    int maxDescent = 0;
    for (uint32_t charCode : unicodeText) {
        const Character& ch = GetOrLoadCharacter(charCode);
        maxAscent = std::max(maxAscent, ch.bearingY);
        maxDescent = std::max(maxDescent, ch.height - ch.bearingY);
    }
    return static_cast<int>((maxAscent + maxDescent) * scale);
}
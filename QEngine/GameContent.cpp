#include "GameContent.h"
#include "ContentFile.h"
#include <filesystem> // Required for std::filesystem::path
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include "VFile.h"
#include <vector>
#include "zlib.h" // Required for compression. Ensure you link against the zlib library.
#include <unordered_set> 
// Use the C++17 filesystem library namespace
namespace fs = std::filesystem;

std::string GenerateUniqueId(const fs::path& relativePath) {
    // Get the path string without the file extension.
    fs::path pathWithoutExt = relativePath.parent_path() / relativePath.stem();
    std::string id = pathWithoutExt.string();

    // Manually replace all directory separators with a dot.
    // This is more robust than std::replace if you are having compiler issues.
    for (char& c : id) {
        if (c == fs::path::preferred_separator || c == '\\' || c == '/') {
            c = '.';
        }
    }

    // Capitalize the first letter of the filename component.
    // For example, "MyFolder.test" becomes "MyFolder.Test".
    size_t lastDot = id.find_last_of('.');
    if (lastDot != std::string::npos && lastDot + 1 < id.length()) {
        id[lastDot + 1] = std::toupper(id[lastDot + 1]);
    }
    else if (lastDot == std::string::npos && !id.empty()) {
        // Handle case where there is no folder, just a file.
        id[0] = std::toupper(id[0]);
    }

    return id;
}


GameContent::GameContent(std::string path) {

	m_ContentPath = path;

}
void GameContent::ScanContent() {
    // Ensure the vector is clear before scanning new content.
    for (auto* content : m_Contents) {
        delete content;
    }
    m_Contents.clear();

    if (!fs::exists(m_ContentPath) || !fs::is_directory(m_ContentPath)) {
        std::cerr << "Error: Content path not found or is not a directory: " << m_ContentPath << std::endl;
        return;
    }

    fs::path contentPath(m_ContentPath);

    for (const auto& entry : fs::recursive_directory_iterator(contentPath)) {
        if (entry.is_regular_file()) {
            fs::path fullPath = entry.path();
            fs::path relativePath = fs::relative(fullPath, contentPath);

            std::string id = GenerateUniqueId(relativePath);

            // Get the file size in bytes.
            long fileSize = static_cast<long>(fs::file_size(entry));

            // Create the new ContentFile with the size and add it to our vector.
            ContentFile* file = new ContentFile(id, fullPath.string(), fileSize);
            m_Contents.push_back(file);
        }
    }
}

void GameContent::Debug_ListFiles() {
    std::cout << "--- Content File List ---\n";
    std::cout << "Total Files: " << m_Contents.size() << "\n\n";
    for (const auto* file : m_Contents) {
        if (file) {
            std::cout << "ID:              " << file->GetID() << "\n";
            std::cout << "Path:            " << file->GetFullPath() << "\n";
            std::cout << "Size:            " << file->GetSize() << " bytes\n";
            std::cout << "Compressed:      " << (file->IsCompressed() ? "Yes" : "No") << "\n";
            std::cout << "Stored Size:     " << file->GetCompressedSize() << " bytes\n";
            std::cout << "Archive Start:   " << file->GetStart() << "\n";
            std::cout << "----------------------------------\n";
        }
    }
}


void GameContent::BuildArchive() {
    if (m_Contents.empty()) {
        std::cout << "No content found to build archive. Run ScanContent() first.\n";
        return;
    }

    const std::unordered_set<std::string> uncompressedExtensions = {
        ".png", ".jpg", ".jpeg", ".zip", ".gz", ".mp3", ".ogg", ".mp4", ".mov"
    };

    fs::path basePath(m_ContentPath);
    std::string indexFilePath = (basePath / "archive.cindex").string();
    std::string contentFilePath = (basePath / "archive.content").string();

    VFile indexFile(indexFilePath.c_str(), FileMode::Write);
    VFile contentFile(contentFilePath.c_str(), FileMode::Write);

    std::cout << "Building archive with VFile (large file support enabled)...\n";

    // BUG FIX: Manually track the position in the archive file.
    // This avoids issues where VFile::GetPosition() might not report the
    // current position correctly inside the loop.
    long currentArchivePosition = 0;

    for (ContentFile* file : m_Contents) {
        // Ensure file is not empty or invalid
        if (file->GetSize() <= 0) {
            if (file->GetSize() < 0) std::cerr << "Warning: File has invalid size: " << file->GetFullPath() << std::endl;
            continue;
        }

        VFile srcFile(file->GetFullPath().c_str(), FileMode::Read);
        // Read the full file using 'long' for size, no longer limited to 'int'.
        void* srcBuffer = srcFile.ReadBytes(file->GetSize());
        srcFile.Close();

        if (!srcBuffer) {
            std::cerr << "Warning: Could not read file to archive: " << file->GetFullPath() << std::endl;
            continue;
        }

        long storedSize = 0;
        bool isCompressed = true;

        std::string extension = fs::path(file->GetFullPath()).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        // Use our manually tracked position as the start byte for this entry.
        long startByte = currentArchivePosition;

        if (uncompressedExtensions.count(extension)) {
            isCompressed = false;
            storedSize = file->GetSize();
            contentFile.WriteBytes(srcBuffer, storedSize);
        }
        else {
            isCompressed = true;
            uLong boundSize = compressBound(file->GetSize());
            std::vector<Bytef> compressedBuffer(boundSize);
            uLong actualCompressedSize = boundSize;

            if (compress(compressedBuffer.data(), &actualCompressedSize, (const Bytef*)srcBuffer, file->GetSize()) != Z_OK) {
                std::cerr << "Warning: zlib compression failed for file: " << file->GetFullPath() << std::endl;
                free(srcBuffer);
                continue;
            }
            storedSize = static_cast<long>(actualCompressedSize);
            contentFile.WriteBytes(compressedBuffer.data(), storedSize);
        }

        // Manually advance our position tracker by the number of bytes we just wrote.
        currentArchivePosition += storedSize;

        free(srcBuffer);

        // Updated to use the new function signature
        file->SetArchiveData(startByte, file->GetSize(), storedSize, isCompressed);

        indexFile.WriteString(file->GetID().c_str());
        indexFile.WriteString(file->GetFullPath().c_str());
        indexFile.WriteLong(file->GetStart());
        indexFile.WriteLong(file->GetSize());
        indexFile.WriteLong(file->GetCompressedSize());
        indexFile.WriteBool(file->IsCompressed());
    }

    indexFile.Close();
    contentFile.Close();

    std::cout << "Archive build complete.\n";
    std::cout << "Index: " << indexFilePath << "\n";
    std::cout << "Data:  " << contentFilePath << "\n";
}

void GameContent::ReadArchive() {
    for (auto* content : m_Contents) {
        delete content;
    }
    m_Contents.clear();
    if (m_ArchiveFile) {
        m_ArchiveFile->Close();
        delete m_ArchiveFile;
        m_ArchiveFile = nullptr;
    }

    fs::path basePath(m_ContentPath);
    std::string indexFilePath = (basePath / "archive.cindex").string();
    std::string contentFilePath = (basePath / "archive.content").string();

    if (!VFile::Exists(indexFilePath.c_str()) || !VFile::Exists(contentFilePath.c_str())) {
        std::cout << "Archive files not found. Cannot read archive.\n";
        return;
    }

    std::cout << "Reading archive index...\n";

    m_ArchiveFile = new VFile(contentFilePath.c_str(), FileMode::Read);
    VFile indexFile(indexFilePath.c_str(), FileMode::Read);
    long indexFileSize = VFile::Length(indexFilePath.c_str());

    while (indexFile.GetPosition() < indexFileSize && indexFile.GetPosition() != -1) {
        const char* id_cstr = indexFile.ReadString();
        const char* path_cstr = indexFile.ReadString();
        long start = indexFile.ReadLong();
        long size = indexFile.ReadLong();
        long compressed_size = indexFile.ReadLong();
        bool is_compressed = indexFile.ReadBool();

        ContentFile* newFile = new ContentFile(id_cstr, path_cstr, start, size, compressed_size, is_compressed);
        newFile->SetContentArchive(m_ArchiveFile);
        m_Contents.push_back(newFile);

        free((void*)id_cstr);
        free((void*)path_cstr);
    }

    indexFile.Close();
    std::cout << "Read " << m_Contents.size() << " entries from archive index.\n";
}

ContentFile* GameContent::FindContent(const std::string& fullPath)
{
    try {
        fs::path targetPath(fullPath);
        fs::path basePath(m_ContentPath);

        // Calculate the path relative to the content directory
        fs::path relativePath = fs::relative(targetPath, basePath);

        // Generate the ID in the same way it's generated during scanning
        std::string targetId = GenerateUniqueId(relativePath);

        // Search for the content file with the matching ID
        for (ContentFile* content : m_Contents) {
            if (content->GetID() == targetId) {
                return content;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        // This can happen if the path is not relative to the base
        std::cerr << "Filesystem error in FindContent: " << e.what() << std::endl;
        return nullptr;
    }

    // Return nullptr if no match is found
    return nullptr;
}

#include "ContentFile.h"
#include "VFile.h"
#include <filesystem> // Required for std::filesystem::path
#include "zlib.h" 
// Use the C++17 filesystem library namespace
namespace fs = std::filesystem;

ContentFile::ContentFile(std::string id, std::string fullPath,long size)
{

	m_ID = id;
	m_FullPath = fullPath;

	// Use std::filesystem to easily and reliably get the extension.
	fs::path filePath(fullPath);
	if (filePath.has_extension()) {
		m_Extension = filePath.extension().string();
	}
	m_Size = size;
}

ContentFile::ContentFile(long start, long size,long compressed_size, bool compressed) {

	m_Start = start;
	m_Size = size;
	m_CompressedSize = compressed_size;
	m_Compressed = compressed;

}

ContentFile::ContentFile(std::string id, std::string path, long start, long size, long compressed_size,bool compressed)
{
	m_ID = id;
	m_FullPath = path;
	m_Start = start;
	m_Size = size;
	m_CompressedSize = compressed_size;
	m_Compressed = compressed; // Assume content in archive is compressed

	fs::path filePath(path);
	if (filePath.has_extension()) {
		m_Extension = filePath.extension().string();
	}

}


void ContentFile::SetArchiveData(long start,long size, long compressed_size,bool compressed) {

	m_Size = size;
	m_Start = start;
	m_CompressedSize = compressed_size;
	m_Compressed = compressed;

}

void* ContentFile::LoadResource()
{
    if (m_InMemory) {
        return m_Data;
    }

    // If there's no archive, it's a loose file on disk.
    if (!m_ContentArchive) {
        std::cout << "Loading loose file: " << m_FullPath << std::endl;
        VFile file(m_FullPath.c_str(), FileMode::Read);
        m_Data = file.ReadBytes(m_Size);
        file.Close();
        m_InMemory = (m_Data != nullptr);
        return m_Data;
    }

    // --- Load from archive ---
    if (m_CompressedSize <= 0) {
        std::cerr << "Error: Cannot load resource " << m_ID << ", invalid stored size." << std::endl;
        return nullptr;
    }

    // Seek to the start of the file's data in the archive.
    m_ContentArchive->Seek(m_Start);

    // Read the stored (potentially compressed) data into a temporary buffer.
    void* compressedBuffer = m_ContentArchive->ReadBytes(m_CompressedSize);
    if (!compressedBuffer) {
        std::cerr << "Error: Failed to read data from archive for " << m_ID << std::endl;
        return nullptr;
    }

    if (m_Compressed) {
        // Allocate memory for the decompressed data.
        m_Data = malloc(m_Size);
        if (!m_Data) {
            std::cerr << "Error: Failed to allocate memory for decompression of " << m_ID << std::endl;
            free(compressedBuffer);
            return nullptr;
        }

        // Decompress the data.
        uLongf destLen = m_Size;
        int result = uncompress(static_cast<Bytef*>(m_Data), &destLen, static_cast<const Bytef*>(compressedBuffer), m_CompressedSize);

        // Free the temporary compressed buffer.
        free(compressedBuffer);

        if (result != Z_OK) {
            std::cerr << "Error: Decompression failed for " << m_ID << ". Zlib error: " << result << std::endl;
            free(m_Data);
            m_Data = nullptr;
            return nullptr;
        }

        // *** BUG FIX: Verify that the decompressed size matches the original file size. ***
        // If they don't match, the data is corrupt and will cause Assimp to fail.
        if (destLen != m_Size) {
            std::cerr << "Error: Decompression size mismatch for " << m_ID << ". "
                << "Expected: " << m_Size << " bytes, but got: " << destLen << " bytes." << std::endl;
            free(m_Data);
            m_Data = nullptr;
            return nullptr;
        }

    }
    else {
        // If not compressed, the data we read is the final data.
        m_Data = compressedBuffer;
    }

    m_InMemory = (m_Data != nullptr);
    return m_Data;
}

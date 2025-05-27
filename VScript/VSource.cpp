#include "pch.h"
#include "VSource.h"


std::string readFileAsString(const std::string& path) {
    // Create an input file stream (ifstream)
    std::ifstream fileStream(path);

    // Check if the file stream was successfully opened
    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    // Create a string stream to hold the contents of the file
    std::stringstream buffer;

    // Read the file into the string stream
    buffer << fileStream.rdbuf();

    // Close the file stream (optional, as destructor will do this)
    fileStream.close();

    // Return the contents of the string stream as a string
    return buffer.str();
}


VSource::VSource(std::string path) {

    m_Source = readFileAsString(path);

}

std::string VSource::GetCode() {

    return m_Source;

}

int VSource::GetNext() {

    if (m_Index >= m_Source.length()) {
        return -1;
    }

    return m_Source[m_Index++];

}

void VSource::Back() {

    m_Index--;

}
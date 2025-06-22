// ModelException.h
#pragma once
#include "CException.h" // Make sure this path is correct
#include <string>
#include <sstream> // Needed for the 'what' implementation

class ModelException : public CException
{
public:
    ModelException(int line, const char* file, std::string note) noexcept
        : CException(line, file), note(std::move(note))
    {
    }

    const char* what() const noexcept override
    {
        std::ostringstream oss;
        oss << CException::what() << std::endl
            << "[Note] " << GetNote();
        whatBuffer = oss.str(); // Assumes CException has a mutable 'whatBuffer'
        return whatBuffer.c_str();
    }

    const char* GetType() const noexcept override
    {
        return "Model Exception";
    }

    const std::string& GetNote() const noexcept
    {
        return note;
    }

private:
    std::string note;
};
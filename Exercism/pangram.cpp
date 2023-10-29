#include <string>
#include <cstring>
#include <unordered_set>

bool findCharInStringCaseIndependent(const std::string &inputString, char charToFind)
{
    for (char charToCheck : inputString)
    {
        if (tolower(charToCheck) == charToFind)
        {
            return true;
        }
    }
    return false;
}

bool isPangram(const std::string &inputString)
{
    if (inputString.empty())
    {
        return false;
    }
    
    const static std::string alphabet{"abcdefghijklmnopqrstuvwxyz"};

    bool isFailed{false};
    
    for (char charInAlphabet : alphabet)
    {
        if (!findCharInStringCaseIndependent(inputString, charInAlphabet))
        {
            isFailed = true;
            break;
        }
    }
    
    return !isFailed;
}
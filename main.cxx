#include "word_counter.hpp"

auto input()
{
    std::cout << ">> ";
    std::string tmp;
    std::getline(std::cin, tmp);
    return tmp;
}

auto validate(std::string_view data)
{
    if (std::cin && !challenge::isASCIIWord(data))
    {
        std::cout << "Error: Invalid non-ASCII string has been received" << std::endl;
        return false;
    }

    if (std::cin.eof())
    {
        std::cout << "Error: End of the file has been achieved" << std::endl;
        return false;
    }

    return true;
}

void printWords(auto const& wordCounter)
{
    std::cout << "=== Word list:" << std::endl;
    for (auto const& [word, count]: wordCounter.storage())
    {
        std::cout << word << " " << count << std::endl;
    }
}

void lookup(auto const& wordCounter)
{
    std::string lookupWord;
    for (;;)
    {
        std::cout << "Enter a word for lookup" << std::endl;

        std::getline(std::cin, lookupWord);
        if (std::cin.eof())
        {
            break;
        }

        if (auto count = wordCounter.lookup(lookupWord); count > 0)
        {
            std::cout << "SUCCESS: " << lookupWord << " was present " << count
                      << " times in the initial word list" << std::endl;
        }
        else
        {
            std::cout << lookupWord << " was NOT found in the initial word list " << std::endl;
        }
    }

    std::cout << "=== Total words found: " << wordCounter.totalFound() << std::endl;
}

int main()
{
    try
    {
        auto wordCounter = challenge::WordCounter<&input, "end", &validate>();

        wordCounter.readInput();

        printWords(wordCounter);
        lookup(wordCounter);
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}

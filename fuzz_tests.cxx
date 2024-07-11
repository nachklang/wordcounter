#include <chrono>
#include <queue>
#include <thread>

#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"

#include "word_counter.hpp"

std::queue<std::string> testData;

std::string input()
{
    while (!testData.empty())
    {
        auto front = testData.front();
        testData.pop();
        return front;
    }

    return "end";
}

auto validate(std::string_view value)
{
    return challenge::isASCIIWord(value);
}

class WordCounterFuzzTest
{
public:
    WordCounterFuzzTest()
    {
    }

    ~WordCounterFuzzTest()
    {
    }

    void ReadInputFuzzing(std::string const& request)
    {
        try
        {
            testData.push(request);
            m_wordCounter.readInput();
            EXPECT_TRUE(m_wordCounter.lookup(request) > 0);
        }
        catch (std::exception const& ex)
        {
            EXPECT_EQ(ex.what(), challenge::UnexpectedInput().what());
        }
    }

private:
    challenge::WordCounter<&input, "end", &validate> m_wordCounter;
};

FUZZ_TEST_F(WordCounterFuzzTest, ReadInputFuzzing).WithDomains(fuzztest::Arbitrary<std::string>());
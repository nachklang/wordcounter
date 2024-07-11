#include <gtest/gtest.h>

#include "word_counter.hpp"

std::vector<std::string> testData; // NOLINT
auto it{testData.begin()};         // NOLINT

void setupInput(std::vector<std::string> data)
{
    testData = std::move(data);
    it = testData.begin();
}

std::string input()
{
    if (it == testData.end())
    {
        return "end";
    }

    return *it++;
}

auto validate(std::string_view data)
{
    return challenge::isASCIIWord(data);
}

auto createWordCounter()
{
    return challenge::WordCounter<&input, "end", &validate>();
}

TEST(WordCounter, WordsCounterStorageIsEmptyAfterInitialization)
{
    auto wordCounter = createWordCounter();

    EXPECT_TRUE(wordCounter.storage().empty());
}

TEST(WordCounter, WordsCounterTotalFoundIsZeroAfterInitialization)
{
    auto wordCounter = createWordCounter();

    EXPECT_EQ(0, wordCounter.totalFound());
}

TEST(WordCounter, ShouldStopReadInputOnEndReceived)
{
    setupInput({"one"});
    auto wordCounter = createWordCounter();

    EXPECT_NO_THROW(wordCounter.readInput());
}

TEST(WordCounter, ShouldIncrementFrequencyCounterForWordOnDuplicateInInput)
{
    setupInput({"one", "one"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_EQ(2, wordCounter.lookup("one"));
}

TEST(WordCounter, ShouldNotAddDuplicateStringToStorage)
{
    setupInput({"one", "one"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_EQ(1, wordCounter.storage().size());
}

TEST(WordCounter, WordCounterStorageShouldBeCaseSensitive)
{
    setupInput({"test", "TEST"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_EQ(2, wordCounter.storage().size());
}

TEST(WordCounter, WordCounterShouldIncrementTotalFoundCounterOnSuccessfulLookup)
{
    setupInput({"one", "two", "three"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();
    wordCounter.lookup("one");
    EXPECT_EQ(1, wordCounter.totalFound());

    wordCounter.lookup("two");
    EXPECT_EQ(2, wordCounter.totalFound());

    wordCounter.lookup("three");
    EXPECT_EQ(3, wordCounter.totalFound());
}

TEST(WordCounter, WordCounterShouldNotIncrementTotalFoundCounterOnUnsuccessfulLookup)
{
    setupInput({"one"});
    auto wordCounter = createWordCounter();

    wordCounter.lookup("two");
    EXPECT_EQ(0, wordCounter.totalFound());
}

TEST(WordCounter, WordCounterShouldAllowAWordToContainNumbers)
{
    setupInput({"hello24"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_NO_THROW(wordCounter.readInput());
}

TEST(WordCounter, WordCounterShouldAllowDifferentLetterCaseOnInput)
{
    setupInput({"tEsT"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_NO_THROW(wordCounter.readInput());
}

TEST(WordCounter, WordCounterLookupShouldBeCaseSensitive)
{
    setupInput({"test"});
    auto wordCounter = createWordCounter();
    wordCounter.readInput();

    EXPECT_EQ(0, wordCounter.lookup("TEST"));
    EXPECT_EQ(1, wordCounter.lookup("test"));
}

TEST(WordCounter, WordCounterShouldThrowUnexpectedInputOnEmptyString)
{
    setupInput({""});
    auto wordCounter = createWordCounter();

    EXPECT_THROW(wordCounter.readInput(), challenge::UnexpectedInput);
}

TEST(WordCounter, WordCounterShouldThrowUnexpectedInputOnSentence)
{
    setupInput({"entire sentence"});
    auto wordCounter = createWordCounter();

    EXPECT_THROW(wordCounter.readInput(), challenge::UnexpectedInput);
    EXPECT_EQ(0, wordCounter.storage().size());
}

TEST(WordCounter, WordCounterShouldThrowUnexpectedInputOnSpecialCharacters)
{
    setupInput({".,[]%$"});
    auto wordCounter = createWordCounter();

    EXPECT_THROW(wordCounter.readInput(), challenge::UnexpectedInput);
    EXPECT_EQ(0, wordCounter.storage().size());
}

TEST(WordCounter, WordCounterOnInvalidInputShouldKeepItsContent)
{
    setupInput({"one", "entire sentence"});
    auto wordCounter = createWordCounter();

    EXPECT_THROW(wordCounter.readInput(), challenge::UnexpectedInput);
    EXPECT_EQ(1, wordCounter.storage().size());
    EXPECT_EQ(1, wordCounter.lookup("one"));
}
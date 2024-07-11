#include <algorithm>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <regex>
#include <string>
#include <thread>

namespace challenge
{
class UnexpectedInput : public std::exception
{
public:
    char const* what() const noexcept override // NOLINT
    {
        return "Unexpected input has been received";
    }
};

bool isASCIIWord(std::string_view data)
{
    std::regex wordRegex("^\\w+$");
    return std::regex_search(std::string(data), wordRegex);
}

template <size_t N>
struct CompileTimeString
{
    consteval CompileTimeString(char const (&aData)[N]) // NOLINT
    {
        std::copy_n(aData, N, this->data.begin());
    }

    consteval operator std::string_view() const // NOLINT
    {
        return data.data();
    }

    std::array<char, N> data;
};

template <auto Producer, CompileTimeString terminateSequence, auto Validator = 0>
class WordCounter
{
public:
    void readInput()
    {
        std::jthread worker(&WordCounter::workerThread, this);

        while (true)
        {
            std::string tmp = std::invoke(Producer);

            if constexpr (std::is_invocable_v<decltype(Validator), std::string>)
            {
                if (!std::invoke(Validator, tmp))
                {
                    worker.request_stop();
                    throw UnexpectedInput();
                }
            }

            if (tmp == static_cast<std::string_view>(terminateSequence))
            {
                worker.request_stop();
                break;
            }

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this] { return m_data_processed; });
            }

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_data = std::move(tmp);
                m_data_available = true;
                m_data_processed = false;
            }
            m_cv.notify_one();
        }
    }

    int lookup(std::string_view word) const
    {
        auto it = m_storage.find(word);
        if (it != m_storage.end())
        {
            ++m_totalFound;
            return it->second;
        }
        return 0;
    }

    auto const& storage() const
    {
        return m_storage;
    }

    int totalFound() const
    {
        return m_totalFound;
    }

private:
    void workerThread(std::stop_token stopToken)
    {
        std::string tmp;

        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, stopToken, [this] { return m_data_available; });

                if (stopToken.stop_requested() && m_data_processed)
                {
                    break;
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                tmp = std::move(m_data);
                m_data_available = false;
                m_data_processed = true;
            }
            m_cv.notify_one();

            ++m_storage[tmp];
        }
    }

private:
    std::mutex m_mutex;
    std::condition_variable_any m_cv;

    std::string m_data;
    std::map<std::string, int, std::less<>> m_storage;
    bool m_data_available{false};
    bool m_data_processed{true};

    mutable int m_totalFound{0};
};

} // namespace challenge
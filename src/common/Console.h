#pragma once

#include <string>
#include <mutex>
#include <functional>
#include <atomic>

namespace common {
    using input_handler_t = std::function<void (std::string)>;

    class Console {
        private:
            std::atomic<bool> input_running;

            input_handler_t input_handler;
            bool is_running;

            mutable std::mutex print_mutex;

            void input_acceptor();
        public:
            enum LogLevel {
                DEBUG,
                INFO,
                ERROR,
                WARNING
            };

            Console(input_handler_t input_handler = {});
            void log(const LogLevel level, const std::string& message) const;
            void bind_input_handler(input_handler_t input_handler);
            ~Console();
    };
}
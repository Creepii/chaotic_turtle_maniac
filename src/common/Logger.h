#pragma once

#include <string>
#include <mutex>
#include <functional>

namespace common {
    using input_handler_t = std::function<void (std::string)>;

    //TODO: input handling doesnt work
    class Logger {
        private:
            input_handler_t input_handler;
            bool is_running;

            std::mutex print_mutex;

            void input_acceptor();
        public:
            enum LogLevel {
                DEBUG,
                INFO,
                ERROR,
                WARNING
            };

            Logger(input_handler_t input_handler = {});
            void log(const LogLevel level, const std::string& message);
            ~Logger();
    };
}
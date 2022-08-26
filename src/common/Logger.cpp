#include "Logger.h"

#include <thread>
#include <iostream>
#include <chrono>

#include <readline/history.h>
#include <readline/readline.h>

using namespace common;

const std::string LOG_LEVEL_TO_STRING[] = {
    "DEBUG",
    "INFO ",
    "ERROR",
    "WARN "
};

common::Logger::Logger(input_handler_t input_handler) : input_handler(input_handler), is_running(true) {
    rl_bind_key('\t', rl_tab_insert);

    std::thread input_acceptor_thread(&Logger::input_acceptor, this);
    input_acceptor_thread.detach();
}

void common::Logger::log(const LogLevel level, const std::string& message) const {
    std::lock_guard<std::mutex> print_lock(this->print_mutex);

    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    rl_clear_visible_line();
    std::cout << " [" << LOG_LEVEL_TO_STRING[level] << "] " << message << std::endl;
    rl_forced_update_display();
}

void common::Logger::bind_input_handler(input_handler_t input_handler) {
    this->input_handler = input_handler;
}

common::Logger::~Logger() {
    this->is_running = false;
}



//private
void common::Logger::input_acceptor() {
    char* buffer = nullptr;
    while(this->is_running) {
        buffer = readline(">> ");

        if(buffer) {
            rl_set_prompt("");
            rl_replace_line("", 0);
            
            if(this->input_handler) this->input_handler(std::string(buffer));

            free(buffer);
            buffer = nullptr;
        }
    }

    if(buffer) {
        free(buffer);
    }
}
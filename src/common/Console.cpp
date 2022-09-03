#include "Console.h"

#include <thread>
#include <iostream>
#include <chrono>

#include <readline/history.h>
#include <readline/readline.h>

using namespace common;

common::Console common::Console::singleton = common::Console{};

const std::string LOG_LEVEL_TO_STRING[] = {
    "DEBUG",
    "INFO ",
    "ERROR",
    "WARN "
};

// private
common::Console::Console() : input_handler(nullptr), is_running(true), input_running(false) {
    rl_bind_key('\t', rl_tab_insert);

    std::thread input_acceptor_thread(&Console::input_acceptor, this);
    input_acceptor_thread.detach();
}

// public

void common::Console::log(const LogLevel level, const std::string& message) const {
    std::lock_guard<std::mutex> print_lock(this->print_mutex);

    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    rl_clear_visible_line();
    std::cout << " [" << LOG_LEVEL_TO_STRING[level] << "] " << message << std::endl;
    rl_forced_update_display();
}

void common::Console::bind_input_handler(input_handler_t input_handler) {
    this->input_handler = input_handler;
}

common::Console::~Console() {
    this->is_running = false;
    this->log(Console::INFO, "Press any key to close the console!");

    while(this->input_running); //wait for input handler to finish
}



// static
common::Console& common::Console::get_instance() {
    return common::Console::singleton;
}



//private
void common::Console::input_acceptor() {
    this->input_running = true;

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

    this->input_running = false;
}
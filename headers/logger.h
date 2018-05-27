//
// Created by thadumi on 5/23/18.
//

#ifndef SHELL_LOGGER_H
#define SHELL_LOGGER_H
#include "../headers/process.h"

int check_template_path();

int init_logger();
void log_job(job job);
void close_logger();

#endif //SHELL_LOGGER_H

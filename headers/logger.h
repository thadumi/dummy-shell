//
// Created by thadumi on 5/6/18.
//

#ifndef SHELL_LOGGER_H
#define SHELL_LOGGER_H


#include "utiliteas.h"

/**
 * This library define all the methods and structs that have to be used
 * to create a logger system for the executed commands' information
 * based on a template file that can use the following informations
 *
 * $date -> when the process was executed
 * $execution_time -> for how long the process was executed
 * $name -> the nome of the process
 * $args -> list of the arguments with witch the process was executed
 * $out -> the output of the process
 * $mode -> bg/fg
 * TODO: define all template variables
 */

 extern const char *template_file_path;
 extern const char *logger_file_path;

 /**
  * Parse the template file to check if it was write in the correct way.
  * @return
  */
bool check_template();

#endif //SHELL_LOGGER_H

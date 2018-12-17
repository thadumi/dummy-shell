CC = gcc
CC_FLAGS = -O3
SRCS = src/main.c src/source/cli_arguments_parser.c src/source/config.c src/source/logger.c src/source/loop_core.c src/source/process.c src/source/shell.c src/source/util/collections/linkedl.c src/source/util/utiliteas.c
HEADERS = src/headers/cli_arguments_parser.h src/headers/config.h src/headers/logger.h src/headers/loop_core.h src/headers/process.h src/headers/shell.h src/headers/util/collections/linkedl.h src/headers/util/utiliteas.h
BUILD_DIR = bin
EXECUTABLE = ${BUILD_DIR}/shell
LOGS_HOME = ~/shell-logs
CONFIG_HOME = ~/.os-shell

all: build

install build:
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}; \
	${CC} ${CC_FLAGS} $(addprefix ../, ${SRCS}) -c
	${CC} ${BUILD_DIR}/*.o ${HEADERS} -o ${EXECUTABLE}

	mkdir -p ${LOGS_HOME}
	touch ${LOGS_HOME}/console.log
	touch ${LOGS_HOME}/log.template
	touch ${LOGS_HOME}/error.log
	echo "# This file is used to undestand who to create the logger file." >> ${LOGS_HOME}/log.template
	echo "# The logger will create the file first writing the general information about" >> ${LOGS_HOME}/log.template
	echo "# the job that was executed, i.e" >> ${LOGS_HOME}/log.template
	echo "#" >> ${LOGS_HOME}/log.template
	echo "# New job executed at \$$job_start_time" >> ${LOGS_HOME}/log.template
	echo "# with command: \$$job_args" >> ${LOGS_HOME}/log.template
	echo "# with gpid: \$$job_pid" >> ${LOGS_HOME}/log.template
	echo "# with status. \$$job_status" >> ${LOGS_HOME}/log.template
	echo "# with execution mode: \$$job_exec_mod" >> ${LOGS_HOME}/log.template
	echo "# with the following processes:" >> ${LOGS_HOME}/log.template
	echo "#   ... and here the logger will use this file to prompt all the information" >> ${LOGS_HOME}/log.template
	echo "#       about every process that composed the job" >> ${LOGS_HOME}/log.template
	echo "#" >> ${LOGS_HOME}/log.template
	echo "name: \$$name" >> ${LOGS_HOME}/log.template
	echo "########### argomenti del comando" >> ${LOGS_HOME}/log.template
	echo "arguments: \$$args" >> ${LOGS_HOME}/log.template
	echo "######### numero di argomenti" >> ${LOGS_HOME}/log.template
	echo "arguments length: \$$argsnr" >> ${LOGS_HOME}/log.template
	echo "pid: \$$pid " >> ${LOGS_HOME}/log.template
	echo "############# true if was in pipe" >> ${LOGS_HOME}/log.template
	echo "executed in pipe: \$$wipipe" >> ${LOGS_HOME}/log.template
	echo "started at time: \$$start " >> ${LOGS_HOME}/log.template
	#echo "ended at time: \$$end " >> ${LOGS_HOME}/log.template
	echo "exit status: \$$status" >> ${LOGS_HOME}/log.template
	echo "">> ${LOGS_HOME}/log.template
	echo "#end" >> ${LOGS_HOME}/log.template
	mkdir -p ${CONFIG_HOME}
	touch ${CONFIG_HOME}/.config
	echo "welcome_message=Benvenuto nella shell" >> ${CONFIG_HOME}/.config
	echo "prompt_msg=\$$host \$$full_path >" >> ${CONFIG_HOME}/.config
	echo "input_file=$$HOME/shell-logs/log.template" >> ${CONFIG_HOME}/.config
	echo "output_file=$$HOME/shell-logs/console.log" >> ${CONFIG_HOME}/.config
	echo "output_file_write_mode=w" >> ${CONFIG_HOME}/.config
	echo "error_file=$$HOME/shell-logs/error.log" >> ${CONFIG_HOME}/.config

run:
	${EXECUTABLE}

clean:
	rm -rvf ${BUILD_DIR}
	rm -rvf ${LOGS_HOME}
	rm -rvf ${CONFIG_HOME}


.PHONY: all build clean run install

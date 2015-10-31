# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains colors escape sequences for colored printing in terminal.

# Terminal colors (0 -- reset, 1 -- bold, 31 -- red, 32 -- green, 34 -- blue).
export COLOR_RESET	= \033[0m
export COLOR_RUN	= \033[34m
export COLOR_PASS	= \033[32m
export COLOR_FAIL	= \033[31m

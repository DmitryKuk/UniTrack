# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains installation configuration of the project.


# ---===        Project global properties         ===---

# Project name
PROJECT_NAME		= UniTrack

# Lowercase version of project name
PROJECT_NAME_LOW	= $(shell echo '$(PROJECT_NAME)' | tr '[[:upper:]]' '[[:lower:]]')

# Prefix `ut_' will cause module `server' dynamic library `libut_server.so'
PROJECT_LIB_PREFIX	= ut_

# ---===     End of project global properties     ===---


# ---===     Project installation properties      ===---

# Installation directories
# Executables
PREFIX_BIN			= /usr/bin

# Shared libraries
PREFIX_LIB			= /usr/lib

# Third-party installation prefix (see $(PREFIX_THIRDPARTY)/{include,libs} directories)
PREFIX_THIRDPARTY	= /opt/$(PROJECT_NAME_LOW)

# Config
PREFIX_CONFIG		= /etc

# WWW data
PREFIX_WWW			= /var

# ---===  End of project installation properties  ===---


export PROJECT_NAME
export PROJECT_NAME_LOW
export PROJECT_LIB_PREFIX

export PREFIX_BIN
export PREFIX_LIB

export PREFIX_THIRDPARTY


export PREFIX_CONFIG
export PREFIX_WWW

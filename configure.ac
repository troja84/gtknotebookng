#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtknotebookng], 0.1)
AM_INIT_AUTOMAKE(gtknotebookng, 0.1)

AC_PROG_CC

CXX=no
F77=no

AC_PROG_LIBTOOL

PKG_CHECK_MODULES(GTKNOTEBOOKNG, gtk+-2.0)

AC_CONFIG_FILES(
  Makefile
  src/Makefile
  tests/Makefile
)

AC_OUTPUT

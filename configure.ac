#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtknotebookng], 0.1)
AM_INIT_AUTOMAKE (gtknotebookng, 0.1)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

AC_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(GTKNOTEBOOKNG, gtk+-2.0)

AC_SUBST(GTKNOTEBOOKNG_CFLAGS)
AC_SUBST(GTKNOTEBOOKNG_LIBS)

AC_CONFIG_FILES(
  Makefile
  src/Makefile
  tests/Makefile
)

AC_OUTPUT

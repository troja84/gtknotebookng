testprogs=test-notebookng

noinst_PROGRAMS=$(testprogs)

ACLOCAL_AMFLAGS = -I m4

INCLUDES=-g -Wall -O0 -I. -I$(top_srcdir)/src -I$(top_builddir)/src

AM_CFLAGS = ${GTKNOTEBOOKNG_CFLAGS} -I$(top_srcdir)/src

DEPS=$(top_srcdir)/src/libgtknotebookng.la

test_notebookng_SOURCES=\
						test-notebookng.c
test_notebookng_DEPENDENCIES=${DEPS}
test_notebookng_LDADD=${GTKNOTEBOOKNG_LIBS} ${DEPS}

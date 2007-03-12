bin_PROGRAMS = aliens
aliens_SOURCES = aliens.cpp

# set the include path found by configure
AM_CPPFLAGS =  $(LIBSDL_CFLAGS) $(all_includes)

# the library search path.
aliens_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
aliens_LDADD = $(LIBSDL_LIBS)

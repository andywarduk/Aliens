AC_INIT([aliens],[0.1])

AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE

AC_LANG([C++])
AC_PROG_CXX
LT_INIT

dnl KDE_FIND_PATH(programm-name, variable-name, list of directories,
dnl     if-not-found, test-parameter)
AC_DEFUN([KDE_FIND_PATH],
[
   AC_MSG_CHECKING([for $1])
   if test -n "$$2"; then
        kde_cv_path="$$2";
   else
        kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

        AC_CACHE_VAL(kde_cv_path_$kde_cache,
        [
        kde_cv_path="NONE"
        dirs="$3"
        kde_save_IFS=$IFS
        IFS=':'
        for dir in $PATH; do
          dirs="$dirs $dir"
        done
        IFS=$kde_save_IFS

        for dir in $dirs; do
          if test -x "$dir/$1"; then
            if test -n "$5"
            then
              evalstr="$dir/$1 $5 2>&1 "
              if eval $evalstr; then
                kde_cv_path="$dir/$1"
                break
              fi
            else
                kde_cv_path="$dir/$1"
                break
            fi
          fi
        done

        eval "kde_cv_path_$kde_cache=$kde_cv_path"

        ])

      eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

   fi

   if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
      AC_MSG_RESULT(not found)
      $4
   else
      AC_MSG_RESULT($kde_cv_path)
      $2=$kde_cv_path

   fi
])

KDE_FIND_PATH(sdl-config, LIBSDL_CONFIG, [${prefix}/bin ${exec_prefix}/bin /usr/local/bin /opt/local/bin], [
  AC_MSG_WARN([Could not find libsdl anywhere, check http://www.sdl.org])
])

if test -n "$LIBSDL_CONFIG"; then
     LIBSDL_LIBS="`$LIBSDL_CONFIG --libs`"
     LIBSDL_RPATH=
     for args in $LIBSDL_LIBS; do
          case $args in
            -L*)
               LIBSDL_RPATH="$LIBSDL_RPATH $args"
               ;;
          esac
     done
     LIBSDL_RPATH=`echo $LIBSDL_RPATH | sed -e "s/-L/-R/g"`
     LIBSDL_CFLAGS="`$LIBSDL_CONFIG --cflags`"

     AC_DEFINE_UNQUOTED(HAVE_LIBSDL, 1, [Defines if your system has the LIBSDL library])
fi


AC_SUBST(LIBSDL_LIBS)
AC_SUBST(LIBSDL_CFLAGS)
AC_SUBST(LIBSDL_RPATH)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

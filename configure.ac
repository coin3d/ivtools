# *******************************************************************
# ** Misc init/setup stuff.
# *******************************************************************

#  This can be any file in the package.
AC_INIT(ivcat/ivcat.cpp)
AC_CONFIG_AUX_DIR(cfg)

# **************************************************************************
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it.

SIM_AC_SETUP_MSVCPP_IFELSE

# *******************************************************************

AM_INIT_AUTOMAKE(IvTools, 0.9)

#  Files to cram results from the configure tests into.
AM_CONFIG_HEADER(config.h)

#  Turn off default maintainer make-rules -- use ./bootstrap instead.
AM_MAINTAINER_MODE

# *******************************************************************
#  Locate C++ compiler and set C++ as the default language to use
#  in tests. The configure script will automatically terminate if
#  it doesn't find a C++ compiler.
# *******************************************************************
AC_LANG_CPLUSPLUS
AC_PROG_CXX

# *******************************************************************
# ** Check if getopt() is available.
# *******************************************************************

# For getopt() (getopt.h is present on some SGI platforms).
AC_CHECK_HEADERS(unistd.h getopt.h)

AC_MSG_CHECKING([whether getopt() is available])
AC_TRY_LINK([
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if HAVE_GETOPT_H
#include <getopt.h>
#endif /* HAVE_GETOPT_H */
],
  [(void)getopt(0, 0L, 0L);],
  [AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_GETOPT],, [Define if getopt() function is available])],
  [AC_MSG_RESULT([no])])

# *******************************************************************
# ** Check if isatty() is available.
# *******************************************************************

AC_MSG_CHECKING([whether isatty() is available])
AC_TRY_LINK([
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
],
  [(void)isatty(0);],
  [AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_ISATTY],, [Define if isatty() function is available])],
  [AC_MSG_RESULT([no])])

# **************************************************************************
# Search and set up necessary stuff to use the Inventor implementation.

if test x"${with_inventor+set}" = x"set" &&
   test x"${with_inventor}" != x"no"; then

  # If we want to compile and link against InventorXt and Inventor, we
  # need to check for a shitload of libraries.

  SIM_AC_CHECK_MATHLIB
  SIM_AC_CHECK_DL
  SIM_AC_CHECK_X11
  
  if test x"$sim_ac_x11_avail" = xyes; then
    SIM_AC_CHECK_X11SHMEM
    SIM_AC_CHECK_X11MU
    SIM_AC_CHECK_X11XID
    SIM_AC_CHECK_X11_XP
    SIM_AC_CHECK_X_INTRINSIC
  fi
  
# FIXME: enabling this causes the configure file to go corrupt for some
# strange reason. 20030813 mortene.
#  SIM_AC_CHECK_OPENGL([], SIM_AC_ERROR([no-opengl]))
  echo "Inventor support defunct. Please fix. 20030813 mortene."
  exit 1

  SIM_AC_CHECK_GLU([], [AC_MSG_ERROR([needs the GLU library functions])])
  SIM_AC_HAVE_INVENTOR_IFELSE(
    [CPPFLAGS="$sim_ac_inventor_cppflags $CPPFLAGS"
     LDFLAGS="$sim_ac_inventor_ldflags $LDFLAGS"
     LIBS="$sim_ac_inventor_libs $LIBS"],
    [AC_MSG_ERROR([couldn't compile and link against Open Inventor])])


else

  SIM_AC_HAVE_COIN_IFELSE([
    CPPFLAGS="$sim_ac_coin_cppflags $CPPFLAGS"
    LDFLAGS="$sim_ac_coin_ldflags $LDFLAGS"
    LIBS="$sim_ac_coin_libs $LIBS"],
    [AC_MSG_ERROR([cannot compile and link with Coin library])])

fi

# *******************************************************************
# ** Generate output files.
# *******************************************************************

AC_OUTPUT([
        Makefile
        ivNodeKitStructure/Makefile
        ivcat/Makefile
        ivfix/Makefile
        ivhierarchy/Makefile
        ivperf/Makefile
        renderos/Makefile
])

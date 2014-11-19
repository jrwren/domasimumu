AC_PREREQ([2.69])
AC_INIT([domasimumu], [0.0.0], [jrwren@xmtp.net])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX

AC_CHECK_HEADERS([curlpp/cURLpp.hpp curlpp/Options.hpp boost/program_options.hpp])

AC_SEARCH_LIBS([curl], [curlpp], [boost_program_options])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT

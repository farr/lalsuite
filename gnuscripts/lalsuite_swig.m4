dnl SWIG configuration
dnl Author: Karl Wette, 2011

dnl SWIG setup and configuration
AC_DEFUN([LALSUITE_WITH_SWIG],[

  dnl minimum required SWIG version
  SWIG_MIN_VERSION=1.3.40

  dnl save and clear CPPFLAGS and LIBS
  swig_CPPFLAGS=${CPPFLAGS}
  swig_LIBS=${LIBS}
  CPPFLAGS=
  LIBS=

  dnl check for sed
  AC_PROG_SED

  dnl ./configure command line option
  AC_ARG_WITH(
    [swig],
    AC_HELP_STRING(
      [--with-swig=LANG...],
      [generate SWIG wrappings for scripting languages LANG]
    ),
    [swig_langs="${with_swig}"],   dnl --with-swig=...
    [swig_langs=no]                dnl --without-swig / no option
  )

  dnl replace any delimiting commas with spaces
  swig_langs=[`echo ${swig_langs} | ${SED} 's|,| |g'`]

  dnl are we are binding LAL itself, or one of the other LAL libraries?
  swig_islal="test x${PACKAGE_NAME} = xlal"

  dnl are we (not) in debugging mode?
  swiglal_ndebug="test x${enable_debug} = xno"

  dnl common SWIG interface headers (with LAL only)
  AS_IF([${swig_islal}],[
    SWIG_HEADERS=
    AC_SUBST(SWIG_HEADERS)
  ])

  dnl message stating which SWIG wrappings will be built
  SWIG_WRAPPINGS_MSG="SWIG wrappings:"

  dnl iterate over SWIG target scripting languages
  swig_any_lang_found=
  for swig_lang in ${swig_langs}; do
    swig_lang_found=

    dnl configure this language, or fail if language was not found
    LALSUITE_SWIG_LANGUAGES
    AS_IF([test "x${swig_lang_found}" != xyes && test "x${swig_lang}" != xno],[
      AC_MSG_ERROR([no SWIG support for LANG='${swig_lang}'])
    ])

  done

  dnl check if any language was found
  AM_CONDITIONAL(SWIG_BUILD,[test x${swig_any_lang_found} = xyes])
  AM_COND_IF(SWIG_BUILD,[

    dnl common SWIG language build makefile
    SWIG_COMMON_MK="${srcdir}/../lal/swig/swig-common.mk"
    AC_SUBST_FILE(SWIG_COMMON_MK)

    dnl SWIG makefile for generating header lists
    SWIG_HEADER_MK="${srcdir}/../lal/swig/swig-header.mk"
    AC_SUBST_FILE(SWIG_HEADER_MK)

    dnl symbols to define when generating SWIG wrapping code
    SWIG_SWIG_DEFINES=
    AC_SUBST(SWIG_SWIG_DEFINES)

    dnl symbols to define when compiling SWIG wrapping code
    SWIG_CXX_DEFINES=
    AC_SUBST(SWIG_CXX_DEFINES)

    dnl are we are binding LAL itself, or one of the other LAL libraries?
    AS_IF([${swig_islal}],[
      SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} SWIGLAL_IS_LAL"
    ])

    dnl are we (not) in debugging mode?
    AS_IF([${swiglal_ndebug}],[
      SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} NDEBUG"
      SWIG_CXX_DEFINES="${SWIG_CXX_DEFINES} NDEBUG"
    ])

    dnl get correct C99 integer sizes on 32/64-bit systems
    AC_CHECK_HEADERS([stdint.h],[],[
      AC_MSG_ERROR([could not find 'stdint.h'])
    ])
    AC_MSG_CHECKING([value of __WORDSIZE])
    AC_LANG_PUSH([C])
    AC_COMPILE_IFELSE(
      AC_LANG_BOOL_COMPILE_TRY([AC_INCLUDES_DEFAULT],[__WORDSIZE == 32]),
      [wordsize=32],
      [
        AC_COMPILE_IFELSE(
          AC_LANG_BOOL_COMPILE_TRY([AC_INCLUDES_DEFAULT],[__WORDSIZE == 64]),
          [wordsize=64],
          [
            AC_MSG_ERROR([could not determine value of __WORDSIZE])
          ]
        )
      ]
    )
    AC_LANG_POP([C])
    AC_MSG_RESULT([${wordsize}])
    SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} SWIGWORDSIZE${wordsize}"

    dnl ensure that all LAL library modules share type information
    SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} SWIG_TYPE_TABLE=swiglaltypetable"

    dnl make SWIG use C++ casts
    SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} SWIG_CPLUSPLUS_CAST"

    dnl define C99 constant and limit macros
    SWIG_CXX_DEFINES="${SWIG_CXX_DEFINES} __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS"               

    dnl common SWIG interface headers (with LAL only)
    AS_IF([${swig_islal}],[
      SWIG_HEADERS="${SWIG_HEADERS} swiglal-common.i"
    ])

    dnl path SWIG should look in for header files:
    dnl keep any -I options in CPPFLAGS, without the -I prefix
    SWIG_INCLPATH=[`for n in ${swig_CPPFLAGS}; do echo $n | ${SED} 's|^-I||p;d'; done`]
    SWIG_INCLPATH=[`echo ${SWIG_INCLPATH}`]   dnl get rid of newlines
    AC_SUBST(SWIG_INCLPATH)

    dnl path SWIG should look in for libraries:
    dnl keep any -L options in _LIB variables, without the -L prefix
    dnl keep any "lib*.la" files, replace filename with $objdir;
    swig_regex=['s|^-L||p;s|lib[^/][^/]*\.la|'"${objdir}"'|p;d']
    SWIG_LIBPATH="${LAL_LIBS} ${LALSUPPORT_LIBS} ${swig_LIBS}"
    SWIG_LIBPATH=[`for n in ${SWIG_LIBPATH}; do echo $n | ${SED} "${swig_regex}"; done`]
    SWIG_LIBPATH=[`echo ${SWIG_LIBPATH}`]   dnl get rid of newlines
    dnl add pre-install locations for lal, lalsupport, and lal* libraries
    SWIG_LIBPATH="${SWIG_LIBPATH} \$(top_builddir)/lib/${objdir}"
    SWIG_LIBPATH="${SWIG_LIBPATH} \$(top_builddir)/packages/support/src/${objdir}"
    SWIG_LIBPATH="${SWIG_LIBPATH} \$(top_builddir)/src/${objdir}"
    AC_SUBST(SWIG_LIBPATH)

  ],[

    dnl if no SWIG languages were found
    SWIG_WRAPPINGS_MSG="${SWIG_WRAPPINGS_MSG} none"
    SWIG_COMMON_MK="/dev/null"
    SWIG_HEADER_MK="/dev/null"

  ])

  dnl restore CPPFLAGS and LIBS
  CPPFLAGS=${swig_CPPFLAGS}
  LIBS=${swig_LIBS}

])

dnl tell the SWIG wrappings to use some feature
AC_DEFUN([LALSUITE_SWIG_USE],[
  SWIG_SWIG_DEFINES="${SWIG_SWIG_DEFINES} SWIGLAL_USE_$1"
])

dnl SWIG language configuration
AC_DEFUN([LALSUITE_SWIG_LANG],[

  dnl check whether to configure this language
  m4_pushdef([swigcondname],translit([swig_build_$1],[a-z],[A-Z]))
  AM_CONDITIONAL(swigcondname,[test x${swig_lang} = x$1 || test x${swig_lang} = xyes])
  AM_COND_IF(swigcondname,[

    dnl first-time general configuration
    AS_IF([test "x${swig_any_lang_found}" = x],[

      dnl check for swig binary
      AC_PATH_PROGS(SWIG,[swig],[])
      AS_IF([test "x${SWIG}" = x],[
        AC_MSG_ERROR([could not find 'swig' in path])
      ])

      dnl check for swig version
      AC_MSG_CHECKING([for swig version])
      swig_regex=['s|^.*SWIG\s\s*[Vv]ersion\s\s*\([0-9.][0-9.]*\).*$|\1|p;d']
      swig_version=[`${SWIG} -version | ${SED} "${swig_regex}"`]
      AS_IF([test "x${swig_version}" = x],[
        AC_MSG_ERROR([could not determine swig version])
      ])
      AC_MSG_RESULT([${swig_version}])

      dnl check if swig version is newer than required
      AS_VERSION_COMPARE([${SWIG_MIN_VERSION}],[${swig_version}],[],[],[
        AC_MSG_ERROR([require swig version >= ${SWIG_MIN_VERSION}])
      ])

    ])
    swig_any_lang_found=yes

    dnl configure language
    swig_lang_found=yes
    SWIG_WRAPPINGS_MSG="${SWIG_WRAPPINGS_MSG} $1"
    $2

    dnl language-specific SWIG interface header (with LAL only)
    AS_IF([${swig_islal}],[
      SWIG_HEADERS="${SWIG_HEADERS} $1/swiglal-$1.i"
    ])

  ])
  m4_popdef([swigcondname])

])

dnl SWIG languages
AC_DEFUN([LALSUITE_SWIG_LANGUAGES],[
])

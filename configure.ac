# Required initializer
AC_INIT(mod_markdown, 0.1)

# Automake initialization
AM_INIT_AUTOMAKE(dist-bzip2)
AM_CONFIG_HEADER(config.h)

# Add a test for a compiler.
AC_PROG_CC
AM_PROG_LIBTOOL

# Define a macro that is used to parse a --with-apache parameter
# The macro is named "APACHE_DIR"
AC_DEFUN([APACHE_DIR],[

    AC_ARG_WITH(
        apache,
        [  --with-apache[=DIR]     Apache server directory],
        ,
        [with_apache="no"]
    )

    AC_MSG_CHECKING(for Apache directory)

    if test "$with_apache" = "no"; then
        AC_MSG_ERROR( You need to specify the apache directory using --with-apache)
    else
        # make sure that a well known include file exists
        if test -e $with_apache/include/httpd.h; then
            apache_dir=$with_apache
            AC_MSG_RESULT(APACHE found!)
        else
            AC_MSG_ERROR( $with_apache not found.  Check the value you specified with --with-apache)
        fi
    fi

])

# Now call the APACHE_DIR macro that was just specified
APACHE_DIR

# Save the location of apache into the "apache_dir" variable.
# The AC_SUBST macro causes the variable to be saved in config.status
AC_SUBST(apache_dir)

AC_ARG_WITH(discount, [  --with-discount=DIR    discount directory ],,[with_discount="no"])
AC_MSG_CHECKING(for discount directory)
if test "$with_discount" = "no"; then
    AC_MSG_ERROR( You need to specify the discount directory using --with-discount)
else
    if test -e $with_discount/include/mkdio.h -o -e $with_discount/include/*/mkdio.h; then
        discount_dir=$with_discount
        AC_MSG_RESULT(DISCOUNT found!)
    else
        AC_MSG_ERROR( $with_discount not found.  Check the value you specified with --with-discount)
    fi
fi
AC_SUBST(discount_dir)

# Write config.status and the Makefile
AC_OUTPUT(Makefile)

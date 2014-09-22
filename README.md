mod_markdown
============

mod_markdown is Markdown filter module for Apache HTTPD Server.

## Dependencies

* discount

  http://www.pell.portland.or.us/~orc/Code/discount/

In debian:

    # apt-get install libmarkdown2-dev

## Build
    % autoreconf -f -i
    % ./configure --with-apxs=<APXS_PATH> --with-discount=<DISCOUNT_DIR>
    % make
    % make install

Note: `<DISCOUNT_DIR>` is the directory that contains the include directory that contains mkdio.h
Probably you need to specify --with-discount=/usr or --with-discount=/usr/local

## Configration
in httpd.conf:

    LoadModule markdown_module modules/mod_markdown.so
    <Location /markdown>
        AddHandler markdown .md

        # If you want to use stylesheet.
        MarkdownCss style.css
    </Location>

Or:

    Alias /md /home/matt/md
    <Directory /home/matt/md>
        AddHandler markdown .md
        DirectoryIndex index.md
        Order allow,deny
        Allow from all
    </Directory>

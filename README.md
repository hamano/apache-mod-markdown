mod_markdown
============

mod_markdown is Markdown filter module for Apache HTTPD Server.

## Dependencies

* discount

  http://www.pell.portland.or.us/~orc/Code/discount/

## Build
    % ./autogen.sh
    % ./configure --with-apxs=<APXS_PATH> --with-discount=<DISCOUNT_DIR>
    % make
    % make install

## Configration
in httpd.conf:

    LoadModule markdown_module modules/mod_markdown.so
    <Location /markdown>
        AddHandler markdown .md

        # If you want to use stylesheet.
        MarkdownCss style.css
    </Location>

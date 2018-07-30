/*
**  mod_markdown.c -- Apache sample markdown module
**  [Autogenerated via ``apxs -n markdown -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory
**  by running:
**
**    $ apxs -c -i mod_markdown.c
**
**  Then activate it in Apache's httpd.conf file for instance
**  for the URL /markdown in as follows:
**
**    #   httpd.conf
**    LoadModule markdown_module modules/mod_markdown.so
**    <Location /markdown>
**    AddHandler markdown .md
**    </Location>
**
**  Then after restarting Apache via
**
**    $ apachectl restart
**
**  you immediately can request the URL /markdown and watch for the
**  output of this module. This can be achieved for instance via:
**
**    $ lynx -mime_header http://localhost/markdown
**
**  The output should be similar to the following one:
**
**    HTTP/1.1 200 OK
**    Date: Tue, 31 Mar 1998 14:42:22 GMT
**    Server: Apache/1.3.4 (Unix)
**    Connection: close
**    Content-Type: text/html
**
**    The sample page from mod_markdown.c
*/
#include "stdlib.h"
#include "limits.h"

#include "strings.h"

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_log.h"
#include "ap_config.h"

#include "apr_file_io.h"
#include "apr_file_info.h"
#include "apr_strings.h"

#include "mkdio.h"

module AP_MODULE_DECLARE_DATA markdown_module;

typedef enum {
    HTML_5 = 0, XHTML_5, XHTML_1_0_STRICT, XHTML_1_0_TRANSITIONAL,
    XHTML_1_0_FRAMESET, XHTML_1_1, HTML_4_01_STRICT, HTML_4_01_TRANSITIONAL,
    HTML_4_01_FRAMESET, XHTML_BASIC_1_0, XHTML_BASIC_1_1, HTML_UNSET = -1
} doctype_t;

typedef struct {
    const void *data;
    struct list_t *next;
} list_t;

typedef struct {
    doctype_t doctype;
    list_t *css;
    mkd_flag_t mkd_flags;
    const char *header;
    const char *footer;
} markdown_conf;

#define P(s) ap_rputs(s, r)
#ifdef MKD_FENCEDCODE
#define DEFAULT_MKD_FLAGS (MKD_TOC | MKD_AUTOLINK | MKD_FENCEDCODE)
#else
#define DEFAULT_MKD_FLAGS (MKD_TOC | MKD_AUTOLINK )
#endif

/* XML - Wikipedia
 * https://en.wikipedia.org/wiki/XML */
#define XML_DECLARATION "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

/* Document type declaration - Wikipedia
 * https://en.wikipedia.org/wiki/Document_type_declaration */
#define DTD_HTML_5 "<!DOCTYPE html>\n"
/* Both DTDs are the same */
#define DTD_XHTML_5 DTD_HTML_5

/* Probably should use Apache's internal macro `DOCTYPE_(X)HTML_*` instead */
#define DTD_XHTML_1_1 \
"<!DOCTYPE html PUBLIC\n"\
"          \"-//W3C//DTD XHTML 1.1//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
#define DTD_XHTML_1_0_STRICT \
"<!DOCTYPE html\n" \
"          PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
#define DTD_XHTML_1_0_TRANSITIONAL \
"<!DOCTYPE html\n" \
"          PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
#define DTD_XHTML_1_0_FRAMESET \
"<!DOCTYPE html\n" \
"          PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">\n"
#define DTD_HTML_4_01_STRICT \
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"\
"          \"http://www.w3.org/TR/html4/strict.dtd\">\n"
#define DTD_HTML_4_01_TRANSITIONAL \
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"\
"          \"http://www.w3.org/TR/html4/loose.dtd\">\n"
#define DTD_HTML_4_01_FRAMESET \
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\"\n"\
"          \"http://www.w3.org/TR/html4/frameset.dtd\">\n"

#define DTD_XHTML_BASIC_1_0 \
"<!DOCTYPE html PUBLIC\n"\
"          \"-//W3C//DTD XHTML Basic 1.0//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml-basic/xhtml-basic10.dtd\">\n"
#define DTD_XHTML_BASIC_1_1 \
"<!DOCTYPE html PUBLIC\n"\
"          \"-//W3C//DTD XHTML Basic 1.1//EN\"\n"\
"          \"http://www.w3.org/TR/xhtml-basic/xhtml-basic11.dtd\">\n"

/* Root Element, <html> standard-specific attributes
 * Currently we only support the common `xmlns` attribute as the remaining att-
 * rs are locale-specific */
#define ROOT_ELEMENT_HTML_ATTR_XMLNS "xmlns=\"http://www.w3.org/1999/xhtml\""
/* Declaring language in HTML
 * https://www.w3.org/International/questions/qa-html-language-declarations
 * "Use the `lang` attribute for pages served as HTML, and the `xml:lang` attri-
 * bute for pages served as XML. For XHTML 1.x and HTML5 polyglot documents, us-
 * e both together."
#define ROOT_ELEMENT_HTML_ATTR_XML_LANG
#define ROOT_ELEMENT_HTML_ATTR_LANG
*/

#include "mod_markdown.h"
int markdown_output(MMIOT *doc, request_rec *r)
{
    int size;
    char *p;
    markdown_conf *conf;
    int result;

    conf = (markdown_conf *) ap_get_module_config(r->per_dir_config,
                                                  &markdown_module);
    mkd_compile(doc, conf->mkd_flags);

    if (conf->header == NULL && conf->footer == NULL) {
        result = markdown_doc_header(doc, r, conf);
    } else {
        result = markdown_doc_contents(r, "Header", conf->header, COMMENT_START | COMMENT_END);
    }

    if (result != OK) {
        return result;
    }

    if ((size = mkd_document(doc, &p)) != EOF) {
        ap_rwrite(p, size, r);
    }

    /* Insert a new line just to be sure it's clean */
    ap_rputc('\n', r);

    if (conf->header == NULL && conf->footer == NULL) {
        result = markdown_doc_footer(r, conf);
    } else {
        result = markdown_doc_contents(r, "Footer", conf->footer, COMMENT_START | COMMENT_END);
    }

    if (result != OK) {
        return result;
    }
    mkd_cleanup(doc);
    return OK;
}

static int markdown_doc_header(MMIOT *doc, request_rec *r, markdown_conf *conf)
{
    switch(conf->doctype){
    case XHTML_5:
    case XHTML_1_0_STRICT:
    case XHTML_1_0_TRANSITIONAL:
    case XHTML_1_0_FRAMESET:
    case XHTML_1_1:
    case XHTML_BASIC_1_0:
    case XHTML_BASIC_1_1:
        ap_rputs(XML_DECLARATION, r);
        break;
    default:
        /* No XML declaration for HTML doctypes */
        break;
    }

    switch(conf->doctype){
    case HTML_5:
        ap_rputs(DTD_HTML_5, r);
        break;
    case XHTML_5:
        ap_rputs(DTD_XHTML_5, r);
        break;
    case XHTML_1_0_STRICT:
        ap_rputs(DTD_XHTML_1_0_STRICT, r);
        break;
    case XHTML_1_0_TRANSITIONAL:
        ap_rputs(DTD_XHTML_1_0_TRANSITIONAL, r);
        break;
    case XHTML_1_0_FRAMESET:
        ap_rputs(DTD_XHTML_1_0_FRAMESET, r);
        break;
    case XHTML_1_1:
        ap_rputs(DTD_XHTML_1_1, r);
        break;
    case HTML_4_01_STRICT:
        ap_rputs(DTD_HTML_4_01_STRICT, r);
        break;
    case HTML_4_01_TRANSITIONAL:
        ap_rputs(DTD_HTML_4_01_TRANSITIONAL, r);
        break;
    case HTML_4_01_FRAMESET:
        ap_rputs(DTD_HTML_4_01_FRAMESET, r);
        break;
    case XHTML_BASIC_1_0:
        ap_rputs(DTD_XHTML_BASIC_1_0, r);
        break;
    case XHTML_BASIC_1_1:
        ap_rputs(DTD_XHTML_BASIC_1_1, r);
        break;
    default:
        /* Shouldn't be here */
        break;
    }

    switch(conf->doctype){
    case HTML_5:
    case HTML_4_01_STRICT:
    case HTML_4_01_TRANSITIONAL:
    case HTML_4_01_FRAMESET:
        ap_rputs("<html>\n", r);
        break;
    case XHTML_5:
    case XHTML_1_0_STRICT:
    case XHTML_1_0_TRANSITIONAL:
    case XHTML_1_0_FRAMESET:
    case XHTML_1_1:
    case XHTML_BASIC_1_0:
    case XHTML_BASIC_1_1:
        ap_rputs("<html " ROOT_ELEMENT_HTML_ATTR_XMLNS ">\n", r);
        break;
    default:
        /* Shouldn't be here */
        break;
    }

    ap_rputs("<head>\n", r);

    /* <meta> - HTML | MDN
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/meta */
    switch(conf->doctype){
    case HTML_5:
    case XHTML_5:
        ap_rputs("<meta charset=\"utf-8\">\n", r);
        break;
    case HTML_4_01_STRICT:
    case HTML_4_01_TRANSITIONAL:
    case HTML_4_01_FRAMESET:
        ap_rputs("<meta http-equiv=\"Content-Type\" content=\"text/html; "
                 "charset=utf-8\">\n", r);
        break;
    case XHTML_1_0_STRICT:
    case XHTML_1_0_TRANSITIONAL:
    case XHTML_1_0_FRAMESET:
    case XHTML_1_1:
    case XHTML_BASIC_1_0:
    case XHTML_BASIC_1_1:
        /* Shouldn't needed as XML declaration already specifies Content-Type */
        break;
    default:
        /* Shouldn't be here */
        break;
    }

    if (conf->css) {
      ap_rputs("<meta http-equiv=\"Content-Style-Type\""
               " content=\"text/css\" />\n", r);
      list_t *css = conf->css;
      do {
        ap_rprintf(r,
          "<link rel=\"stylesheet\" href=\"%s\""
          " type=\"text/css\" />\n",
          (char *)css->data);
        css = (list_t *)css->next;
      } while (css);
    }

    char *title = mkd_doc_title(doc);
    if (title) {
        ap_rprintf(r, "<title>%s</title>\n", title);
    } else {
        ap_rprintf(r, "<title></title>\n");
    }

    ap_rputs("</head>\n", r);
    ap_rputs("<body>\n", r);

    if (title) {
        ap_rprintf(r, "<h1 class=\"title\">%s</h1>\n", title);
    }

   return OK;
}

static int markdown_doc_footer(request_rec *r, markdown_conf *conf)
{
    ap_rputs("</body>\n", r);
    ap_rputs("</html>\n", r);

    return OK;
}

static int markdown_check_file_exists(request_rec *r, server_rec *s, const char *section, const char *filename) {
    apr_finfo_t a_info;
    int rc, exists;

    rc = apr_stat(&a_info, filename, APR_FINFO_MIN, r->pool);
    if (rc == APR_SUCCESS) {
        exists =
        (
            (a_info.filetype != APR_NOFILE)
        && !(a_info.filetype &  APR_DIR)
        );

        if (!exists) {
            if (r) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "apache-mod-markdown: Failed to find %s file: %d - %s ", section, rc, filename);
            } else {
                ap_log_error(APLOG_MARK, APLOG_ERR, 0, s, "apache-mod-markdown: Failed to find %s file: %d - %s ", section, rc, filename);
            }
            rc = HTTP_INTERNAL_SERVER_ERROR;
        } else {
            rc = OK;
        }
    } else {
        if (r) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "apache-mod-markdown: Failed to find %s file: %d - %s ", section, rc, filename);
        } else {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, s, "apache-mod-markdown: Failed to find %s file: %d - %s ", section, rc, filename);
        }
	rc = HTTP_INTERNAL_SERVER_ERROR;
    }

    return rc;
}

static int markdown_doc_contents(request_rec *r, const char *section, const char *filename, enum COMMENT_FLAGS flags)
{
    int rc, exists;

    char buffer[256];
    const char *a_section = apr_pstrdup(r->pool, section);

    apr_size_t  a_size;
    apr_file_t  *a_file;

    //local_file = apr_pstrdup(r->pool, filename);

    /* Figure out if the file we request exists and isn't a directory */
    rc = markdown_check_file_exists(r, NULL, section, filename);
    if (rc == OK) {
        if ((flags & COMMENT_START) == COMMENT_START) {
            ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): header", a_section);
            ap_rprintf(r, "\n\n<!-- Start Of %s -->\n\n", a_section);
            ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): header done", a_section);
        }

        a_size = sizeof(buffer);
        rc = apr_file_open(&a_file, filename, APR_READ, APR_OS_DEFAULT, r->pool);
        if (rc == APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): first read", section);
            while (apr_file_read(a_file, buffer, &a_size) == APR_SUCCESS) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): other read", section);
                ap_rwrite(buffer, a_size, r);
            }

            if ((flags & COMMENT_END) == COMMENT_END) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): footer", section);
                ap_rprintf(r, "\n\n<!-- End Of %s -->\n\n", section);
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): footer done", section);
            }

            apr_file_close(a_file);
            ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "apache-mod-markdown->markdown_doc_contents(%s): file closed", section);
            rc = OK;
        } else {
            rc = HTTP_NOT_FOUND;
        }
    }

    return rc;
}

/*
 * This routine is called after the server processes the configuration
 * files.  At this point the module may review and adjust its configuration
 * settings in relation to one another and report any problems.  On restart,
 * this routine will be called twice, once in the startup process (which
 * exits shortly after this phase) and once in the running server process.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, the
 * server will still call any remaining modules with an handler for this
 * phase.
 */
static int markdown_hook_check_config(apr_pool_t *pconf, apr_pool_t *plog,
                                       apr_pool_t *ptemp, server_rec *s)
{
    markdown_conf *conf;

    conf = (markdown_conf *) ap_get_module_config(s->module_config,
                                                  &markdown_module);

    ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s, "markdown_hook_check_config: makedown_conf found? %s", (conf == NULL ? "false" : "true"));
    if (conf != NULL) {
        if (conf->header != NULL && conf->footer != NULL) {
            if (!markdown_check_file_exists(NULL, s, "Header", conf->header) ||
                !markdown_check_file_exists(NULL, s, "Footer", conf->footer)) {
                return DECLINED;
            }

            if (conf->css) {
                ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s, "CSS specified whilst using Header/Footer file options, will be ignored");
            }

            /*************************************************
             * The following is in the source commented out  *
             * because markdown_hook_handler() does actually *
             * use the DocType option                        *
             *************************************************
            if (conf->doc_type) {
                ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, NULL, "DocType specifid whilst using Header/Footer file options, will be ignored");
            }
            */
        } else if (conf->header != NULL && conf->footer == NULL) {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, s, "Header specified, but footer was not\n");
            return DECLINED;
	} else if (conf->header == NULL && conf->footer != NULL) {
		ap_log_error(APLOG_MARK, APLOG_ERR, 0, s, "Footer specified, but header was not\n");
		return DECLINED;
        }
    }
    return OK;
}

/* The markdown handler */
static int markdown_hook_handler(request_rec *r)
{
    FILE *fp;
    MMIOT *doc;
    markdown_conf *conf;

    conf = (markdown_conf *) ap_get_module_config(r->per_dir_config,
                                                  &markdown_module);

    if (strcmp(r->handler, "markdown")) {
        return DECLINED;
    }

    if (r->header_only) {
        return OK;
    }

    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                  "markdown_handler(): %s", r->filename);

    if (r->args && !strcasecmp(r->args, "raw")) {
        return DECLINED;
    }

    fp = fopen(r->filename, "r");
    if (fp == NULL) {
        switch (errno) {
        case ENOENT:
            return HTTP_NOT_FOUND;
        case EACCES:
            return HTTP_FORBIDDEN;
        default:
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "open error, errno: %d\n", errno);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    switch(conf->doctype){
    case HTML_5:
    case HTML_4_01_STRICT:
    case HTML_4_01_TRANSITIONAL:
    case HTML_4_01_FRAMESET:
        r->content_type = "text/html";
        break;
    case XHTML_5:
    case XHTML_1_0_STRICT:
    case XHTML_1_0_TRANSITIONAL:
    case XHTML_1_0_FRAMESET:
    case XHTML_1_1:
    case XHTML_BASIC_1_0:
    case XHTML_BASIC_1_1:
        r->content_type = "application/xhtml+xml";
        break;
    default:
        /* Shouldn't be here */
        break;
    }

    doc = mkd_in(fp, 0);
    fclose(fp);
    if (doc == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mkd_in() returned NULL\n");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return markdown_output(doc, r);
}



static void *markdown_config_server_create(apr_pool_t *p, server_rec *s)
{
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_server_create(): started with%s server rec\n",
        (s == NULL ? "out" : ""));

    markdown_conf *c = (markdown_conf *) apr_pcalloc(p, sizeof(markdown_conf));

    c->doctype   = HTML_4_01_TRANSITIONAL;
    c->mkd_flags = DEFAULT_MKD_FLAGS;
    c->header    = NULL;
    c->footer    = NULL;
    c->css       = NULL;

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_server_create(): finished with%s server rec\n",
        (s == NULL ? "out" : ""));

    return (void *) c;
}

static void *markdown_config_server_merge(apr_pool_t *p, void *parent, void *add)
{
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_server_merge(): started with%s parent, with%s add\n",
        (parent == NULL ? "out" : ""),
        (add == NULL ? "out" : ""));

    markdown_conf *c    = (markdown_conf *) apr_palloc(p, sizeof(markdown_conf));
    markdown_conf *dir  = (markdown_conf *) add;
    markdown_conf *base = (markdown_conf *) parent;

    c->doctype   = ( dir->doctype   == HTML_UNSET ? base->doctype   : dir->doctype);
    c->mkd_flags = ( dir->mkd_flags == 0          ? base->mkd_flags : dir->mkd_flags);
    c->header    = ( dir->header    == NULL       ? base->header    : dir->header);
    c->footer    = ( dir->footer    == NULL       ? base->footer    : dir->footer);
    c->css       = ( dir->css       == NULL       ? base->css       : dir->css);

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_server_merge(): finished with%s parent, with%s add\n",
        (parent == NULL ? "out" : ""),
        (add == NULL ? "out" : ""));

    return c;
}

static void *markdown_config_per_dir_create(apr_pool_t * p, char *context)
{
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_per_dir_create(): started with%s context\n",
        (context == NULL ? "out" : ""));

    markdown_conf *c = (markdown_conf *) apr_pcalloc(p, sizeof(markdown_conf));

    c->doctype   = HTML_UNSET;
    c->mkd_flags = 0;
    c->header    = NULL;
    c->footer    = NULL;
    c->css       = NULL;

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_per_dir_create(): finished with%s context\n",
        (context == NULL ? "out" : ""));

    return (void *) c;
}

static void *markdown_config_per_dir_merge(apr_pool_t * p, void *parent, void *add)
{
    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_per_dir_merge(): started with%s parent, with%s add\n",
        (parent == NULL ? "out" : ""),
        (add == NULL ? "out" : ""));

    markdown_conf *c    = (markdown_conf *) apr_palloc(p, sizeof(markdown_conf));
    markdown_conf *dir  = (markdown_conf *) add;
    markdown_conf *base = (markdown_conf *) parent;

    c->doctype   = ( dir->doctype   == HTML_UNSET ? base->doctype   : dir->doctype);
    c->mkd_flags = ( dir->mkd_flags == 0          ? base->mkd_flags : dir->mkd_flags);
    c->header    = ( dir->header    == NULL       ? base->header    : dir->header);
    c->footer    = ( dir->footer    == NULL       ? base->footer    : dir->footer);
    c->css       = ( dir->css       == NULL       ? base->css       : dir->css);

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, NULL, "makedown_config_per_dir_merge(): finished with%s parent, with%s add\n",
        (parent == NULL ? "out" : ""),
        (add == NULL ? "out" : ""));

    return c;
}

static const char *set_markdown_doctype(cmd_parms * cmd, void *conf,
                                    const char *arg)
{
    markdown_conf *c = (markdown_conf *) conf;
    if(!strcmp(arg, "HTML_5")){
        c->doctype = HTML_5;
    }else if(!strcmp(arg, "XHTML_5")){
        c->doctype = XHTML_5;
    }else if(!strcmp(arg, "XHTML_1_0_STRICT")){
        c->doctype = XHTML_1_0_STRICT;
    }else if(!strcmp(arg, "XHTML_1_0_TRANSITIONAL")){
        c->doctype = XHTML_1_0_TRANSITIONAL;
    }else if(!strcmp(arg, "XHTML_1_0_FRAMESET")){
        c->doctype = XHTML_1_0_FRAMESET;
    }else if(!strcmp(arg, "XHTML_1_1")){
        c->doctype = XHTML_1_1;
    }else if(!strcmp(arg, "HTML_4_01_STRICT")){
        c->doctype = HTML_4_01_STRICT;
    }else if(!strcmp(arg, "HTML_4_01_TRANSITIONAL")){
        c->doctype = HTML_4_01_TRANSITIONAL;
    }else if(!strcmp(arg, "HTML_4_01_FRAMESET")){
        c->doctype = HTML_4_01_FRAMESET;
    }else if(!strcmp(arg, "XHTML_BASIC_1_0")){
        c->doctype = XHTML_BASIC_1_0;
    }else if(!strcmp(arg, "XHTML_BASIC_1_1")){
        c->doctype = XHTML_BASIC_1_1;
    }else{
        /* Unknown value, set doctype to the least strict default */
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, NULL, "apache-mod-markdown: Doctype \"%s\" "
                     "unknown, setting to HTML 4.01 Transitional.\n", arg);
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, NULL, "apache-mod-markdown: Available options: "
                     "HTML_5, XHTML_5, XHTML_1_0_STRICT, "
                     "XHTML_1_0_TRANSITIONAL, XHTML_1_0_FRAMESET, XHTML_1_1, "
                     "HTML_4_01_STRICT, HTML_4_01_TRANSITIONAL, "
                     "HTML_4_01_FRAMESET, XHTML_BASIC_1_0, XHTML_BASIC_1_1.");
        c->doctype = HTML_4_01_TRANSITIONAL;
    }
    return NULL;
}

static const char *set_markdown_css(cmd_parms * cmd, void *conf,
                                    const char *arg)
{
    markdown_conf *c = (markdown_conf *) conf;
    list_t *item = (list_t *)malloc(sizeof(list_t));
    item->data = arg;
    item->next = NULL;

    list_t *tail;
    if(c->css){
        tail = c->css;
        while(tail->next) tail = (list_t *)tail->next;
        tail->next = (struct list_t *)item;
    }else{
        c->css = item;
    }
    return NULL;
}

static const char *set_markdown_header(cmd_parms * cmd, void *conf,
									   const char *arg)
{
    markdown_conf *c = (markdown_conf *) conf;

    c->header = arg;
    return NULL;
}

static const char *set_markdown_footer(cmd_parms * cmd, void *conf, const char *arg)
{
    markdown_conf *c = (markdown_conf *) conf;
    c->footer = arg;
    return NULL;
}

static const char *set_markdown_flags(cmd_parms * cmd, void *conf, const char *arg)
{
    long int flags;
    markdown_conf *c = (markdown_conf *) conf;

    flags = strtol(arg, NULL, 0);
    if(flags < 0 || flags > UINT_MAX){
        /* Currently mkd_flag_t is an unsigned integer */

        /* Invalid(out of range) flag, setting flag to the
         * current default */
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, NULL, "apache-mod-markdown: Flags \"%#lX\" "
                     "invalid, setting to default value \"%#X\".\n",
                     flags, DEFAULT_MKD_FLAGS);
        c->mkd_flags = DEFAULT_MKD_FLAGS;
    }else{
        c->mkd_flags = flags;
    }
    return NULL;
}

static const command_rec markdown_cmds[] = {
    AP_INIT_TAKE1("MarkdownDoctype", set_markdown_doctype, NULL, OR_ALL,
                  "set Doctype"),
    AP_INIT_TAKE1("MarkdownCSS", set_markdown_css, NULL, OR_ALL,
                  "set CSS"),
    AP_INIT_TAKE1("MarkdownHeaderFile", set_markdown_header, NULL, OR_ALL,
                  "set Header FILE"),
    AP_INIT_TAKE1("MarkdownFooterFile", set_markdown_footer, NULL, OR_ALL,
                  "set Footer FILE"),
    AP_INIT_TAKE1("MarkdownFlags", set_markdown_flags, NULL, OR_ALL,
                  "set Discount flags"),
    {NULL}
};

static void markdown_register_hooks(apr_pool_t * p)
{
    ap_hook_check_config(markdown_hook_check_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(markdown_hook_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA markdown_module = {
    STANDARD20_MODULE_STUFF,
    markdown_config_per_dir_create,            /* create per-dir    config structures */
    markdown_config_per_dir_merge,             /* merge  per-dir    config structures */
    markdown_config_server_create,             /* create per-server config structures */
    markdown_config_server_merge,              /* merge  per-server config structures */
    markdown_cmds,                             /* table of config file commands       */
    markdown_register_hooks                    /* register hooks                      */
};

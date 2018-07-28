#ifndef MOD_MARKDOWN_H
#define MOD_MARKDOWN_H

enum COMMENT_FLAGS {
    COMMENT_START = 0x1,
    COMMENT_END   = 0x2
};

static int markdown_doc_header(MMIOT *doc, request_rec *r, markdown_conf *conf);
static int markdown_doc_footer(request_rec *r, markdown_conf *conf);
static int markdown_doc_contents(request_rec *r, const char *section, const char *filename, enum COMMENT_FLAGS flags);
static void *markdown_config(apr_pool_t * p, char *dummy);
static void markdown_register_hooks(apr_pool_t * p);

#endif

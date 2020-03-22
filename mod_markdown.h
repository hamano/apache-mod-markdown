#ifndef MOD_MARKDOWN_H
#define MOD_MARKDOWN_H

enum COMMENT_FLAGS {
    COMMENT_START = 0x1,
    COMMENT_END   = 0x2
};

int markdown_output(MMIOT *doc, request_rec *r, markdown_conf *conf);
static int markdown_doc_header(MMIOT *doc, request_rec *r, markdown_conf *conf);
static int markdown_doc_footer(request_rec *r, markdown_conf *conf);
static int markdown_handler(request_rec *r);
static int markdown_check_file_exists(request_rec *r, server_rec *s, const char *section, const char *filename);
static int markdown_doc_contents(request_rec *r, const char *section, const char *filename, enum COMMENT_FLAGS flags);
static void markdown_register_hooks(apr_pool_t * p);
static void *markdown_config(apr_pool_t * p, char *dummy);
static void *markdown_config_server_create(apr_pool_t *p, server_rec *s);
static void *markdown_config_server_merge(apr_pool_t *p, void *BASE, void *ADD);
static void *markdown_config_per_dir_create(apr_pool_t * p, char *context);
static void *markdown_config_per_dir_merge(apr_pool_t * p, void *BASE, void *ADD);

#endif


#ifndef MOD_MARKDOWN_H
#define MOD_MARKDOWN_H

enum COMMENT_FLAGS {
    COMMENT_START = 0x1,
    COMMENT_END   = 0x2
};

int markdown_output(MMIOT *doc, request_rec *r, markdown_conf *conf);

#endif

#ifndef _NGX_BASE64_H_
#define _NGX_BASE64_H_

#include <sys/types.h>

typedef struct {
    size_t      len;
    u_char     *data;
} ngx_str_t;


extern void ngx_encode_base64(ngx_str_t *dst, ngx_str_t *src);
extern int ngx_decode_base64(ngx_str_t *dst, ngx_str_t *src);

#endif

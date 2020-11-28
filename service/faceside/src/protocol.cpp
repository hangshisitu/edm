#include "protocol.h"
#include <unistd.h>

void sockbuf_destroy(sockbuf_t* psb)
{
    if ( !psb )
        return;

    if (psb->sockfd > 0)
        close(psb->sockfd);

    if (psb->buf)
        delete [] psb->buf;

    delete psb;
    return;
}

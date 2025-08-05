#ifndef UA2F_HANDLER_H
#define UA2F_HANDLER_H

#include "queue.h"
#include "nfqueue.h"

/* Handle a single packet: parse and modify as needed, then send verdict. */
void handle_packet(struct nfqueue_context *nfctx, struct packet *pkt);

/* Worker thread function. The argument is a pointer to nfqueue_context. */
void *worker_thread(void *arg);

#endif /* UA2F_HANDLER_H */

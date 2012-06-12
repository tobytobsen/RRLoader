#ifndef LIBNET_FTP_H_
#define LIBNET_FTP_H_

#include <types.h>

#include <net/queue.h>
#include <net/socket.h>

#include <net/ftp_result.h>

typedef struct ftp_ctx {
	socket_t		socket;
	
} ftp_ctx_t;

#endif /* LIBNET_FTP_H_ */

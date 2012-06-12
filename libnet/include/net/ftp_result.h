#ifndef LIBNET_FTP_RESULT_H_
#define LIBNET_FTP_RESULT_H_

typedef enum ftp_res_type {
	LIBNET_FTP_RES_NONE = 0,

	LIBNET_FTP_RES_LS,
	LIBNET_FTP_RES_CWD,

	LIBNET_FTP_RES_UNDEFINED,
} ftp_res_type_t;

typedef struct ftp_res {
	ftp_res_type_t	type;
	void *data;
} ftp_res_t;

#endif /* LIBNET_FTP_RESULT_H_ */

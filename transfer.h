#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#define SEND_SIZE_BYTES			1024
#define RECV_BUF_SIZE_BYTES		2048

int tf_send(int socket_fd, const void *buf, size_t size_bytes);
int tf_recv(int socket_fd, void **buf, size_t *size_bytes, int *term);

// TODO: get rid of offset argument, let user seek before calling
int tf_send_file(int socket_fd, FILE *fp, int offset, int size_bytes);
int tf_recv_file(int socket_fd, FILE *fp, int *term);

#endif
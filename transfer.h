#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#define MAX_MESSAGE_SIZE	1024

int tf_send(int socket_fd, const void *buf, size_t size_bytes);
int tf_recv(int socket_fd, void **buf, size_t *size_bytes, int *term);

int tf_send_file(int socket_fd, FILE *fp, int offset, int size_bytes);
int tf_recv_file(int socket_fd, FILE *fp, int offset);

#endif
#ifndef _TRANSFER_H_
#define _TRANSFER_H_

int tf_send(int socket_fd, const void *buf, size_t size_bytes);
int tf_recv(int socket_fd, void **buf, size_t *size_bytes, int *term);

int tf_send_file(int socket_fd, char *file_name);

#endif
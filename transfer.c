#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

#include <transfer.h>

#define MAGIC_CHECK			123456789

typedef enum TF_SIGNAL{
	FAILURE,
	GOT_HEADER,
	GOT_DATA
} TF_SIGNAL;

typedef struct tf_signal {
	int magic;
	TF_SIGNAL sig;
} tf_signal;

typedef struct tf_header {
	int magic;
	int bytes;
} tf_header;

static int wait_for_signal(int socket_fd, TF_SIGNAL desired_sig) {
	tf_signal signal;
	int bytes_received = recv(socket_fd, &signal, sizeof(tf_signal), 0);
	if (bytes_received != sizeof(tf_signal) ||
		signal.magic != MAGIC_CHECK ||
		signal.sig != desired_sig) {
		return 1;
	}

	return 0;
}

static int send_signal(int socket_fd, TF_SIGNAL sig) {
	tf_signal signal = {MAGIC_CHECK, sig};
	if (send(socket_fd, &signal, sizeof(tf_signal), 0) == -1) {
		return 1;
	}

	return 0;
}

static int wait_for_header(int socket_fd, tf_header *header, int *term) {
	*term = 0;
	int bytes_received;

	// Receive header
	if ((bytes_received = recv(socket_fd, header, sizeof(tf_header), 0)) == -1) {
		*term = 1;
		return 1;
	}

	// Verify correct size
	if (bytes_received == 0) {
		*term = 1;
		return 1;
	}

	if (bytes_received != sizeof(tf_header) ||
		header->magic != MAGIC_CHECK) {
		return 1;
	}

	// Send confirmation signal
	if (send_signal(socket_fd, GOT_HEADER)) {
		return 1;
	}

	return 0;
}

static int send_header(int socket_fd, int size_bytes) {
	tf_header header = {MAGIC_CHECK, size_bytes};
	if (send(socket_fd, &header, sizeof(tf_header), 0) == -1) {
		return 1;
	}

	// Wait for confirmation signal
	if (wait_for_signal(socket_fd, GOT_HEADER)) {
		return 1;
	}

	return 0;
}

static int min(int x, int y) {
	int result = (x < y) ? x : y;
	return result;
}

int tf_send(int socket_fd, const void *buf, size_t size_bytes) {

	// Send header
	if (send_header(socket_fd, size_bytes)) {
		return 1;
	}

	// Iteratively send all the data
	int bytes_to_send = size_bytes;
	const char *next = buf;
	while (bytes_to_send > 0) {
		
		// Send a maximum of MAX... bytes
		int bytes_this_round = min(bytes_to_send, MAX_MESSAGE_SIZE);
		send(socket_fd, next, bytes_this_round, 0);

		// Decrement/Increment by number of bytes sent
		bytes_to_send -= bytes_this_round;
		next = next + bytes_this_round;
	}

	// Wait for confirmation signal
	if (wait_for_signal(socket_fd, GOT_DATA)) {
		return 1;
	}

	return 0;
}

int tf_send_file(int socket_fd, FILE *fp, int offset, int size_bytes) {
	
	// Send header
	if (send_header(socket_fd, size_bytes)) {
		return 1;
	}

	// Iteratively send all the data
	if (fseek(fp, 0, SEEK_SET) == -1) {
		return 1;
	}

	int bytes_to_send = size_bytes;
	char next[MAX_MESSAGE_SIZE];
	while (bytes_to_send > 0) {

		memset(next, 0, MAX_MESSAGE_SIZE);

		// Send a maximum of MAX... bytes
		int bytes_this_round = min(bytes_to_send, MAX_MESSAGE_SIZE);
		if (fread(next, bytes_this_round, 1, fp) != 1) {
			return 1;
		}

		if (send(socket_fd, next, bytes_this_round, 0) == -1) {
			return 1;
		}

		bytes_to_send -= bytes_this_round;
	}

	return 0;	
}


int tf_recv(int socket_fd, void **buf, size_t *size_bytes, int *term) {

	tf_header header;

	// Wait for header
	if (wait_for_header(socket_fd, &header, term)) {
		goto exit;
	}

	// Allocate space for data
	*buf = malloc(header.bytes);
	if (*buf == NULL) {
		goto exit;
	}

	// Wait for data
	int total_bytes_received = 0;
	char *next = *buf;
	while(total_bytes_received < header.bytes) {
		int bytes_received = recv(socket_fd, next, header.bytes - total_bytes_received, 0);
		if (bytes_received == 0) {
			*term = 1;
			goto cleanup_buf;
		}

		total_bytes_received += bytes_received;
		next = next + bytes_received;
	}

	if (total_bytes_received != header.bytes) {
		goto cleanup_buf;
	}

	// Send confiration signal
	if (send_signal(socket_fd, GOT_DATA)) {
		goto cleanup_buf;
	}

	return 0;

cleanup_buf:
	free(*buf);
	*buf = NULL;
exit:
	return 1;
}

int tf_recv_file(int socket_fd, FILE *fp, int offset) {

	printf("Receive to file here\n");

	return 0;
}
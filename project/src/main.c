#define _GNU_SOURCE
#include <fcntl.h>
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef enum op_type {
    OP_READ_FILE,
} op_type;

typedef struct io_context {
    op_type type;
    int fd;
    char* buf;
    size_t buf_len;
} io_context;

int main() {
    struct io_uring ring;
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    params.flags |= IORING_SETUP_SQPOLL;
    params.sq_thread_idle = 2000;

    int ret = io_uring_queue_init_params(8, &ring, &params);
    if (ret < 0) {
        fprintf(stderr, "Failed to init io_uring: %s\n", strerror(-ret));
        return 1;
    }

    // 1. Open the file (this is still synchronous — use io_uring_prep_openat
    //    if you want to async this too, but one step at a time)
    int fd = open("assets/public/index.html", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        return 1;
    }

    // 2. Get file size so we know how large a buffer to allocate
    struct stat st;
    fstat(fd, &st);
    size_t file_size = st.st_size;

    // 3. Build context — this is what comes back on the CQE
    io_context* ctx = malloc(sizeof(io_context));
    ctx->type  = OP_READ_FILE;
    ctx->fd    = fd;
    ctx->buf   = malloc(file_size + 1);
    ctx->buf_len = file_size;
    memset(ctx->buf, 0, file_size + 1);

    // 4. Get SQE and prepare the read
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        fprintf(stderr, "SQ full.\n");
        return 1;
    }

    // io_uring_prep_read(sqe, fd, buf, nbytes, file_offset)
    // Use 0 for file_offset to read from the start
    io_uring_prep_read(sqe, ctx->fd, ctx->buf, ctx->buf_len, 0);
    io_uring_sqe_set_data(sqe, ctx); // attach context

    // 5. Submit
    io_uring_submit(&ring);

    // 6. Wait for completion
    struct io_uring_cqe* cqe;
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
        fprintf(stderr, "wait_cqe failed: %s\n", strerror(-ret));
        return 1;
    }

    // 7. Dispatch on type — scales naturally when you add more op types
    io_context* completed_ctx = io_uring_cqe_get_data(cqe);
    switch (completed_ctx->type) {
        case OP_READ_FILE:
            if (cqe->res < 0) {
                fprintf(stderr, "Read failed: %s\n", strerror(-cqe->res));
            } else {
                printf("Read %d bytes:\n%s\n", cqe->res, completed_ctx->buf);
            }
            break;
    }

    io_uring_cqe_seen(&ring, cqe);

    // 8. Cleanup — ctx owns the buffer, so free both together
    close(completed_ctx->fd);
    free(completed_ctx->buf);
    free(completed_ctx);

    io_uring_queue_exit(&ring);
    return 0;
}
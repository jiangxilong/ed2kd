#include "event_callback.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <event2/event.h>
#include "server.h"
#include "client.h"
#include "log.h"

void signal_cb( evutil_socket_t fd, short what, void *ctx )
{
        (void)fd;
        (void)what;
        (void)ctx;
        ED2KD_LOGNFO("caught SIGINT, terminating...");
        event_base_loopexit(g_instance.evbase_login, NULL);
        event_base_loopexit(g_instance.evbase_tcp, NULL);
}

void tcp_read_cb( struct bufferevent *bev, void *ctx )
{
        struct job *job = (struct job *)calloc(1, sizeof *job);
        (void)bev;

        job->type = JOB_SERVER_READ;
        job->clnt = (struct client*)ctx;

        server_add_job(job);
}

void tcp_event_cb( struct bufferevent *bev, short events, void *ctx )
{

        struct job_event *job = (struct job_event*)calloc(1, sizeof *job);
        (void)bev;

        job->hdr.type = JOB_SERVER_EVENT;
        job->hdr.clnt = (struct client*)ctx;
        job->events = events;

        server_add_job((struct job*)job);
}

void tcp_status_notify_cb( evutil_socket_t fd, short events, void *ctx )
{
        struct job *job = (struct job*)calloc(1, sizeof(*job));
        (void)fd;
        (void)events;

        job->type = JOB_SERVER_STATUS_NOTIFY;
        job->clnt = (struct client*)ctx;

        server_add_job(job);
}

void portcheck_read_cb( struct bufferevent *bev, void *ctx )
{
        struct job *job = (struct job *)calloc(1, sizeof(*job));
        (void)bev;

        job->type = JOB_PORTCHECK_READ;
        job->clnt = (struct client*)ctx;

        server_add_job((struct job*)job);
}

void portcheck_timeout_cb( evutil_socket_t fd, short events, void *ctx )
{
        struct job *job = (struct job*)calloc(1, sizeof(*job));
        (void)fd;
        (void)events;

        job->type = JOB_PORTCHECK_TIMEOUT;
        job->clnt = (struct client*)ctx;

        server_add_job(job);
}

void portcheck_event_cb( struct bufferevent *bev, short events, void *ctx )
{
        struct job_event *job = (struct job_event*)calloc(1, sizeof(*job));
        (void)bev;

        job->hdr.type = JOB_PORTCHECK_EVENT;
        job->hdr.clnt = (struct client*)ctx;
        job->events = events;

        server_add_job((struct job*)job);
}

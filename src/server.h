#pragma once

#ifndef SERVER_H
#define SERVER_H

/**
  @file server.h General server configuration variables and routines
*/

#include <stdint.h>
#include "job.h"
#include <pthread.h>
#include <atomic_ops.h>

struct event_base;
struct bufferevent;
struct client;

#define MAX_WELCOMEMSG_LEN		1024
#define MAX_SERVER_NAME_LEN		64
#define MAX_SERVER_DESCR_LEN	        64
#define MAX_SEARCH_FILES                200
#define MAX_UNCOMPRESSED_PACKET_SIZE    300*1024

struct server_config
{
        /* listen ip address */
        char listen_addr[15];
        uint32_t listen_addr_inaddr;
        /* listen port */
        uint16_t listen_port;
        /* listen backlog */
        int listen_backlog;
        /* server hash */
        unsigned char hash[16];
        /* server TCP capabilities flags */
        uint32_t srv_tcp_flags;

        /* welcome message length */
        size_t welcome_msg_len;
        /* welcome message */
        char welcome_msg[MAX_WELCOMEMSG_LEN+1];

        /* server name length */
        size_t server_name_len;
        /* server name */
        char server_name[MAX_SERVER_NAME_LEN+1];

        /* server description length */
        size_t server_descr_len;
        /* server description */
        char server_descr[MAX_SERVER_DESCR_LEN+1];

        /* port check timeout */
        struct timeval portcheck_timeout_tv;

        /* server status sending interval */
        struct timeval status_notify_tv;

        /* allow lowid clients flag */
        unsigned allow_lowid:1;
};

struct server_instance {
        /* event base */
        struct event_base *evbase;
        /* server configuration loaded from file */
        const struct server_config *cfg;
        /* working threads count */
        size_t thread_count;
        /* connected users count */
        volatile AO_t user_count;
        /* shared files count */
        volatile AO_t file_count;
        /* lowid counter */
        volatile AO_t lowid_counter;

        /* termination flag */
        volatile AO_t terminate;
        /* job queue mutex */
        pthread_mutex_t job_mutex;
        /* job access condition */
        pthread_cond_t job_cond;
        /* job queue */
        struct job_queue jqueue;

        /* common timeval for port check timeout */
        const struct timeval *portcheck_timeout_tv;
        /* common server status notify interval */
        const struct timeval *status_notify_tv;
};

extern struct server_instance g_instance;

void server_add_job( struct job *job );

void *server_job_worker( void *ctx );

void server_remove_client_jobs( const struct client *clnt );

#endif // SERVER_H

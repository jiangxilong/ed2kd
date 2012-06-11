#include <stdint.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "client.h"
#include "ed2kd.h"
#include "config.h"
#include "ed2k_proto.h"
#include "version.h"
#include "log.h"

struct e_client *client_new()
{
    struct e_client *client = (struct e_client*)malloc(sizeof(struct e_client));
    memset(client, 0, sizeof(struct e_client));

    return client;
}

void client_free( struct e_client *client )
{
#ifdef DEBUG
	ED2KD_LOGDBG("client removed (%s:%d)", client->dbg.ip_str, client->port);
#endif

    if( client->nick ) free(client->nick);
	if( client->bev_cli ) bufferevent_free(client->bev_cli);
	if( client->bev_srv ) bufferevent_free(client->bev_srv);

    free(client);
}

void send_server_message( struct e_client *client, const char *msg, uint16_t len )
{
    struct packet_server_message data;
	data.proto = PROTO_EDONKEY;
	data.length = sizeof data - sizeof(struct packet_header) + len;
	data.opcode = OP_SERVERMESSAGE;
	data.msg_len = len;

    bufferevent_write(client->bev_srv, &data, sizeof data);
    bufferevent_write(client->bev_srv, msg, len);
}

void send_id_change( struct e_client *client )
{
    struct packet_id_change data;
    data.proto = PROTO_EDONKEY;
    data.length = sizeof data - sizeof(struct packet_header);
    data.opcode = OP_IDCHANGE;
    data.user_id = client->ip;
    data.tcp_flags = ED2KD_SRV_TCP_FLAGS;

    bufferevent_write(client->bev_srv, &data, sizeof data);
}

void send_server_status( struct e_client *client )
{
    struct packet_server_status data;
    data.proto = PROTO_EDONKEY;
    data.length = sizeof data - sizeof(struct packet_header);
    data.opcode = OP_SERVERSTATUS;
    data.user_count = ed2kd()->user_count;
    data.file_count = ed2kd()->file_count;

    bufferevent_write(client->bev_srv, &data, sizeof data);
}

void send_server_ident( struct e_client *client )
{
    //struct evbuffer *outbuf = bufferevent_get_output(client->bev);
}

void send_search_result( struct e_client *client )
{
    //struct evbuffer *outbuf = bufferevent_get_output(client->bev);
}

void send_found_sources( struct e_client *client )
{
    //struct evbuffer *outbuf = bufferevent_get_output(client->bev);
}

void send_reject( struct e_client *client )
{
	static const char data[] = { PROTO_EDONKEY, 1, 0, 0, 0, OP_REJECT };
    bufferevent_write(client->bev_srv, &data, sizeof data);
}

void client_portcheck_finish( struct e_client *client )
{
	bufferevent_free(client->bev_cli);
	client->bev_cli = NULL;
	client->portcheck_finished = 1;

    send_id_change(client);
    send_server_message(client, ed2kd()->welcome_msg, ed2kd()->welcome_msg_len);
}

void client_portcheck_failed( struct e_client *client )
{
#ifdef DEBUG
	ED2KD_LOGDBG("port check failed (%s:%d)", client->dbg.ip_str, client->port);
#endif
	client_free(client);
}

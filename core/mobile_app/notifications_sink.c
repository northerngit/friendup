/*©mit**************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the MIT License, *
* found in the file license_mit.txt.                                           *
*                                                                              *
*****************************************************************************©*/

#include "notifications_sink_websocket.h"
#include "mobile_app.h"
#include <pthread.h>
#include <util/hashmap.h>
#include <system/json/jsmn.h>
#include <system/systembase.h>
#include <system/user/user_mobile_app.h>

extern SystemBase *SLIB;

static Hashmap *_socket_auth_map; //maps websockets to boolean values that are true then the websocket is authenticated
static char *_auth_key;


static void _notifications_sink_init(void);
static void _websocket_remove(struct lws *wsi);
static char* _get_websocket_hash(struct lws *wsi);
static int _process_incoming_request(struct lws *wsi, char *data, size_t len, void *udata );
static int _reply_error(struct lws *wsi, int error_code);
static bool _is_socket_authenticated(struct lws *wsi);
static bool _verify_auth_key(const char *key_to_verify);


/**
 * Initialize Notification Sink
 *
 */
static void _notifications_sink_init( void )
{
	DEBUG("Initializing mobile app module\n");

	_socket_auth_map = HashmapNew();
}

/**
 * 
 * Connection with server callback
 * 
 */

void websocket_notification_conn_callback( struct WebsocketClient *wc, char *msg, int len )
{
	
}

/**
 * Main Websocket notification sink callback
 *
 * @param wsi pointer to Websocket connection
 * @param reason callback reason
 * @param user pointer to user data
 * @param in pointer to message
 * @param len size of provided message
 * @return 0 when everything is ok, otherwise return different value
 */
int websocket_notifications_sink_callback( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	DEBUG("notifications websocket callback, reason %d, len %zu, wsi %p\n", reason, len, wsi);
	
	if( reason == LWS_CALLBACK_PROTOCOL_INIT )
	{
		_notifications_sink_init();
		return 0;
	}

	if( reason == LWS_CALLBACK_CLOSED || reason == LWS_CALLBACK_WS_PEER_INITIATED_CLOSE )
	{
		mobile_app_notif *man = (mobile_app_notif *)user;
		_websocket_remove( wsi );
		
		if( man != NULL )
		{
			if( man->mans_Connection != NULL )
			{
				WebsocketClientDelete( man->mans_Connection );
				man->mans_Connection = NULL;
			}
		}
		
		return 0;
	}

	if( reason != LWS_CALLBACK_RECEIVE )
	{
		DEBUG("Unimplemented callback, reason %d\n", reason);
		return 0;
	}

	if( len == 0 )
	{
		DEBUG("Empty websocket frame (reason %d)\n", reason);
		return 0;
	}

	return _process_incoming_request( wsi, (char*)in, len, user );
}

/**
 * Process incoming request
 *
 * @param wsi pointer to Websocket connection
 * @param data pointer to string where message is stored
 * @param len length of message
 * @return 0 when success, otherwise error number
 */
static int _process_incoming_request( struct lws *wsi, char *data, size_t len, void *udata )
{
	DEBUG("Incoming notification request: <%*s>\n", (unsigned int)len, data);
	mobile_app_notif *man = (mobile_app_notif *)udata;

	jsmn_parser parser;
	jsmn_init( &parser );
	jsmntok_t tokens[16]; //should be enough

	int tokens_found = jsmn_parse( &parser, data, len, tokens, sizeof(tokens)/sizeof(tokens[0]) );

	if( tokens_found < 1 )
	{
		return _reply_error(wsi, 5);
	}

	json_t json = { .string = data, .string_length = len, .token_count = tokens_found, .tokens = tokens };

	char *msg_type_string = json_get_element_string(&json, "t");
	if( msg_type_string == NULL )
	{
		return _reply_error(wsi, 6);
	}

	if( strcmp(msg_type_string, "auth") == 0 )
	{
		char *auth_key = json_get_element_string(&json, "key");
		if( auth_key == NULL )
		{
			return _reply_error(wsi, 7);
		}
		
		if( _verify_auth_key( auth_key ) == false )
		{
			return _reply_error( wsi, 8 );
		}

		//at this point the authentication key is verified and we can add this socket to the trusted list
		char *websocket_hash = _get_websocket_hash( wsi ); //do not free, se HashmapPut comment

		HashmapElement *e = HashmapGet( _socket_auth_map, websocket_hash );
		if( e == NULL )
		{
			bool *auth_flag = FCalloc(1, sizeof(bool));
			*auth_flag = true;
			HashmapPut( _socket_auth_map, websocket_hash, auth_flag );

		}
		else
		{ //this socket exists but the client somehow decided to authenticate again
			*((bool*)e->data) = true;
		}

		char reply[ 128 ];
		strcpy( reply + LWS_PRE, "{ \"t\" : \"auth\", \"status\" : 1}" );
		unsigned int json_message_length = strlen( reply + LWS_PRE );

		lws_write( wsi, (unsigned char*)reply+LWS_PRE, json_message_length, LWS_WRITE_TEXT );
		
		//man->mans_Connection = WebsocketClientNew( SLIB->l_AppleServerHost, SLIB->l_AppleServerPort, websocket_notification_conn_callback );
		return 0;

	}
	else if( _is_socket_authenticated( wsi ) )
	{
		char *username = json_get_element_string( &json, "username" );
		char *channel_id = json_get_element_string( &json, "channel_id" );
		char *title = json_get_element_string( &json, "title" );
		char *message = json_get_element_string( &json, "message" );

		if( username == NULL || channel_id == NULL || title == NULL || message == NULL )
		{
			return _reply_error( wsi, 8 );
		}

		int notification_type = 0;

		if( json_get_element_int( &json, "notification_type", &notification_type) == false )
		{
			return _reply_error( wsi, 9 );
		}

		bool status = mobile_app_notify_user( username, channel_id, title, message, (mobile_notification_type_t)notification_type, NULL/*no extras*/);

		char reply[128];
		sprintf(reply + LWS_PRE, "{ \"t\" : \"notify\", \"status\" : %d}", status);
		unsigned int json_message_length = strlen( reply + LWS_PRE );

		lws_write( wsi, (unsigned char*)reply+LWS_PRE, json_message_length, LWS_WRITE_TEXT );
		return 0;

	}
	else
	{
		return _reply_error(wsi, 20);
	}
	return 0;
}

/**
 * Set websocket notification key
 *
 * @param key pointer to new notification key
 */
void websocket_notifications_set_auth_key( const char *key )
{
	unsigned int len = strlen(key);
	if( len < 10 )
	{ //effectively disable the whole module if the key is too weak or non-existent
		_auth_key = NULL;
		DEBUG("Notifications key not set, the service will be disabled\n");
		return;
	}
	_auth_key = FCalloc(len+1, sizeof(char));
	strcpy(_auth_key, key);
	DEBUG("Notifications key is <%s>\n", _auth_key);
}

/**
 * Reply error code to user
 *
 * @param wsi pointer to Websocket connection
 * @param error_code error code
 * @return -1
 */
static int _reply_error( struct lws *wsi, int error_code )
{
	char response[LWS_PRE+32];
	snprintf(response+LWS_PRE, sizeof(response)-LWS_PRE, "{ \"t\":\"error\", \"status\":%d}", error_code);
	DEBUG("Error response: %s\n", response+LWS_PRE);

	DEBUG("WSI %p\n", wsi);
	lws_write(wsi, (unsigned char*)response+LWS_PRE, strlen(response+LWS_PRE), LWS_WRITE_TEXT);

	_websocket_remove(wsi);

	return -1;
}

/**
 * Check if websocket is authenticated
 *
 * @param wsi pointer to Websocket connection
 * @return true when socket is authenticated, otherwise false
 */
static bool _is_socket_authenticated( struct lws *wsi )
{
	char *websocket_hash = _get_websocket_hash(wsi);
	bool *socket_is_authenticated = (bool*)HashmapGetData(_socket_auth_map, websocket_hash);
	FFree(websocket_hash);
	if( socket_is_authenticated/*the pointer, not the value!*/ != NULL )
	{
		return *socket_is_authenticated;
	}
	return false;
}

/**
 * Verify authentication key
 *
 * @param key_to_verify pointer to string with key
 * @return true when key passed verification, otherwise false
 */
static bool _verify_auth_key( const char *key_to_verify )
{
	//TODO: verify against multiple keys in case many notification sources connect to the core
	if( _auth_key )
	{
		if( strcmp( _auth_key, key_to_verify) == 0 )
		{
			return true;
		}
	}
	return false;
}

/**
 * Remove Websocket connection from global pool
 *
 * @param wsi pointer to Websocket connection
 */
static void _websocket_remove( struct lws *wsi )
{
	char *websocket_hash = _get_websocket_hash(wsi);
	bool *socket_is_authenticated = HashmapGetData(_socket_auth_map, websocket_hash);

	if( socket_is_authenticated/*the pointer, not the value!*/ != NULL )
	{
		HashmapRemove(_socket_auth_map, websocket_hash);
		FFree(socket_is_authenticated);
	}	

	FFree(websocket_hash);
}

/**
 * Get Websocket hash
 *
 * @param wsi pointer to Websocket connection
 * @return pointer to hash in allocated string
 */
static char* _get_websocket_hash( struct lws *wsi )
{
	/*FIXME: this is a dirty workaround for currently used hashmap module. It accepts
	 * only strings as keys, so we'll use the websocket pointer printed out as
	 * string for the key. Eventually there should be a hashmap implementation available
	 * that can use ints (or pointers) as keys!
	 */
	char *hash = FCalloc(16, 1);
	snprintf(hash, 16, "%p", wsi);
	return hash;
}

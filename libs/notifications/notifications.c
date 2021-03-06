/*©lgpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Lesser   *
* General Public License, found in the file license_lgpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

#include <core/types.h>
#include <core/library.h>
#include <core/mobile_app/mobile_app.h>
#include <util/log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <systembase.h>
#include <util/string.h>
#include <util/hashmap.h>

#include "notifications.h"

#define LIB_NAME "notifications.library"
#define LIB_VERSION 1

static NotificationsLibrary_t *_library_handle;

/**
 * Library initialization
 *
 * @param systembase pointer to SystemBase
 */
void *libInit( void *systembase )
{
	FERROR("************* library init, systembase %p", systembase);
	NotificationsLibrary_t* l;

	if( ( _library_handle = calloc( sizeof( NotificationsLibrary_t ), 1 ) ) == NULL )
	{
		return NULL;
	}

	_library_handle->l_Name = LIB_NAME;
	_library_handle->l_Version = LIB_VERSION;
	_library_handle->sb = systembase;
	_library_handle->libClose = dlsym ( _library_handle->handle, "libClose");
	_library_handle->GetVersion = dlsym ( _library_handle->handle, "GetVersion");
	_library_handle->WebRequest = dlsym ( _library_handle->handle, "WebRequestNotification");

	return _library_handle;
}

void libClose( NotificationsLibrary_t *l )
{
	FERROR("************* library close");
}

long GetVersion(void)
{
	return LIB_VERSION;
}

/**
 * WebRequest notification handler
 *
 * @param l pointer to notification.library
 * @param func path passed
 * @param request pointer to Http structure
 * @return response in Http structure
 */
Http* WebRequestNotification(struct Library *l __attribute__((unused)), char* func, Http *request)
{
	INFO("Func is <%s>", func);

	struct TagItem tags[] = {
			{ HTTP_HEADER_CONTENT_TYPE, (FULONG)StringDuplicate( "text/plain" ) },
			{ HTTP_HEADER_CONNECTION, (FULONG)StringDuplicate( "close" ) },
			{TAG_DONE, TAG_DONE}
	};

	if( strcmp( func, "notify" ) == 0 )
	{
		Http *response = HttpNewSimple(HTTP_200_OK, tags);

		HashmapElement *message_element = GetHEReq(request, "message");
		HashmapElement *title_element = GetHEReq(request, "title");
		HashmapElement *session_element = GetHEReq(request, "sessionid");
		HashmapElement *extra_element = GetHEReq(request, "extra");

		if( message_element && title_element && session_element && extra_element )
		{
			User *user = USMGetUserBySessionID(((SystemBase*)(_library_handle->sb))->sl_USM, session_element->data);
			if( user )
			{
				char *message = UrlDecodeToMem( message_element->data );
				char *title = UrlDecodeToMem( title_element->data );
				char *extra = UrlDecodeToMem( extra_element->data );
				char *username = user->u_Name;

				/* Small bug: JavaScript call
				 * new Library('notifications.library').execute("notify?message=your_message_here&extra=something&title=sometitle")
				 * appends a trailing slash to the URL. Of course it is received as a value of the field.
				 * To keep it quick'n'simple for now it is assumed that title comes last
				 * and remove the last printable byte.
				 */
				title[strlen(title)-1] = '\0';

				bool status = mobile_app_notify_user( username, "lib"/*channel id*/, title, message, MN_force_all_devices, extra );

				FFree( message );
				FFree( title );
				FFree( extra );

				HttpAddTextContent( response, "OK" );
				INFO("sending OK");
			}
			else
			{
				HttpAddTextContent( response, "no session" );
				INFO("sending no session");
			}
		}
		else
		{
			HttpAddTextContent( response, "missing data" );
			INFO("sending missing data");
		}
		return response;
	}
	else
	{
		Http* response = HttpNewSimple(HTTP_404_NOT_FOUND, tags);
		HttpAddTextContent( response, "unsupported" );
		return response;
	}
}

/*©agpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Affero   *
* General Public License, found in the file license_agpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

self.addEventListener( 'push', ( event ) => {
	console.log('Received a push message', event);
	let title = data.title;
	let body = data.body;
	let icon = data.icon;
	let tag = 'friendos-tag';
	let data = { some: 'data' };

	event.waitUntil(
		self.registration.showNotification( title, {
			body: body,
			icon: icon,
			tag: tag,
			data: data
		} )
	);
	alert( 'Test' );
} );

self.addEventListener( 'notificationclick', event => {
	try
	{
		event.notification.close();
		alert( 'Here we go.' );
		const data = event.data?.json() ?? {};
		
		event.waitUntil(
			clients.openWindow( data ? data.url : 'https://intranet.friendup.cloud/webclient/index.html' )
		);
	}
	catch( e )
	{
		console.log( 'Error with service worker click: ', e );
	}
} );


/*©agpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Affero   *
* General Public License, found in the file license_agpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

function base64ToBytes( base64 )
{
    const binString = atob( base64 );
    return Uint8Array.from( binString, ( m ) => m.codePointAt( 0 ) );
}

self.addEventListener( 'push', ( event ) => {
	const data = event.data?.json() ?? {};
	const title = data.title;
	const body = data.body;
	const icon = data.icon;
	const tag = 'friendos-tag';
	
	let text = decodeURIComponent( body );
    try
    {
        let dec = new TextDecoder().decode( base64ToBytes( text ) );
        text = dec;
    }
    catch( e2 ){};
	
	event.waitUntil(
		self.registration.showNotification( title, {
			body: text,
			icon: icon,
			tag: tag,
			data: event.data,
			vibrate: [ 300, 100, 400 ]
		} )
	);
} );

self.addEventListener( 'notificationclick', event => {
	event.notification.close();
	event.waitUntil( ( async function( test )
	{
		console.log( 'What is this: ', test, event.data ? ( 'Text: ' + event.data.text() ) : ( 'Body: ' + event.notification.body ) );
		const data = event.data?.json() ?? {};
		
		console.log( 'Notification debug:' );
		for( let a in event )
		{
			console.log( a + ' -> ' + event[ a ] );
		}
		console.log( 'Event debug:' );
		for( let a in event )
		{
			console.log( a + ' -> ' + event[ a ] );
		}
		console.log( 'Data debug: ' );
		for( let a in data )
		{
			console.log( a + ' -> ' + data[ a ] );
		}
		clients.openWindow( data && data.url ? data.url : 'https://intranet.friendup.cloud/webclient/index.html' );
	} )() );
} );


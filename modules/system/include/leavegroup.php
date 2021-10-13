<?php

/*©lgpl*************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the GNU Lesser   *
* General Public License, found in the file license_lgpl.txt.                  *
*                                                                              *
*****************************************************************************©*/

if( isset( $args->args->groupId ) )
{
	$groupId = intval( $args->args->groupId, 10 );
	
	// Make sure you cannot leave if you weren't invited
	$g = new dbIO( 'FUserGroup' );
	if( $g->load( $groupId ) )
	{
		$q = new dbIO( 'FQueuedEvent' );
		$q->TargetGroupID = $groupId;
		$q->TargetUserID = $User->ID;
		if( $q->load() )
		{
			if( FriendCoreQuery( '/system.library/group/removeusers', 
			[
				'id'    => $groupId,
				'users' => $User->ID
			] ) )
			{
				//
			}
			else
			{
				// If FriendCore didn't wanna do this, just do it! ...
				$SqlDatabase->query( 'DELETE FROM FUserToGroup WHERE UserID=\'' . $User->ID . '\' AND UserGroupID=\'' . $groupId . '\'' );
			}
			$SqlDatabase->query( 'DELETE FROM FQueuedEvent WHERE TargetGroupID=\'' . $groupId . '\' AND TargetUserID=\'' . $User->ID . '\'' );
			die( 'ok<!--separate-->' );
		}
	}
}
die( 'fail' );

?>

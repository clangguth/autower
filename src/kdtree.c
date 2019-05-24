/*
ACKNOWLEDGEMENT: This file was taken from
http://www.cs.sunysb.edu/~algorith/implement/weisses/distrib/c-implementation/kdtree.c
and slightly adapted for the purposes of this program. Thanks and credits go to the original implementor.

I assume that the original code, since it is publicly available and no indication of its use is given,
is in the public domain. Whichever license applies to the original code will of course apply to this
(only very modestly modified) file. In other words: This file is explicitly NOT covered by the GPL, and
I do not claim authorship for it.

Christoph Langguth

*/


#include <stdio.h>
#include <stdlib.h>
#include "autower.h"


static KdTree RecursiveInsert( LatLon Item, AirportInfo *payload, KdTree T, int Level ) {
	if( T == NULL )
	{
		T = malloc( sizeof( struct KdNode ) );
//		if( T == NULL )
//			FatalError( "Out of space!!!" );
		T->Left = T->Right = NULL;
		T->Data[ 0 ] = Item[ 0 ];
		T->Data[ 1 ] = Item[ 1 ];
		T->payload = payload;
	}
	else
	if( Item[ Level ] < T->Data[ Level ] )
		T->Left = RecursiveInsert( Item, payload, T->Left, !Level );
	else
		T->Right = RecursiveInsert( Item, payload, T->Right, !Level );
	return T;
}

KdTree treeInsertAirport( LatLon Item, AirportInfo* payload, KdTree T )
{
	return RecursiveInsert( Item, payload, T, 0 );
}
/* END */

/* START: fig12_44.txt */
/* Print items satisfying */
/* Low[ 0 ] <= Item[ 0 ] <= High[ 0 ] and */
/* Low[ 1 ] <= Item[ 1 ] <= High[ 1 ] */

static void RecPrintRange( LatLon Low, LatLon High,
			   KdTree T, int Level)
{
	if( T != NULL )
	{
		if( Low[ 0 ] <= T->Data[ 0 ] &&
						T->Data[ 0 ] <= High[ 0 ] &&
						Low[ 1 ] <= T->Data[ 1 ] &&
						T->Data[ 1 ] <= High[ 1 ] ) {
			treeFindResultCallback(T->payload);
		}

		if( Low[ Level ] <= T->Data[ Level ] )
			RecPrintRange( Low, High, T->Left, !Level );
		if( High[ Level ] >= T->Data[ Level ] )
			RecPrintRange( Low, High, T->Right, !Level );
	}
}

void treeFindAirport( LatLon Low, LatLon High, KdTree T)
{
	RecPrintRange( Low, High, T, 0);
}
/* END */


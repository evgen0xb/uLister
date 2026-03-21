/* Copyright (c) 2001, 2016, Oracle and/or its affiliates. All rights reserved. */
   /*
    |	Remote Filter Access
    |	Include File sccra.h
    |
    |	RRRR     A
    |	R   R   A A 
    |	RRRR   A   A
    |	R R    AAAAA 
    |	R  R   A   A
    |	R   R  A   A
    |	  
    |
    */

#ifndef _SCCRA_H
#define _SCCRA_H

#include <scctype.h>
#include <sccda.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
|  T Y P E D E F S
*/

typedef VTHDOC   VTHREMOTE;
typedef VTLPHDOC VTLPHREMOTE;



/*
|  P R O T O T Y P E S
*/

DA_ENTRYSC DAERR DA_ENTRYMOD RAOpenRemote( VTHDOC hDoc, VTLPHREMOTE phRemote );
DA_ENTRYSC DAERR DA_ENTRYMOD RACloseRemote( VTHREMOTE hRemote );
DA_ENTRYSC DAERR DA_ENTRYMOD RAGetRemoteData( VTHREMOTE hRemote, PIORFAREQUEST  pRequest, VTLPBYTE * ppDataBuffer, VTDWORD * pdwDataSize);

#ifdef __cplusplus
}
#endif

#endif /* SCCRA_H */

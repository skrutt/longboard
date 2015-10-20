/*
 * platform.h
 *
 * Created: 2015-10-20 15:26:09
 *  Author: petter
 */ 

//This is just a glue file

#ifndef PLATFORM_H_
#define PLATFORM_H_

#define LONGBOARD

#ifdef LONGBOARD
	#include "longboard.h"
#else
	#include "other_platform.h"	
#endif


#endif /* PLATFORM_H_ */
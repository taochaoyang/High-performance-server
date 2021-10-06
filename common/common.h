/*************************************************************************
	> File Name: common.h
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Tue Sep 21 11:29:41 2021
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H



#include "config.h"
#include <iostream>
#include <cstdio>
#include "assert.h" 


using namespace std;

// For canceling the color.
#define NONE "\033[m"

// For serious warning.
#define RED "\033[0;32;31m" 

// For healthy logging.
#define GREEN "\033[0;32;32m"

// For warning.
#define YELLOW "\033[1;33m"

// For debug. 
#define BLUE "\033[0;32;34m"

#define CYAN "\033[0;31m"

// Define macro "_D" to enable debug mode.
#ifdef _D
	#ifndef DBG
		#define DBG(fmt, args...) \
		printf(BLUE "File: %s, Line: %d, Function: %s : " NONE YELLOW fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
		#define DBGIN(fmt, args...) \
		printf(RED "File: %s, Line: %d, Function: %s : " NONE GREEN fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
		#define DBGOUT(fmt, args...) \
		printf(BLUE "File: %s, Line: %d, Function: %s : " NONE CYAN fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
		#define CUT(fmt, args...) \
		printf(RED "File: %s, Line: %d, Function: %s : -------------------------------------------------------" NONE fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);	
	#endif
#else
	#ifndef DBG
		#define DBG(fmt, args...)
		#define DBGIN(fmt, args...) 
		#define DBGOUT(fmt, args...) 
		#define CUT(fmt, args...) 
	#endif
#endif



#endif
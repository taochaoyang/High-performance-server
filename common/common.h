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

// Define macro "_D" to enable debug mode.
#ifdef _D
	#ifndef DBG
		#define DBG(fmt, args...) \
		printf(BLUE fmt NONE "\n", ##args);
	#endif
#else
	#ifndef DBG
		#define DBG(fmt, args...)
	#endif
#endif



#endif
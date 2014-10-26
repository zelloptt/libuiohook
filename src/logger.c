/* libUIOHook: Cross-platfrom userland keyboard and mouse hooking.
 * Copyright (C) 2006-2014 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/libuiohook/
 *
 * libUIOHook is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libUIOHook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <uiohook.h>

#include "logger.h"

static bool default_logger(unsigned int level, const char *format, ...) {
	bool status = false;

	#ifndef USE_QUIET
	va_list args;
	switch (level) {
		#ifdef USE_DEBUG
		case LOG_LEVEL_DEBUG:
		#endif
		case LOG_LEVEL_INFO:
			va_start(args, format);
  			status = vfprintf(stdout, format, args) >= 0;
			va_end(args);
			break;

		case LOG_LEVEL_WARN:
		case LOG_LEVEL_ERROR:
			va_start(args, format);
  			status = vfprintf(stderr, format, args) >= 0;
			va_end(args);
			break;
	}
	#endif

	return status;
}

// Current logger function pointer, this should never be null.
logger_t logger = &default_logger;


UIOHOOK_API void hook_set_logger_proc(logger_t logger_proc) {
	if (logger_proc == NULL) {
		logger = &default_logger;
	}
	else {
		logger = logger_proc;
	}
}

//TODO: think about where does this make more sense
//Whether the event should be trapped/consumed or not
//with extern doesnt compile on mingw but without doesn't on mac-clang...
//extern bool trapEvents[ EVENT_MOUSE_WHEEL - EVENT_HOOK_START + 1];
bool trapEvents[ EVENT_MOUSE_WHEEL - EVENT_HOOK_START + 1];

//TODO: move to a better place and initialize somewhere to all false
UIOHOOK_API void hook_trap_event( event_type eventType, bool shouldBeTrapped ){
	trapEvents[ eventType - 1 ] = shouldBeTrapped;
}

//TODO: move to a better place and initialize somewhere to all false
void reserveIfTrapEvent( uint16_t *reserveEvent, event_type eventType ){
	//Set reserved if the event was marked to be trapped/consumed
	//And respect the value if already enable
	//This can be controlled by the application using the library api
	//i.e. using a special key comb to enable/disable traps
	if( *reserveEvent ^ 0x01 )
		*reserveEvent = (trapEvents[ eventType - 1 ]) ? 0x01: 0x00;
		//event.reserved = (trapEvents[ event.type - 1 ]) ? 0x01: 0x00;
	//return (trapEvents[ eventType - 1 ]) ? 0x01: 0x00;
}



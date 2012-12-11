#pragma once

#ifndef ALREADY_INCLUDED_LIBS
	#define ALREADY_INCLUDED_LIBS
	#pragma comment(lib, "WS2_32.lib")
	#pragma comment(lib, "winmm.lib")
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "Oleaut32.lib")

	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "MikeNetDebug.lib")
		#else
			#pragma comment(lib, "MikeNet.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "MikeNetDebug64.lib")
		#else
			#pragma comment(lib, "MikeNet64.lib")
		#endif
	#endif
#endif
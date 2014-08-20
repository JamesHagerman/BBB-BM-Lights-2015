#include <stdio.h>
#include "p9813.h"

class TCLControl {
	public:
		TCLControl();
		~TCLControl();
		void BuildRadialRemap();
		void PrintRemapArray();
		void Update();

		bool enabled;

		int totalPixels;
		// These actually get handed in to the TCopen function...:
		static const int nStrands = 6; 
		static const int pixelsPerStrand = 100;

		// ... While these are the REAL spoke pattern on the car:
		static const int wands = 12;
		static const int pixelsPerWand = 50;

		// Our pixel buffer itself:
		// Keep in mind that this is malloc'd so it's REAL memory... Don't leak it.
		TCpixel *pixelBuf;
		int *remapArray;

		// Stats and Statuses about the TC library:
		TCstats stats;
		TCstatusCode status;
	private:

		
};
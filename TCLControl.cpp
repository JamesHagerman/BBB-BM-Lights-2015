#include <stdlib.h>
#include "p9813.h"
#include "TCLControl.h"

TCLControl::TCLControl() {
	printf("Initalizing Main TCLControl class\n");
	// Configure FTDI pins:
    
    status = TCsetStrandPin(0, TC_FTDI_TX); // default
    status = TCsetStrandPin(1, TC_FTDI_RX); // default
    status = TCsetStrandPin(2, TC_FTDI_DTR); // spliting dtr and rts
    status = TCsetStrandPin(3, TC_FTDI_RTS);
    status = TCsetStrandPin(4, TC_FTDI_RI);
    status = TCsetStrandPin(5, TC_FTDI_DSR);
    status = TCsetStrandPin(6, TC_FTDI_DCD);

    /* Allocate pixel array.  One TCpixel per pixel per strand. */
    totalPixels = nStrands * pixelsPerStrand;
    int memSize = totalPixels * sizeof(TCpixel);
    if(NULL == (pixelBuf = (TCpixel *)malloc(memSize)))
    {
        printf("Could not allocate space for %d pixels (%d bytes).\n", totalPixels, memSize);
        exit(1);
    }

    /* Initialize library, open FTDI device.  Baud rate errors
       are non-fatal; program displays a warning but continues. */
    if((status = TCopen(nStrands,pixelsPerStrand)) != TC_OK)
    {
        TCprintError(static_cast<TCstatusCode>(status));
        if(status < TC_ERR_DIVISOR){
            // exit(1);
            // We don't wanna exit. We wanna keep running the UI until a reset is called for
            printf("Oops! FTDI had issues! Check your connections.\n");
            printf("That said, we're still gonna show that UI...\n");
            enabled = false;
        }
    } else {
        enabled = true;
    }

    /* Initialize statistics structure before use. */
    TCinitStats(&stats);

}

TCLControl::~TCLControl() {
	TCclose();
    free(pixelBuf);
}

void TCLControl::Update() {
	// Update the lights:
    // x += (double)pixelsPerStrand / 20000.0;
    // s1 = sin(x                 ) *  11.0;
    // s2 = sin(x *  0.857 - 0.214) * -13.0;
    // s3 = sin(x * -0.923 + 1.428) *  17.0;
    // for(i=0;i<totalPixels;i++)
    // {
    //     r   = (int)((sin(s1) + 1.0) * 127.5);
    //     g   = (int)((sin(s2) + 1.0) * 127.5);
    //     b   = (int)((sin(s3) + 1.0) * 127.5);
    //     pixelBuf[i] = TCrgb(r,g,b);
    //     s1 += 0.273;
    //     s2 -= 0.231;
    //     s3 += 0.428;
    // }

    // Simply call TCrefresh with our updated buffer:
	// TCstatusCode TCrefresh(
	// 	TCpixel *pixelInBuffer,
	// 	int     *remap,
	// 	TCstats *stats)
    if((status = TCrefresh(pixelBuf,NULL,&stats)) != TC_OK)
       TCprintError(static_cast<TCstatusCode>(status));

    // /* Update statistics once per second. */
    // if((t = time(NULL)) != prev)
    // {
    //     system("clear");
    //     TCprintStats(&stats);
    //     prev = t;
    // }
}
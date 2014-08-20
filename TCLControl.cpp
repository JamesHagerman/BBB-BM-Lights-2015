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
    BuildRadialRemap();
    PrintRemapArray();
    
    /* Initialize library, open FTDI device.  Baud rate errors
       are non-fatal; program displays a warning but continues. */
    if((status = TCopen(nStrands,pixelsPerStrand)) != TC_OK)
    {
        TCprintError(static_cast<TCstatusCode>(status));
        if(status < TC_ERR_DIVISOR){
            // exit(1);
            // We don't wanna exit. We wanna keep running the UI until a reset is called for
            printf("Oops! FTDI had issues! Check your connections.\n");
            printf("That said, we're still gonna show that UI...\n\n");
            enabled = false;
        }
    } else {
        enabled = true;
    }

    /* Initialize statistics structure before use. */
    TCinitStats(&stats);
}

TCLControl::~TCLControl() {
    delete [] remapArray;
    TCclose();
    free(pixelBuf);
}

void TCLControl::BuildRadialRemap() {
    remapArray = new int[totalPixels];

    int index = 0;
    for(int i=0; i<wands; i++) {
      // println("Setting wand: " + i);
      for(int j=0;j<pixelsPerWand;j++) {
        if(j%2==0) { // even led's (0,2,4,6...)
          remapArray[j-(j/2) + (pixelsPerWand * i)] = index;
          // if (i == 1) {
          //   println("index " + index + " is: " + radialMap[index]);
          // }
        } else { // odd led's (1,3,5,7...)
          remapArray[(pixelsPerWand * (i+1)) - (j-(j/2))] = index;
          // if (i == 1) {
          //   println("index " + index + " is: " + radialMap[index]);
          // }
        }
        index += 1;
      }
    }
}

void TCLControl::PrintRemapArray() {
    int index = 0;
    int strandIndex = 0;
    for(int i=0; i<totalPixels; i++) {
        // Print wand index:
        if (index == 0) {
            printf("Wand %i: ", strandIndex);
        }

        // Print Pixel index:
        printf("%i ", remapArray[i]);
        if (index < 49) {
            index += 1;
        } else {
            printf("\n");
            strandIndex += 1;
            index = 0;
        }
    }
    printf("Done printing remap array!\n\n");
}

void TCLControl::Update() {
    // Update the lights:
    if((status = TCrefresh(pixelBuf,remapArray,&stats)) != TC_OK) {
        TCprintError(static_cast<TCstatusCode>(status));
    }
}
#include <unistd.h>
#include <stdlib.h>

#include "WinTypes.h"
#include "ftd2xx.h"
#include <strings.h>
#include <malloc.h>
//#ifdef _WINDOWS
//#include <windows.h>
//#endif

#include "ftd2xx_if.h"
//#include <stdio.h>
#define FT2232_SYNC 1

#define FT_LATENCY   2
#define FT_BITMASK   (0x00)        /*all inputs*/
#define FT_BITMODE   (0x08)        /*MCU Host Bus Emulation Mode*/
//static FILE * logfile;
static FT_HANDLE ftHandle;
static FT_STATUS ftStatus;
static char bus_usb_description[] = "miniLA USB Interface A";
static char bus_usb_description2[] = "miniLA USB Interface B"; // after the "A" device gets opened it disappears from the list

static DWORD bytesReadReally;

int d2xx_open( void)
{
  DWORD bytesNr;
  unsigned char ftBuffer[1000];
  char bCommandEchod;
  unsigned int icnt;
  int openstatus = 0;
  //logfile = fopen("log.log","wb");
  // otevrit zarizeni
  ftStatus = FT_OpenEx((void*)bus_usb_description, FT_OPEN_BY_DESCRIPTION,&ftHandle);
  if (ftStatus == FT_OK) {

     do { // because of fast break
       //printf("\nConfiguring port for MPSSE use...\n");
       ftStatus |= FT_ResetDevice(ftHandle); // reset the device

        //Purge USB receive buffer first by reading out all old data from FT2232H receive buffer
        ftStatus |= FT_GetQueueStatus(ftHandle, &bytesNr);   // Get the number of bytes in the FT2232H receive buffer
        if ((ftStatus == FT_OK) && (bytesNr > 0)) {
           FT_Read(ftHandle, &ftBuffer, bytesNr, &bytesNr);    //Read out the data from FT2232H receive buffer
        }

        ftStatus |= FT_SetUSBParameters(ftHandle, 65536, 65535);  //Set USB request transfer sizes to 64K
        ftStatus |= FT_SetChars(ftHandle, 0, 0, 0, 0);    //Disable event and error characters
        ftStatus |= FT_SetTimeouts(ftHandle, 0, 5000);            //Sets the read and write timeouts in milliseconds
        ftStatus |= FT_SetLatencyTimer(ftHandle, 16);             //Set the latency timer (default is 16mS)
        ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x00);           //Reset controller
        ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x08);           //Enable MPSSE MCU mode
        if (ftStatus != FT_OK) {
     //           printf("Error in initializing the MPSSE %d\n", ftStatus);
           FT_Close(ftHandle);
           break;
        }
     

//#ifdef FT2232_SYNC
        /* ----------------------------------------------------------- */
        /* Synchronize the MPSSE by sending a bogus opcode (0xAA), */
        /* The MPSSE will respond with "Bad Command" (0xFA) followed by */
        /* the bogus opcode itself. */
        /* ----------------------------------------------------------- */
     //        printf("MCU mode synchronization\n");
        ftStatus |= FT_GetQueueStatus(ftHandle, &bytesNr);   // Get the number of bytes in the FT2232H receive buffer
   //        printf("d2xx_sync - flushing\n");
        if ((ftStatus == FT_OK) && (bytesNr > 0)) {
           FT_Read(ftHandle, &ftBuffer, bytesNr, &bytesNr);    //Read out the data from FT2232H receive buffer
/*            for(icnt = 0; icnt < bytesNr; icnt++) { */
/*               printf( "0x%02X ", ftBuffer[icnt]); */
/*            } */
/*            printf("\n"); */

        }
        do {
           ftBuffer[0] = 0xAA;
           ftStatus = FT_Write( ftHandle, ftBuffer, 1, &bytesNr);    // Send off the BAD commands
           usleep( 10000);
           ftStatus = FT_GetQueueStatus(ftHandle, &bytesNr); // Get the number of bytes in the device input buffer
      //           printf("*");
        } while ((bytesNr == 0) && (ftStatus == FT_OK));

        bCommandEchod = 0;
        ftStatus = FT_Read(ftHandle, ftBuffer, bytesNr, &bytesNr); //Read out the data from input buffer
   //        printf("d2xx_sync - received response: ");
        for (icnt = 0; icnt < bytesNr - 1; icnt++) {  //Check if Bad command and echo command received
     //         printf( "0x%02X ", ftBuffer[icnt]);
           if ((ftBuffer[icnt] == 0xFA) && (ftBuffer[icnt+1] == 0xAA)) {
              bCommandEchod = 1;
              break;
           }
        }
   //      printf("\n");
        if (bCommandEchod == 0) {
     //           printf("Error in synchronizing the MPSSE\n");
           FT_Close(ftHandle);
           break;
        }
   openstatus = 1;
     } while (0);
   //        printf("MCU mode synchronization - done\n");
//#endif


  }

  return openstatus;
}

int d2xx_close( void)
{
   FT_SetBitMode( ftHandle,0,0); //fclose( logfile);
  if ( FT_Close( ftHandle) == FT_OK) {
    return 1;
  } else {
    return 0;
  }
}

#define FUNC_D2XX_WRITE (0x93)
#define FUNC_D2XX_READ (0x91)
#define FUNC_D2XX_FLUSH (0x87)

#define BYTES_SENT (100)

int d2xx_writereg ( int addr, unsigned char data)
{
   char txbuffer[BYTES_SENT];
   DWORD bytesWritten;
   //unsigned char ftdi_flush = FUNC_D2XX_FLUSH; //0x87
   FT_STATUS ftStat;
   //unsigned int icnt;

   // for( icnt = 0; icnt < BYTES_SENT; icnt += 4) {
   txbuffer[0] = FUNC_D2XX_WRITE; // 0x93
   txbuffer[1] = 0;
   txbuffer[2] = addr;
   txbuffer[3] = data;
      // }
   if( (ftStat = FT_Write( ftHandle, txbuffer, 4, &bytesWritten)) != FT_OK) {
     //      printf("d2xx_writereg() - write error 0x%02X!!!\n", ftStat);
      return -1;
   }
   //   printf("d2xx_writereg() - writen %d bytes\n", bytesWritten);
   
   ftStat = FT_GetQueueStatus(ftHandle, &bytesWritten);   // Get the number of bytes in the FT2232H receive buffer
   if ((ftStat == FT_OK) && (bytesWritten > 0)) {
      FT_Read(ftHandle, &txbuffer, bytesWritten, &bytesWritten);    //Read out the data from FT2232H receive buffer
/*       printf("d2xx_writereg() - received response: "); */
/*       for(icnt = 0; icnt < bytesWritten; icnt++) { */
/*          printf( "0x%02X ", txbuffer[icnt]); */
/*       } */
/*       printf("\n"); */
      
   }

/*    if( FT_Write( ftHandle, &ftdi_flush, 1, &bytesWritten) != FT_OK) { */
/*       printf("d2xx_writereg() - flush error\n"); */
/*       return -1; */
/*     } */

   bytesReadReally = 0; //nulovani cteciho bufferu

  return 0;
}

int d2xx_readreg ( int addr, unsigned char *data)
{
   char txbuffer[10];
   DWORD bytesWritten;
   DWORD bytesRead;
   unsigned char ftdi_flush = FUNC_D2XX_FLUSH; //0x87
   char rxbuffer[20];
   unsigned int icnt;


   txbuffer[0] = FUNC_D2XX_READ; //0x91
   txbuffer[1] = 1;
   txbuffer[2] = addr;
   if( FT_Write( ftHandle, txbuffer, 3, &bytesWritten) != FT_OK) {
     //      printf("d2xx_readreg() - write error !!!");
   }

   if( FT_Write( ftHandle, &ftdi_flush, 1, &bytesWritten) != FT_OK) {
     //      printf("d2xx_readreg() - flush error\n");
      return -1;
   }

   do {
      ftStatus = FT_GetQueueStatus(ftHandle, &bytesRead); // Get the number of bytes in the device input buffer
   } while ((bytesRead == 0) && (ftStatus == FT_OK));

   if( FT_Read( ftHandle, rxbuffer, bytesRead, &bytesRead) != FT_OK) {
     //      printf("d2xx_readreg() - read error\n");
      return -1;
   } else {
     for (icnt = 0; icnt < bytesRead; icnt++) {
       *data = rxbuffer[icnt];
      }
   }
   
  return 0;
}

int d2xx_readreg_multi ( int addr, unsigned char *data, unsigned int length)
{
  unsigned char *txbuf;
  unsigned char *rxbuf;
  unsigned char ftdi_flush = FUNC_D2XX_FLUSH; //0x87
  unsigned int icnt;
  DWORD bytesWritten;
  DWORD bytesRead;
  int retval = -1;

  txbuf = (unsigned char *) calloc(length*3, sizeof(char));
  rxbuf = (unsigned char *) calloc(length, sizeof(char));

  for (icnt = 0; icnt < length*3; icnt+= 3) {
    txbuf[icnt] = FUNC_D2XX_READ; //0x91
    txbuf[icnt+1] = 1;
    txbuf[icnt+2] = addr;
    
  }

  do {
    if( FT_Write( ftHandle, txbuf, length*3, &bytesWritten) != FT_OK) {
      //      printf("d2xx_readreg() - write error !!!");
      break;
    }

    if( FT_Write( ftHandle, &ftdi_flush, 1, &bytesWritten) != FT_OK) {
      //      printf("d2xx_readreg() - flush error\n");
      break;
    }

    do {
      ftStatus = FT_GetQueueStatus(ftHandle, &bytesRead); // Get the number of bytes in the device input buffer
      bytesReadReally += bytesRead;
/*       if( bytesRead > 0) { */
/*          fprintf( logfile, "Arrived chunk %d bytes, %d is alltogether\n", bytesRead, bytesReadReally); */
/*       } else { */
/*          fprintf( logfile,"."); */
/*       } */
/*       fflush( logfile); */
    } while ((bytesReadReally < length) && (ftStatus == FT_OK));
    //fprintf( logfile, "Arrived %d bytes\n",bytesReadReally);
    if( FT_Read( ftHandle, data, length, &bytesRead) != FT_OK) {
      //      printf("d2xx_readreg() - read error\n");
      break;
    } else {
/*       for (icnt = 0; icnt < bytesRead; icnt++) { */
/*    *data = rxbuf[icnt]; */
/*    *data++; */
/*       } */
       bytesReadReally -= bytesRead;
       retval = (int) bytesRead;
    }
  } while (0);

  free( rxbuf);
  free( txbuf);

  return retval;
}

// return 0 - no tinila
int d2xx_detect_tinila(void)
{
   FT_STATUS ftStatus;
   DWORD numDevs;

   char *strings[11]; //expecting maximum of 5 FTDI devices
   unsigned int i;
   int det = 0;
   
   // Allocate memory for each string
   for ( i = 0; i < 10; i++) {
       strings[i] = (char *) malloc(64);
   }
   
   strings[10] = NULL;
   ftStatus = FT_ListDevices(strings,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
   if (ftStatus == FT_OK) {
       // FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and 
       // numDevs contains the number of devices connected
       for (i=0; i<numDevs; i++) {
          if ( 0 == strcmp(strings[i], bus_usb_description2)) {
             det++;
          }
       }
   }  
   
   for ( i = 0; i < 10; i++) {
    free(strings[i]);
   }
   return det;

   
}


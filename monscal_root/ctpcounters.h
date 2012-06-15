/*  ctpcounters.h */
/* the number of counters per 1 CTP board: */
#define NCOUNTERS_L0 160    //  3 spares
#define NCOUNTERS_L0_SP1 15
#define NCOUNTERS_L0_SP2 99
#define NCOUNTERS_L0_SP3 159
#define NCOUNTERS_L1 160    // 14 spares
#define NCOUNTERS_L1_SP1 38   // 2 spares
#define NCOUNTERS_L1_SP2 148  // 12 spares
#define NCOUNTERS_L2 134    //  1 spare
#define NCOUNTERS_L2_SP1 25
#define NCOUNTERS_FO  48    // there are 6 FO boards (FO1-FO6) (was 34 till 11.11)
#define NCOUNTERS_BUSY 160  // 105 +49spares + 6runx
#define NCOUNTERS_BUSY_SP1 105
#define NCOUNTERS_BUSY_TSGROUP 153  // active Time sharing class group
#define NCOUNTERS_BUSY_RUNX1 154  // 6 RUNX counters from here (see dimcoff,dims)
#define NCOUNTERS_INT 19 
#define NCOUNTERS_SPEC 49   /* 2 + 1 + 22 + 24
 0: 2 words:server unix time in seconds + micseconds
 2: 1 word: Orbit number
 3: 2 words: temperature, volts for BUSY board
 5: 2 words: temperature, volts for L0   board
 7: 2 words: temperature, volts for L1   board
 9: 2 words: temperature, volts for L2   board
11: 2 words: temperature, volts for INT  board
13: 2x6 words: temperature, volts for FO1-6  boards
          temperature: in centigrades (0: if not valid or error)
          volts: 4 values in 4 bytes. See
          'LTU software model', chapter: LTU DC Voltage monitoring
          on ALICE-trigger web page
25: 24 words: volts for 24 LTUs
together: 49 WORDS
*/
#define NCOUNTERS_MAX 160   // max. number of counters on 1 board

#define NCOUNTERS (NCOUNTERS_L0+NCOUNTERS_L1+NCOUNTERS_L2+6*NCOUNTERS_FO+\
        NCOUNTERS_BUSY+NCOUNTERS_INT+NCOUNTERS_SPEC)
// i.e.: 320+134+6*34+48+19+2+23+24=774
//       320+134+6*34+160+19+2+23+24=886
//       320+134+6*48+160+19+2+23+24=970     11.11.2008
#define CSTART_L0   0
#define CSTART_L1   NCOUNTERS_L0
#define CSTART_L2   (NCOUNTERS_L0+NCOUNTERS_L1)
#define CSTART_FO   (NCOUNTERS_L0+NCOUNTERS_L1+NCOUNTERS_L2)
#define CSTART_BUSY (NCOUNTERS_L0+NCOUNTERS_L1+NCOUNTERS_L2+6*NCOUNTERS_FO)
#define CSTART_INT  (NCOUNTERS_L0+NCOUNTERS_L1+NCOUNTERS_L2+6*NCOUNTERS_FO+\
        NCOUNTERS_BUSY)
#define CSTART_SPEC  (NCOUNTERS_L0+NCOUNTERS_L1+NCOUNTERS_L2+6*NCOUNTERS_FO+\
        NCOUNTERS_BUSY+NCOUNTERS_INT)
#define CSTART_LTUVOLTS CSTART_SPEC+25
#define CSTART_RUNX (CSTART_BUSY+NCOUNTERS_BUSY_RUNX1)
#define CSTART_TSGROUP (CSTART_BUSY+NCOUNTERS_BUSY_TSGROUP)

// following defined for ctp_proxy.c:
#define SODEODfol0out1 36
#define SODEODfol1out1 40
#define SODEODfol2stro1 44


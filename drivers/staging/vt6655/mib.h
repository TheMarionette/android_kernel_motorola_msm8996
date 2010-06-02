/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * File: mib.h
 *
 * Purpose: Implement MIB Data Structure
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 */

#ifndef __MIB_H__
#define __MIB_H__

#include "ttype.h"
#include "tether.h"
#include "desc.h"

/*---------------------  Export Definitions -------------------------*/
//
// 802.11 counter
//

typedef struct tagSDot11Counters {
    unsigned long Length;             // Length of structure
    unsigned long long   TransmittedFragmentCount;
    unsigned long long   MulticastTransmittedFrameCount;
    unsigned long long   FailedCount;
    unsigned long long   RetryCount;
    unsigned long long   MultipleRetryCount;
    unsigned long long   RTSSuccessCount;
    unsigned long long   RTSFailureCount;
    unsigned long long   ACKFailureCount;
    unsigned long long   FrameDuplicateCount;
    unsigned long long   ReceivedFragmentCount;
    unsigned long long   MulticastReceivedFrameCount;
    unsigned long long   FCSErrorCount;
    unsigned long long   TKIPLocalMICFailures;
    unsigned long long   TKIPRemoteMICFailures;
    unsigned long long   TKIPICVErrors;
    unsigned long long   TKIPCounterMeasuresInvoked;
    unsigned long long   TKIPReplays;
    unsigned long long   CCMPFormatErrors;
    unsigned long long   CCMPReplays;
    unsigned long long   CCMPDecryptErrors;
    unsigned long long   FourWayHandshakeFailures;
//    unsigned long long   WEPUndecryptableCount;
//    unsigned long long   WEPICVErrorCount;
//    unsigned long long   DecryptSuccessCount;
//    unsigned long long   DecryptFailureCount;
} SDot11Counters, *PSDot11Counters;


//
// MIB2 counter
//
typedef struct tagSMib2Counter {
    long    ifIndex;
    char    ifDescr[256];               // max size 255 plus zero ending
                                        // e.g. "interface 1"
    long    ifType;
    long    ifMtu;
    DWORD   ifSpeed;
    BYTE    ifPhysAddress[ETH_ALEN];
    long    ifAdminStatus;
    long    ifOperStatus;
    DWORD   ifLastChange;
    DWORD   ifInOctets;
    DWORD   ifInUcastPkts;
    DWORD   ifInNUcastPkts;
    DWORD   ifInDiscards;
    DWORD   ifInErrors;
    DWORD   ifInUnknownProtos;
    DWORD   ifOutOctets;
    DWORD   ifOutUcastPkts;
    DWORD   ifOutNUcastPkts;
    DWORD   ifOutDiscards;
    DWORD   ifOutErrors;
    DWORD   ifOutQLen;
    DWORD   ifSpecific;
} SMib2Counter, *PSMib2Counter;

// Value in the ifType entry
//#define ETHERNETCSMACD      6           //
#define WIRELESSLANIEEE80211b      6           //

// Value in the ifAdminStatus/ifOperStatus entry
#define UP                  1           //
#define DOWN                2           //
#define TESTING             3           //


//
// RMON counter
//
typedef struct tagSRmonCounter {
    long    etherStatsIndex;
    DWORD   etherStatsDataSource;
    DWORD   etherStatsDropEvents;
    DWORD   etherStatsOctets;
    DWORD   etherStatsPkts;
    DWORD   etherStatsBroadcastPkts;
    DWORD   etherStatsMulticastPkts;
    DWORD   etherStatsCRCAlignErrors;
    DWORD   etherStatsUndersizePkts;
    DWORD   etherStatsOversizePkts;
    DWORD   etherStatsFragments;
    DWORD   etherStatsJabbers;
    DWORD   etherStatsCollisions;
    DWORD   etherStatsPkt64Octets;
    DWORD   etherStatsPkt65to127Octets;
    DWORD   etherStatsPkt128to255Octets;
    DWORD   etherStatsPkt256to511Octets;
    DWORD   etherStatsPkt512to1023Octets;
    DWORD   etherStatsPkt1024to1518Octets;
    DWORD   etherStatsOwners;
    DWORD   etherStatsStatus;
} SRmonCounter, *PSRmonCounter;

//
// Custom counter
//
typedef struct tagSCustomCounters {
    unsigned long Length;

    unsigned long long   ullTsrAllOK;

    unsigned long long   ullRsr11M;
    unsigned long long   ullRsr5M;
    unsigned long long   ullRsr2M;
    unsigned long long   ullRsr1M;

    unsigned long long   ullRsr11MCRCOk;
    unsigned long long   ullRsr5MCRCOk;
    unsigned long long   ullRsr2MCRCOk;
    unsigned long long   ullRsr1MCRCOk;

    unsigned long long   ullRsr54M;
    unsigned long long   ullRsr48M;
    unsigned long long   ullRsr36M;
    unsigned long long   ullRsr24M;
    unsigned long long   ullRsr18M;
    unsigned long long   ullRsr12M;
    unsigned long long   ullRsr9M;
    unsigned long long   ullRsr6M;

    unsigned long long   ullRsr54MCRCOk;
    unsigned long long   ullRsr48MCRCOk;
    unsigned long long   ullRsr36MCRCOk;
    unsigned long long   ullRsr24MCRCOk;
    unsigned long long   ullRsr18MCRCOk;
    unsigned long long   ullRsr12MCRCOk;
    unsigned long long   ullRsr9MCRCOk;
    unsigned long long   ullRsr6MCRCOk;

} SCustomCounters, *PSCustomCounters;


//
// Custom counter
//
typedef struct tagSISRCounters {
    unsigned long Length;

    DWORD   dwIsrTx0OK;
    DWORD   dwIsrAC0TxOK;
    DWORD   dwIsrBeaconTxOK;
    DWORD   dwIsrRx0OK;
    DWORD   dwIsrTBTTInt;
    DWORD   dwIsrSTIMERInt;
    DWORD   dwIsrWatchDog;
    DWORD   dwIsrUnrecoverableError;
    DWORD   dwIsrSoftInterrupt;
    DWORD   dwIsrMIBNearfull;
    DWORD   dwIsrRxNoBuf;

    DWORD   dwIsrUnknown;               // unknown interrupt count

    DWORD   dwIsrRx1OK;
    DWORD   dwIsrATIMTxOK;
    DWORD   dwIsrSYNCTxOK;
    DWORD   dwIsrCFPEnd;
    DWORD   dwIsrATIMEnd;
    DWORD   dwIsrSYNCFlushOK;
    DWORD   dwIsrSTIMER1Int;
    /////////////////////////////////////
} SISRCounters, *PSISRCounters;


// Value in the etherStatsStatus entry
#define VALID               1           //
#define CREATE_REQUEST      2           //
#define UNDER_CREATION      3           //
#define INVALID             4           //

//#define MAX_RATE            12
//
// statistic counter
//
typedef struct tagSStatCounter {
    //
    // ISR status count
    //


    // RSR status count
    //
    DWORD   dwRsrFrmAlgnErr;
    DWORD   dwRsrErr;
    DWORD   dwRsrCRCErr;
    DWORD   dwRsrCRCOk;
    DWORD   dwRsrBSSIDOk;
    DWORD   dwRsrADDROk;
    DWORD   dwRsrBCNSSIDOk;
    DWORD   dwRsrLENErr;
    DWORD   dwRsrTYPErr;

    DWORD   dwNewRsrDECRYPTOK;
    DWORD   dwNewRsrCFP;
    DWORD   dwNewRsrUTSF;
    DWORD   dwNewRsrHITAID;
    DWORD   dwNewRsrHITAID0;

    DWORD   dwRsrLong;
    DWORD   dwRsrRunt;

    DWORD   dwRsrRxControl;
    DWORD   dwRsrRxData;
    DWORD   dwRsrRxManage;

    DWORD   dwRsrRxPacket;
    DWORD   dwRsrRxOctet;
    DWORD   dwRsrBroadcast;
    DWORD   dwRsrMulticast;
    DWORD   dwRsrDirected;
    // 64-bit OID
    unsigned long long   ullRsrOK;

    // for some optional OIDs (64 bits) and DMI support
    unsigned long long   ullRxBroadcastBytes;
    unsigned long long   ullRxMulticastBytes;
    unsigned long long   ullRxDirectedBytes;
    unsigned long long   ullRxBroadcastFrames;
    unsigned long long   ullRxMulticastFrames;
    unsigned long long   ullRxDirectedFrames;

    DWORD   dwRsrRxFragment;
    DWORD   dwRsrRxFrmLen64;
    DWORD   dwRsrRxFrmLen65_127;
    DWORD   dwRsrRxFrmLen128_255;
    DWORD   dwRsrRxFrmLen256_511;
    DWORD   dwRsrRxFrmLen512_1023;
    DWORD   dwRsrRxFrmLen1024_1518;

    // TSR status count
    //
    DWORD   dwTsrTotalRetry[TYPE_MAXTD];        // total collision retry count
    DWORD   dwTsrOnceRetry[TYPE_MAXTD];         // this packet only occur one collision
    DWORD   dwTsrMoreThanOnceRetry[TYPE_MAXTD]; // this packet occur more than one collision
    DWORD   dwTsrRetry[TYPE_MAXTD];             // this packet has ever occur collision,
                                         // that is (dwTsrOnceCollision0 + dwTsrMoreThanOnceCollision0)
    DWORD   dwTsrACKData[TYPE_MAXTD];
    DWORD   dwTsrErr[TYPE_MAXTD];
    DWORD   dwAllTsrOK[TYPE_MAXTD];
    DWORD   dwTsrRetryTimeout[TYPE_MAXTD];
    DWORD   dwTsrTransmitTimeout[TYPE_MAXTD];

    DWORD   dwTsrTxPacket[TYPE_MAXTD];
    DWORD   dwTsrTxOctet[TYPE_MAXTD];
    DWORD   dwTsrBroadcast[TYPE_MAXTD];
    DWORD   dwTsrMulticast[TYPE_MAXTD];
    DWORD   dwTsrDirected[TYPE_MAXTD];

    // RD/TD count
    DWORD   dwCntRxFrmLength;
    DWORD   dwCntTxBufLength;

    BYTE    abyCntRxPattern[16];
    BYTE    abyCntTxPattern[16];



    // Software check....
    DWORD   dwCntRxDataErr;             // rx buffer data software compare CRC err count
    DWORD   dwCntDecryptErr;            // rx buffer data software compare CRC err count
    DWORD   dwCntRxICVErr;              // rx buffer data software compare CRC err count
    unsigned int idxRxErrorDesc[TYPE_MAXRD]; // index for rx data error RD

    // 64-bit OID
    unsigned long long   ullTsrOK[TYPE_MAXTD];

    // for some optional OIDs (64 bits) and DMI support
    unsigned long long   ullTxBroadcastFrames[TYPE_MAXTD];
    unsigned long long   ullTxMulticastFrames[TYPE_MAXTD];
    unsigned long long   ullTxDirectedFrames[TYPE_MAXTD];
    unsigned long long   ullTxBroadcastBytes[TYPE_MAXTD];
    unsigned long long   ullTxMulticastBytes[TYPE_MAXTD];
    unsigned long long   ullTxDirectedBytes[TYPE_MAXTD];

//    DWORD   dwTxRetryCount[8];
    //
    // ISR status count
    //
    SISRCounters ISRStat;

    SCustomCounters CustomStat;

   #ifdef Calcu_LinkQual
       //Tx count:
    unsigned long TxNoRetryOkCount;         //success tx no retry !
    unsigned long TxRetryOkCount;              //success tx but retry !
    unsigned long TxFailCount;                      //fail tx ?
      //Rx count:
    unsigned long RxOkCnt;                          //success rx !
    unsigned long RxFcsErrCnt;                    //fail rx ?
      //statistic
    unsigned long SignalStren;
    unsigned long LinkQuality;
   #endif
} SStatCounter, *PSStatCounter;

/*---------------------  Export Classes  ----------------------------*/

/*---------------------  Export Variables  --------------------------*/

/*---------------------  Export Functions  --------------------------*/

void STAvClearAllCounter(PSStatCounter pStatistic);

void STAvUpdateIsrStatCounter(PSStatCounter pStatistic, DWORD dwIsr);

void STAvUpdateRDStatCounter(PSStatCounter pStatistic,
                              BYTE byRSR, BYTE byNewRSR, BYTE byRxRate,
                              PBYTE pbyBuffer, unsigned int cbFrameLength);

void STAvUpdateRDStatCounterEx(PSStatCounter pStatistic,
                              BYTE byRSR, BYTE byNewRsr, BYTE byRxRate,
                              PBYTE pbyBuffer, unsigned int cbFrameLength);

void STAvUpdateTDStatCounter(PSStatCounter pStatistic, BYTE byTSR0, BYTE byTSR1,
		PBYTE pbyBuffer, unsigned int cbFrameLength, unsigned int uIdx);

void STAvUpdateTDStatCounterEx(
    PSStatCounter   pStatistic,
    PBYTE           pbyBuffer,
    DWORD           cbFrameLength
    );

void STAvUpdate802_11Counter(
    PSDot11Counters p802_11Counter,
    PSStatCounter   pStatistic,
    DWORD           dwCounter
    );

void STAvClear802_11Counter(PSDot11Counters p802_11Counter);

#endif // __MIB_H__




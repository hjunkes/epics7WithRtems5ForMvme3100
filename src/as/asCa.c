/* share/src/as/asCa.c	*/
/* share/src/as $Id$ */
/* Author:  Marty Kraimer Date:    10-15-93 */
/*****************************************************************
                          COPYRIGHT NOTIFICATION
*****************************************************************

(C)  COPYRIGHT 1993 UNIVERSITY OF CHICAGO

This software was developed under a United States Government license
described on the COPYRIGHT_UniversityOfChicago file included as part
of this distribution.
**********************************************************************/
/*
 *
 * Modification Log:
 * -----------------
 * .01  03-22-94	mrk	Initial Implementation
 */

/*This module is separate from asDbLib because CA uses old database access*/
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "dbDefs.h"
#include "osiThread.h"
#include "osiSem.h"
#include "cantProceed.h"
#include "errlog.h"
#include "taskwd.h"
#include "asDbLib.h"
#include "cadef.h"
#include "caerr.h"
#include "caeventmask.h"
#include "alarm.h"

#define epicsExportSharedSymbols

int asCaDebug = 0;
epicsShareExtern ASBASE volatile *pasbase;
LOCAL int firstTime = TRUE;
LOCAL threadId threadid=0;
LOCAL int caInitializing=FALSE;
LOCAL semId asCaTaskLock;		/*lock access to task */
LOCAL semId asCaTaskWait;		/*Wait for task to respond*/
LOCAL semId asCaTaskAddChannels;	/*Tell asCaTask to add channels*/
LOCAL semId asCaTaskClearChannels;	/*Tell asCaTask to clear channels*/

typedef struct {
    struct dbr_sts_double rtndata;
    chid		chid;
} CAPVT;

/*connectCallback only handles disconnects*/
LOCAL void connectCallback(struct connection_handler_args arg)
{
    chid		chid = arg.chid;
    ASGINP		*pasginp = (ASGINP *)ca_puser(chid);
    ASG			*pasg = pasginp->pasg;

    if(ca_state(chid)!=cs_conn) {
	if(!(pasg->inpBad & (1<<pasginp->inpIndex))) {
	    /*was good so lets make it bad*/
	    pasg->inpBad |= (1<<pasginp->inpIndex);
	    if(!caInitializing) asComputeAsg(pasg);
	    if(asCaDebug) printf("as connectCallback disconnect %s\n",
		ca_name(chid));
	}
    }
}

LOCAL void eventCallback(struct event_handler_args arg)
{
    int		caStatus = arg.status;
    chid	chid = arg.chid;
    ASGINP	*pasginp = (ASGINP *)arg.usr;
    ASG		*pasg;
    CAPVT	*pcapvt;
    READONLY struct dbr_sts_double *pdata;

    if(caStatus!=ECA_NORMAL) {
	if(chid) {
	    epicsPrintf("asCa: eventCallback error %s channel %s\n",
	        ca_message(caStatus),ca_name(chid));
	} else {
	    epicsPrintf("asCa: eventCallback error %s chid is null\n",
		ca_message(caStatus));
	}
	return;
    }
    pasg = pasginp->pasg;
    pcapvt = (CAPVT *)pasginp->capvt;
    if(chid!=pcapvt->chid) {
	epicsPrintf("asCa: eventCallback error pcapvt->chid != arg.chid\n");
	return;
    }
    if(ca_state(chid)!=cs_conn || !ca_read_access(chid)) {
	if(!(pasg->inpBad & (1<<pasginp->inpIndex))) {
	    /*was good so lets make it bad*/
	    pasg->inpBad |= (1<<pasginp->inpIndex);
	    if(!caInitializing) asComputeAsg(pasg);
	    if(asCaDebug) {
		printf("as eventCallback %s inpBad ca_state %d"
		    " ca_read_access %d\n",
		    ca_name(chid),ca_state(chid),ca_read_access(chid));
	    }
	}
	return;
    }
    pdata = arg.dbr;
    pcapvt->rtndata = *pdata; /*structure copy*/
    if(pdata->severity==INVALID_ALARM) {
        pasg->inpBad |= (1<<pasginp->inpIndex);
	if(asCaDebug)
	    printf("as eventCallback %s inpBad because INVALID_ALARM\n",
	    ca_name(chid));
    } else {
        pasg->inpBad &= ~((1<<pasginp->inpIndex));
        pasg->pavalue[pasginp->inpIndex] = pdata->value;
	if(asCaDebug)
	    printf("as eventCallback %s inpGood data %f\n",
		ca_name(chid),pdata->value);
    }
    pasg->inpChanged |= (1<<pasginp->inpIndex);
    if(!caInitializing) asComputeAsg(pasg);
}

LOCAL void asCaTask(void)
{
    ASG		*pasg;
    ASGINP	*pasginp;
    CAPVT	*pcapvt;
    int		status;

    taskwdInsert(threadGetIdSelf(),NULL,NULL);
    SEVCHK(ca_task_initialize(),"ca_task_initialize");
    while(TRUE) { 
        semBinaryTakeAssert(asCaTaskAddChannels);
	caInitializing = TRUE;
	pasg = (ASG *)ellFirst(&pasbase->asgList);
	while(pasg) {
	    pasginp = (ASGINP *)ellFirst(&pasg->inpList);
	    while(pasginp) {
		pasg->inpBad |= (1<<pasginp->inpIndex);
		pcapvt = pasginp->capvt = asCalloc(1,sizeof(CAPVT));
		/*Note calls connectCallback immediately for local Pvs*/
		status = ca_search_and_connect(pasginp->inp,&pcapvt->chid,
		    connectCallback,pasginp);
		if(status!=ECA_NORMAL) {
		    epicsPrintf("asCa ca_search_and_connect error %s\n",
			ca_message(status));
		}
		/*Note calls eventCallback immediately  for local Pvs*/
		status = ca_add_event(DBR_STS_DOUBLE,pcapvt->chid,
		    eventCallback,pasginp,0);
		if(status!=ECA_NORMAL) {
		    epicsPrintf("asCa ca_add_event error %s\n",
			ca_message(status));
		}
		pasginp = (ASGINP *)ellNext((ELLNODE *)pasginp);
	    }
	    pasg = (ASG *)ellNext((ELLNODE *)pasg);
	}
	asComputeAllAsg();
	caInitializing = FALSE;
	if(asCaDebug) printf("asCaTask initialized\n");
	semBinaryGive(asCaTaskWait);
	while(TRUE) {
	    if(semBinaryTakeNoWait(asCaTaskClearChannels)==semTakeOK) break;
	    ca_pend_event(2.0);
	}
	pasg = (ASG *)ellFirst(&pasbase->asgList);
	while(pasg) {
	    pasginp = (ASGINP *)ellFirst(&pasg->inpList);
	    while(pasginp) {
		pcapvt = (CAPVT *)pasginp->capvt;
		status = ca_clear_channel(pcapvt->chid);
		if(status!=ECA_NORMAL) {
		    epicsPrintf("asCa ca_clear_channel error %s\n",
			ca_message(status));
		}
		free(pasginp->capvt);
		pasginp->capvt = 0;
		pasginp = (ASGINP *)ellNext((ELLNODE *)pasginp);
	    }
	    pasg = (ASG *)ellNext((ELLNODE *)pasg);
	}
	if(asCaDebug) printf("asCaTask has cleared all channels\n");
	semBinaryGive(asCaTaskWait);
    }
}
    
void asCaStart(void)
{
    if(asCaDebug) printf("asCaStart called\n");
    if(firstTime) {
	firstTime = FALSE;
        if((asCaTaskLock=semMutexCreate())==0)
	    cantProceed("asCa semMutexCreate failure\n");
        if((asCaTaskWait=semBinaryCreate(semEmpty))==0)
	    cantProceed("asCa semBinaryCreate failure\n");
        if((asCaTaskAddChannels=semBinaryCreate(semEmpty))==0)
	    cantProceed("asCa semBinaryCreate failure\n");
        if((asCaTaskClearChannels=semBinaryCreate(semEmpty))==0)
	    cantProceed("asCa semBCreate failure\n");
        threadid = threadCreate("asCaTask",
            (threadPriorityScanLow - 3),
            threadGetStackSize(threadStackBig),
            (THREADFUNC)asCaTask,0);
	if(threadid==0) {
	    errMessage(0,"asCaStart: taskSpawn Failure\n");
	}
    }
    semMutexTakeAssert(asCaTaskLock);
    semBinaryGive(asCaTaskAddChannels);
    semBinaryTakeAssert(asCaTaskWait);
    if(asCaDebug) printf("asCaStart done\n");
    semMutexGive(asCaTaskLock);
}

void asCaStop(void)
{
    if(threadid==0) return;
    if(asCaDebug) printf("asCaStop called\n");
    semMutexTakeAssert(asCaTaskLock);
    semBinaryGive(asCaTaskClearChannels);
    semBinaryTakeAssert(asCaTaskWait);
    if(asCaDebug) printf("asCaStop done\n");
    semMutexGive(asCaTaskLock);
}

# epics7WithRtems5ForMvme3100
Epics7 (epics-base) for use with RTEMS5 and MVME3100 (for Miroslaw)


Hello Miroslaw,
unfortunately I don't know exactly how to get this. You can clone it and then you need to do
"git checkout 7.0".

The bash script "INSTALLING_5_MASTER" can be used to install RTEMS5 for mvme3100. It contains as 
here document my small changes for the i2c devices.

Have fun/success with it.

30. 3. 2022

The console does not yet work with the MVME3100 neither with SIMPLE_CONSOLE nor with CONSOLE
```
 ***** Setting up file system *****
 ***** Initializing NFS *****
 nfsMount("141.14.131.192", "/Volumes/Epics", "/Volumes/Epics")
 Mount 141.14.131.192:/Volumes/Epics on /Volumes/Epics
 RTEMS-RPCIOD, Till Straumann, Stanford/SLAC/SSRL 2002, See LICENSE file for licensing info.
 RTEMS-NFS, Till Straumann, Stanford/SLAC/SSRL 2002, See LICENSE file for licensing info.
  check for time registered , C++ initialization ...
 ***** Preparing EPICS application *****
 chdir("/Volumes/Epics/MVME3100/dycrMvme3100/iocBoot/iocdycrMvme3100/")
 ***** Starting EPICS application *****
 #< envPaths
 epicsEnvSet("TOP","/Volumes/Epics/MVME3100/dycrMvme3100")
 cd /Volumes/Epics/MVME3100/dycrMvme3100
 ## Register all support components
 dbLoadDatabase("dbd/dycrMvme3100.dbd")
 dycrMvme3100_registerRecordDeviceDriver(pdbbase)
 ## Load record instances
 dbLoadTemplate("db/user.substitutions")
 dbLoadRecords("db/dycrMvme3100Version.db", "user=junkes")
 dbLoadRecords("db/dbSubExample.db", "user=junkes")
 #var mySubDebug 1
 #traceIocInit
 iocInit
 Starting iocInit
 ############################################################################
 ## EPICS R7.0.6.2-DEV
 ## Rev. R7.0.6.1-77-g63aef2fb1174ddf09be1
 ############################################################################
 iocRun: All initialization complete
 ## Start any sequence programs
 #seq(sncExample, "user=junkes")
 DYCRVME>
```

# Update Wedneday late Afternoon

Unfortunately I found another problem and nothing works now after I fixed it :-(

 `__RTEMS_MAJOR__` is not defined to "5", I had to add
 
 OP_SYS_INCLUDE_CPPFLAGS += -include $(RTEMS_TOOLS)/powerpc-rtems5/mvme3100/lib/include/rtems/score/cpuopts.h
 
 to configure/os/CONFIG.Common.RTEMS-mvme3100
 
 (Sorry have to leave ...)
 
 # Update Thursday
 
 Everything runs again as before without the modification to CONFIG.Common.RTEMS-mvme3100.
 `__RTEMS_MAJOR__` is defined as before???
 
 telnet access works, but console doesn't...
 

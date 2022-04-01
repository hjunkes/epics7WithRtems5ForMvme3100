# epics7WithRtems5ForMvme3100
Epics7 (epics-base) for use with RTEMS5 and MVME3100 (for Miroslaw)

### Friday 1.4. :
I don't know exactly why, but now it seems to run reasonably well. Also with the console.

```
DYCRVME> date
2022/04/01 16:51:25.661184
DYCRVME> epicsThreadShowAll
            NAME       EPICS ID   PTHREAD ID   OSIPRI  OSSPRI  STATE
          _main_       0x52e930            0      0       0       OK
          errlog       0x531b10    184614915     10      26       OK
          taskwd       0xd307f8    184614916     10      26       OK
      timerQueue       0xd1f120    184614917     70     178       OK
           cbLow       0xd21200    184614918     59     150       OK
        cbMedium       0xd213a8    184614919     64     162       OK
          cbHigh       0xd21550    184614920     71     180       OK
        dbCaLink       0xd217a0    184614921     50     127       OK
            PVAL       0xd220c8    184614922     50     127       OK
       PDB-event      0x123efb8    184614923     19      49       OK
pvAccess-client       0x12cb588    184614924     35      89       OK
UDP-rx 0.0.0.0:0      0x13cc850    184614925     50     127       OK
UDP-rx 141.14.12      0x1531190    184614926     50     127       OK
UDP-rx 141.14.14      0x1631390    184614927     50     127       OK
UDP-rx 224.0.0.1      0x1755980    184614928     50     127       OK
        scanOnce      0x1858ef0    184614929     67     170       OK
         scan-10      0x1959150    184614930     60     152       OK
          scan-5      0x1a59278    184614931     61     155       OK
          scan-2      0x1b593a0    184614932     62     157       OK
          scan-1      0x1c594c8    184614933     63     160       OK
        scan-0.5      0x1d595f0    184614934     64     162       OK
        scan-0.2      0x1e59718    184614935     65     165       OK
        scan-0.1      0x1f59840    184614936     66     167       OK
         CAS-TCP      0x205e2b8    184614937     18      46       OK
         CAS-UDP      0x205e3d8    184614938     16      41       OK
      CAS-beacon      0x216f0f0    184614939     17      44       OK
       CAC-event      0x21c29f0    184614940     51     130       OK
     PVAS timers      0x2277480    184614941     25      64       OK
    TCP-acceptor      0x2377c38    184614942     50     127       OK
UDP-rx 0.0.0.0:0      0x2378418    184614943     50     127       OK
UDP-rx 141.14.12      0x25dcd68    184614944     50     127       OK
UDP-rx 141.14.14      0x26dcf68    184614914     50     127       OK
UDP-rx 224.0.0.1      0x2801528    184614945     50     127       OK
```
Helpful is also the rt - command built in by Michael:

```
DYCRVME> rt help
help: The topics are
  all, help, misc, files, rtems, mem, network, monitor
ERR: 1
DYCRVME> rt driver
  Major      Entry points
------------------------------------------------------------------------------
  0          init: [0x3d8154];  control: [0x3d82dc]
             open: [0x3d827c];  close: [0x3d82c4]
             read: [0x3d82cc];  write: [0x3d82d4]
  1          init: [0x27b440];  control: [0x27b4e4]
             open: [0x27b4b4];  close: [0x27b4bc]
             read: [0x27b4c4];  write: [0x27b4cc]
  2          init: [0x27b4ec];  control: [0x27b558]
             open: [0x27b500];  close: [0x27b508]
             read: [0x27b510];  write: [0x27b548]
  3          init: [0x3e99b0];  control: [0x3e98b8]
             open: [0x3e97d8];  close: [0x3e9848]
             read: [0x3ea330];  write: [0x3ea460]
  4          init: [0x26b55c];  control: [0x0]
             open: [0x0];  close: [0x0]
             read: [0x0];  write: [0x0]
  ...
  ```
  Telnet is also possible:
  ```
  root@nfs:/srv/tftp >> telnet 141.14.128.11
Trying 141.14.128.11...
Connected to 141.14.128.11.
Escape character is '^]'.
syslog: telnetd: accepted connection from 141.14.131.192 on /dev/pty4
info:  pty dev name = /dev/pty4
tIocSh> casr,3
Channel Access Server V4.13
No clients connected.
CAS-TCP server on 0.0.0.0:5064 with
    CAS-UDP name server on 0.0.0.0:5064
        Last name requested by 141.14.142.150:58065:
        User '', V4.13, Priority = 0, 0 Channels
Sending CAS-beacons to 1 address:
    141.14.143.255:5065
```
("bye" to leave the telnet, otherwise console gets blocked)
```
tIocSh> bye
Will end session
syslog: telnetd: releasing connection from 141.14.131.192 on /dev/pty4
Connection closed by foreign host.
```

I hope you can get on with it now.
Heinz

--------------

  





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
 

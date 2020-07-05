
>There is a app note:
release_notes/bsa_examples/Release_app_manager.txt:15:
    In fact both server and applications need to access the named socket file (**bt-daemon-socket**),

>The log“~~ERROR: app_mgt_open: Connection to server unsuccessful (0), retrying...~~”, maybe caused by access socket file (bt-daemon-socket) failed. Please check "/var/run/bt-daemon-socket".
So make sure app_xxx run under **"/var/run/"** directory.

# Description:

>bsa0106_01.62.00_ingenic_linglong_20161220.tar.gz --> demo source code
bsa0106_01.62.00_ingenic_linglong_20170627.tar.gz --> bt so lib

```
$ ls  bsa0106_01.62.00_ingenic_linglong_20161220
3rdparty
mips_notrace
release_notes
x86_mips_trace

$ ls ../bsa0106_01.62.00_ingenic_linglong_20170627/
master_mips_no_trace.tar.gz  master_mips_with_trace.tar.gz
```

# Build app on PC

## setup env.
Check makefile File:bsa0106_01.62.00_ingenic_linglong_20161220/3rdparty/embedded/bsa_examples/linux/app_common/build/make.common

```
   $ export customer=bsa_examples
   $ export project=linux
   $ export MIPSGCC=mips-linux-gnu-gcc
   $ export CPU=mips
   $ export BSASHAREDLIB=TRUE

   # bsa no trace version
   $ rm -fr 3rdparty/embedded/bsa_examples/linux/libbsa/
   $ cp -fr mips_notrace/libbsa 3rdparty/embedded/bsa_examples/linux/libbsa

   # or bsa trace version
   $ rm -fr 3rdparty/embedded/bsa_examples/linux/libbsa/
   $ cp -fr x86_mips_trace/libbsa 3rdparty/embedded/bsa_examples/linux/libbsa
```


## Build app_manager
```
   $ cd bsa0106_01.62.00_ingenic_linglong_20161220/3rdparty/embedded/bsa_examples/linux/app_manager/build
   $ make CPU=mips
 target file:  mips/app_manager
```

##  Build app_ble
```
   $ cd bsa0106_01.62.00_ingenic_linglong_20161220/3rdparty/embedded/bsa_examples/linux/app_ble/build
   $ make CPU=mips
 target file:  mips/app_ble
```


## Strip target image:
```
   $ mips-linux-gnu-gcc bsa_server
   $ mips-linux-gnu-gcc ...
```

## push bsa app to target board:
```
   $ adb push bsa0106_01.62.00_ingenic_linglong_20161220/x86_mips_trace/server/mips/bsa_server /usr/bin/
   $ adb push bsa0106_01.62.00_ingenic_linglong_20161220/x86_mips_trace/libbsa/build/mips/sharedlib/libbsa.so /usr/lib/
   $ adb push bsa0106_01.62.00_ingenic_linglong_20161220/3rdparty/embedded/bsa_examples/linux/app_manager/build/mips/app_manager /usr/bin/
   $ adb push bsa0106_01.62.00_ingenic_linglong_20161220/3rdparty/embedded/bsa_examples/linux/app_ble/build/mips/app_ble /usr/bin/
```


# Run app on Halley2 board:

## 1. run bsa_server

### A0 firmware:
> \# **bsa_server** -d/dev/ttyS0 -p/lib/firmware/BCM4343A0_001.001.034.0056.0221_26M_ORC.hcd  -r 14  -u /var/run/

### A1 firmware:
> \# **bsa_server** -d/dev/ttyS0 -p/lib/firmware/BCM4343A1_001.002.009.0025.0059.hcd  -r 14  -u /var/run/

### Debug:
>You can add --all=5 for more debug log.

### bt-daemon-socket
 It will generate bt-avk-fifo bt-daemon-socket and it means bsa  service opened successfully.
>\# cd /var/run/
\# ls
dbus              sdp               wpa_supplicant
bt-avk-fifo       ifstate           syslogd.pid
bt-daemon-socket  klogd.pid         telnetd.pid
bt_config.xml     network           utmp


## 2. app_manager
>>> **Note: run app_manager first before all other app_xxx;**

---

>\# cd /var/run/
\# **app_manager**
	Created thread with thread_id=  77a874a0
	DEBUG: app_mgr_read_config: Enable:1
	DEBUG: app_mgr_read_config: Discoverable:1
	DEBUG: app_mgr_read_config: Connectable:1
	DEBUG: app_mgr_read_config: Name:My BSA Bluetooth Device
	DEBUG: app_mgr_read_config: BLE Name:
	DEBUG: app_mgr_read_config: Bdaddr be:ef:be:ef:76:73
	DEBUG: app_mgr_read_config: ClassOfDevice:00:04:24 => Audio/Video
	DEBUG: app_mgr_read_config: RootPath:./pictures
	DEBUG: app_mgr_read_config: Default PIN (4):0000
	ERROR: app_xml_read_db: open(./bt_devices.xml) failed
	ERROR: app_mgr_read_remote_devices: app_xml_read_db failed:-1
	ERROR: app_mgr_config: No remote device database found
	DEBUG: app_mgr_get_bt_config: Enable:1
	DEBUG: app_mgr_get_bt_config: Discoverable:0
	DEBUG: app_mgr_get_bt_config: Connectable:0
	DEBUG: app_mgr_get_bt_config: Name:JBL GO Smart
	DEBUG: app_mgr_get_bt_config: Name:Ble Device
	DEBUG: app_mgr_get_bt_config: Bdaddr 43:43:a0:12:1f:ac
	DEBUG: app_mgr_get_bt_config: ClassOfDevice:00:00:00
	DEBUG: app_mgr_get_bt_config: First host disabled channel:255
	DEBUG: app_mgr_get_bt_config: Last host disabled channel:255
	DEBUG: app_mgr_sec_set_security:
	Add all devices found in database
	ERROR: app_xml_read_si_db: open(./si_devices.xml) failed
	ERROR: app_read_xml_si_devices: app_xml_read_si_db failed: -1
	DEBUG: app_mgr_set_bt_config: Enable:1
	DEBUG: app_mgr_set_bt_config: Discoverable:1
	DEBUG: app_mgr_set_bt_config: Connectable:1
	DEBUG: app_mgr_set_bt_config: Name:My BSA Bluetooth Device
	DEBUG: app_mgr_set_bt_config: BLE Name:
	DEBUG: app_mgr_set_bt_config: Bdaddr be:ef:be:ef:76:73
	DEBUG: app_mgr_set_bt_config: ClassOfDevice:00:04:24
	DEBUG: app_mgr_set_bt_config: First host disabled channel:79
	DEBUG: app_mgr_set_bt_config: Last host disabled channel:79
	Server status:0
	FW Version:1.1.34.221
	BSA Server Version:BSA0106_01.62.00
	DEBUG: app_dm_get_dual_stack_mode: Get DualStackMode
	Current DualStack mode:DUAL_STACK_MODE_BSA
	Bluetooth Application Manager Main menu:
    1 => Abort Discovery
    2 => Discovery
    3 => Discovery test
    4 => Bonding
    5 => Cancel Bonding
    6 => Remove device from security database
    7 => Services Discovery (all services)
    8 => Device Id Discovery
    9 => Stop Bluetooth
    10 => Restart Bluetooth
    11 => Accept Simple Pairing
    12 => Refuse Simple Pairing
    13 => Enter BLE Passkey
    14 => Act As HID Keyboard (SP passkey entry)
    15 => Read Device configuration
    16 => Read Local Out Of Band data
    17 => Enter remote Out Of Band data
    18 => Set device visibility
    19 => Set device BLE visibility
    20 => Set AFH Configuration
    21 => Set Tx Power Class2 (specific FW needed)
    22 => Set Tx Power Class1.5 (specific FW needed)
    23 => Change Dual Stack Mode (currently:DUAL_STACK_MODE_BSA)
    24 => Set Link Policy
    25 => Enter Passkey
    26 => Get Remote Device Name
    27 => RSSI Measurement
    96 => Kill BSA server
    97 => Connect to BSA server
    98 => Disconnect from BSA server
    99 => Quit

---

>**Confirm the device MAC address:
    15 => Read Device configuration
Connect as a traditional Bluetooth:
    18 => Set device visibility
Connect as a ble Bluetooth:
    19 => Set device BLE visibility**

## 3. app_ble

Run on board:
>\# cd /var/run/
\# **app_ble**
DEBUG: app_ble_client_xml_read: cannot open:./bt_ble_client_devices.xml in read mode
DEBUG: app_ble_client_xml_read: Create an empty BLE Client database
Created thread with thread_id=  76e234a0
app_ble_start
    1 => Abort Discovery
    2 => Start BLE Discovery
    3 => Configure BLE Background Connection Type
    4 => Configure BLE Scan Parameter
    5 => Configure BLE Connection Parameter
    6 => Configure BLE Advertisement Parameters
    7 => Configure for Wake on BLE
    **** Bluetooth Low Energy Client menu ****
    8 => Register client app
    9 => Deregister Client app
    10 => Connect to Server
    11 => Close Connection
    12 => Remove BLE device
    13 => Read information
    14 => Write information
    15 => Service Discovery
    16 => Register Notification
    17 => Deregister Notification
    18 => Display Clients
    19 => Search Device Information Service
    20 => Read Manufacturer Name
    21 => Read Model Number
    22 => Read Serial Number
    23 => Read Hardware Revision
    24 => Read Firmware Revision
    25 => Read Software Revision
    26 => Read System ID
    27 => Search Battery Service
    28 => Read Battery Level
    **** Bluetooth Low Energy Server menu ****
    29 => Register server app
    30 => Deregister server app
    31 => Connect to client
    32 => Close connection
    33 => Create service
    34 => Add character
    35 => Start service
    36 => Configure BLE Advertisement data
    37 => Display Servers
    38 => Send indication
    99 => QUIT
    Select action =>

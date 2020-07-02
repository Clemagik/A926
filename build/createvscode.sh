#/bin/bash

PROJECTNAME=$1
PROGRAMPATH=$2
WORKDIR=$(cd $3; pwd)

TOPDIR=$(cd $4; pwd)

OSDIR=$5

CrossPROGRAM=$1
DebuggerPath=$(which mips-linux-gnu-gdb)

DBG=.vscode

FILENAME1=launch.json
FILENAME2=tasks.json

FILENAME=$1

touch $WORKDIR/$FILENAME.code-workspace

echo "{" > $WORKDIR/$FILENAME.code-workspace
echo "    \"folders\": [ " >> $WORKDIR/$FILENAME.code-workspace
echo "        {" >> $WORKDIR/$FILENAME.code-workspace
echo "            \"path\": \".\" " >> $WORKDIR/$FILENAME.code-workspace
echo "        }" >> $WORKDIR/$FILENAME.code-workspace
echo "    ]," >> $WORKDIR/$FILENAME.code-workspace
echo "    \"settings\": {" >> $WORKDIR/$FILENAME.code-workspace
echo "        \"code-runner.executorMap\": {" >> $WORKDIR/$FILENAME.code-workspace
echo "         	 \"c\": \" cd $TOPDIR && make $PROJECTNAME && cd $TOPDIR/$PROGRAMPATH && cp $CrossPROGRAM $CrossPROGRAM.old && mips-linux-gnu-strip $CrossPROGRAM && adb push $CrossPROGRAM / && mv $CrossPROGRAM.old $CrossPROGRAM && adb shell /gdbserver 127.0.0.1:5000 /$CrossPROGRAM \"," >> $WORKDIR/$FILENAME.code-workspace
echo "         	 \"cpp\": \" cd $TOPDIR && make $PROJECTNAME && cd $TOPDIR/$PROGRAMPATH && cp $CrossPROGRAM $CrossPROGRAM.old && mips-linux-gnu-strip $CrossPROGRAM && adb push $CrossPROGRAM / && mv $CrossPROGRAM.old $CrossPROGRAM && adb shell /gdbserver 127.0.0.1:5000 /$CrossPROGRAM \"," >> $WORKDIR/$FILENAME.code-workspace
echo "        }" >> $WORKDIR/$FILENAME.code-workspace
echo "    }" >> $WORKDIR/$FILENAME.code-workspace
echo "}" >> $WORKDIR/$FILENAME.code-workspace

mkdir $WORKDIR/$DBG

echo "{" > $WORKDIR/$DBG/$FILENAME1
echo "    \"version\": \"0.2.0\"," >> $WORKDIR/$DBG/$FILENAME1
echo "    \"configurations\": [" >> $WORKDIR/$DBG/$FILENAME1
echo "         {" >> $WORKDIR/$DBG/$FILENAME1
echo "            \"name\": \"Cross-debugging\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"type\": \"cppdbg\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"request\": \"launch\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"program\": \"$TOPDIR/$PROGRAMPATH/$CrossPROGRAM\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"args\": []," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"stopAtEntry\": false," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"cwd\": \"$WORKDIR\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"environment\": []," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"externalConsole\": true," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"linux\": {" >> $WORKDIR/$DBG/$FILENAME1
echo "                \"MIMode\": \"gdb\"," >> $WORKDIR/$DBG/$FILENAME1
echo "                \"miDebuggerPath\": \"$DebuggerPath\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            }," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"additionalSOLibSearchPath\": \"$OSDIR\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"preLaunchTask\": \"adb forward\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"postDebugTask\": \"adb forward remove\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"miDebuggerServerAddress\": \"127.0.0.1:5000\"," >> $WORKDIR/$DBG/$FILENAME1
echo "            \"setupCommands\": [" >> $WORKDIR/$DBG/$FILENAME1
echo "                {" >> $WORKDIR/$DBG/$FILENAME1
echo "                    \"description\": \"Enable pretty-printing for gdb\"," >> $WORKDIR/$DBG/$FILENAME1
echo "                    \"text\": \"-enable-pretty-printing\"," >> $WORKDIR/$DBG/$FILENAME1
echo "                    \"ignoreFailures\": true, " >> $WORKDIR/$DBG/$FILENAME1
echo "                }" >> $WORKDIR/$DBG/$FILENAME1
echo "            ]" >> $WORKDIR/$DBG/$FILENAME1
echo "         }," >> $WORKDIR/$DBG/$FILENAME1
echo "     ]," >> $WORKDIR/$DBG/$FILENAME1
echo "}" >> $WORKDIR/$DBG/$FILENAME1

echo "{" > $WORKDIR/$DBG/$FILENAME2
echo "    \"version\": \"2.0.0\"," >> $WORKDIR/$DBG/$FILENAME2
echo "    \"tasks\": [" >> $WORKDIR/$DBG/$FILENAME2
echo "        {" >> $WORKDIR/$DBG/$FILENAME2
echo "            \"label\": \"adb forward\"," >> $WORKDIR/$DBG/$FILENAME2
echo "            \"type\": \"shell\"," >> $WORKDIR/$DBG/$FILENAME2
echo "            \"command\": \"adb forward tcp:5000 tcp:5000\"," >> $WORKDIR/$DBG/$FILENAME2
echo "        }," >> $WORKDIR/$DBG/$FILENAME2
echo "        {" >> $WORKDIR/$DBG/$FILENAME2
echo "            \"label\": \"adb forward remove\"," >> $WORKDIR/$DBG/$FILENAME2
echo "            \"type\": \"shell\"," >> $WORKDIR/$DBG/$FILENAME2
echo "            \"command\": \"adb forward --remove tcp:5000\"," >> $WORKDIR/$DBG/$FILENAME2
echo "        }," >> $WORKDIR/$DBG/$FILENAME2
echo "    ]" >> $WORKDIR/$DBG/$FILENAME2
echo "}" >> $WORKDIR/$DBG/$FILENAME2


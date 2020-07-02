#!/bin/bash
echo "This command is used to generate the commonly used template, specific content need to edit"
touch Build.mk
echo "LOCAL_PATH := \$(my-dir)" > Build.mk
echo "include \$(CLEAR_VARS)" >> Build.mk
echo  >> Build.mk
echo "LOCAL_MODULE =      #name of the module:" >> Build.mk
echo "LOCAL_MODULE_TAGS := optional           #Development mode of module ,userdebug,eng,optional" >> Build.mk
echo "LOCAL_MODULE_PATH := \$(TARGET_FS_BUILD)/     #which directory the target file copy " >> Build.mk

echo "Host module or device module(A:host   B:device):"
read  -p "Input Module :" num
if [[ $num == A ]] || [[ $num == a ]]
then
	echo "Please select host target type you want to build(A:execute   B:thirdpart   C:static library   D:shared library):"
	read -p host
	if [[ $host == A ]] || [[ $host == a ]]
	then
		echo "include \$(BUILD_HOST_EXECUTABLE)" >> Build.mk
	elif [[ $host == B ]] || [[ $host == b ]]
	then
		echo >> Build.mk
		echo "LOCAL_C_INCLUDES :=                             #include files" >> Build.mk
		echo "LOCAL_CPP_INCLUDES:=                            #cpp include files" >> Build.mk
		echo  >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_FILES :=                 #export included files " >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_DIRS :=                   #export included dir" >> Build.mk
		echo  >> Build.mk
		echo "LOCAL_MODULE_GEN_SHARED_FILES :=                  #The dynamic library file module" >> Build.mk
		echo "LOCAL_MODULE_GEN_STATIC_FILES :=                  #The static library file module" >> Build.mk
		echo "LOCAL_MODULE_GEN_BINRARY_FILES :=                 #The binrary file module" >> Build.mk
		echo  >> Build.mk
		echo "LOCAL_MODULE_CONFIG_FILES :=                    #extern module configure file name" >> Build.mk
		echo "LOCAL_MODULE_CONFIG :=                          #extern module config command" >> Build.mk
		echo "LOCAL_MODULE_COMPILE =                          #extern module compile command" >> Build.mk
		echo "LOCAL_MODULE_COMPILE_CLEAN =                    #extern module clean command" >> Build.mk
		echo "include \$(BUILD_HOST_THIRDPART)" >> Build.mk
	elif [[ $host == C ]] || [[ $host == c ]]
	then
		echo "LOCAL_SRC_FILES :=                      #compiled src files " >> Build.mk
		echo >> Build.mk
		echo "include \$(BUILD_HOST_STATIC_LIBRARY)" >> Build.mk
	elif [[ $host == D ]] || [[ $host == d ]]
	then
		echo "LOCAL_SRC_FILES :=                      #compiled src files " >> Build.mk
		echo >> Build.mk
		echo "include \$(BUILD_HOST_SHARED_LIBRARY)" >> Build.mk
	else
		echo "No this select"
	fi
elif [[ $num == B ]] || [[ $num == b ]]
then
	echo ""
	echo "***************************************************"
	echo "Please select device target type you want to build"
	echo -e "\t A:execute  "
	echo -e "\t B:thirdpart  "
	echo -e "\t C:static library "
	echo -e "\t D:shared library  "
	echo -e "\t E:multi prebuilt  "
	echo -e "\t F:multi copy  "
	read -p "Input module :" device
	if [[ $device == A ]] || [[ $device == a ]]
	then
		echo "LOCAL_SRC_FILES :=                      #compiled src files " >> Build.mk
		echo "LOCAL_CFLAGS :=                                #C compile parameters" >> Build.mk
		echo "LOCAL_LDFLAGS :=                               #Link parameters" >> Build.mk
		echo "LOCAL_LDLIBS :=                                #Link parameters" >> Build.mk
		echo "LOCAL_C_INCLUDES :=                             #include files" >> Build.mk
		echo "LOCAL_CPP_INCLUDES:=                            #cpp include files" >> Build.mk
		echo "LOCAL_MODULE_GEN_BINRARY_FILES :=                 #The binrary file module" >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_FILES :=                 #export included files " >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_DIRS :=                   #export included dir" >> Build.mk
		echo "LOCAL_SHARE_LIBRARIES:=                          #depend shared library"  >>Build.mk
        echo "LOCAL_DEPANNER_MODULES :=                       #depend on module name" >> Build.mk
		echo "include \$(BUILD_EXECUTABLE)" >> Build.mk
	elif [[ $device == B ]] || [[ $device == b ]]
	then
		echo "LOCAL_MODULE_CONFIG_FILES :=                    #extern module configure file name" >> Build.mk
		echo "LOCAL_MODULE_CONFIG :=                          #extern module config command" >> Build.mk
		echo "LOCAL_MODULE_COMPILE =                          #extern module compile command" >> Build.mk
		echo "LOCAL_MODULE_COMPILE_CLEAN =                    #extern module clean command" >> Build.mk
		echo "LOCAL_DEPANNER_MODULES :=                       #depend on module name" >> Build.mk
		echo "include \$(BUILD_THIRDPART)" >> Build.mk
	elif [[ $device == C ]] || [[ $device == c ]]
	then
		echo "LOCAL_SRC_FILES :=                      #compiled src files " >> Build.mk
		echo "LOCAL_DEPANNER_MODULES :=  "        >>Build.mk             #编译需要依赖的工程中另外的模块名
		echo "LOCAL_SHARED_LIBRARIES :=   "                >>Build.mk   #依赖的动态库的全称
		echo "LOCAL_STATIC_LIBRARIES := "        >>Build.mk
		echo "LOCAL_CFLAGS :=                                #C compile parameters" >> Build.mk
		echo "LOCAL_LDFLAGS :=                               #Link parameters" >> Build.mk
		echo "LOCAL_LDLIBS :=                                #Link parameters" >> Build.mk
		echo "LOCAL_C_INCLUDES :=                             #include files" >> Build.mk
		echo "LOCAL_CPP_INCLUDES:=                            #cpp include files" >> Build.mk
		echo "LOCAL_MODULE_GEN_STATIC_FILES :=                  #The static library file module" >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_FILES :=                 #export included files " >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_DIRS :=                   #export included dir" >> Build.mk
		echo "include \$(BUILD_STATIC_LIBRARY)" >> Build.mk
	elif [[ $device == D ]] || [[ $device == d ]]
	then
		echo "LOCAL_SRC_FILES :=                      #compiled src files "                            >> Build.mk
		echo "LOCAL_DEPANNER_MODULES :=                       #depend on module name"                  >> Build.mk
		echo "LOCAL_CFLAGS := -Wa,-mips32r2 -O2 -G 0 -Wall -fPIC -shared        #C compile parameters" >> Build.mk
		echo "LOCAL_LDFLAGS :=                               #Link parameters" >> Build.mk
		echo "LOCAL_LDLIBS :=                                #Link parameters" >> Build.mk
		echo "LOCAL_C_INCLUDES :=                             #include files"                      >> Build.mk
		echo "LOCAL_CPP_INCLUDES:=                            #cpp include files"                  >> Build.mk
		echo "LOCAL_MODULE_GEN_SHARED_FILES :=                  #The dynamic library file module"  >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_FILES :=                 #export included files "             >> Build.mk
		echo "LOCAL_EXPORT_C_INCLUDE_DIRS :=                   #export included dir"               >> Build.mk
		echo "include \$(BUILD_SHARED_LIBRARY)"                                                    >> Build.mk
	elif [[ $device == E ]] || [[ $device == e ]]
	then
		echo "LOCAL_MODULE_CLASS:="                                                   >> Build.mk
		echo "LOCAL_MODULE_DIR :=                             #src module dir"         >> Build.mk
		echo "LOCAL_MODULE_OUT_DIR :="                                                 >>Build.mk
		echo "LOCAL_COPY_FILES :="                                                     >>Build.mk
		echo "include \$(BUILD_MULTI_PREBUILT)"                                        >> Build.mk
	elif [[ $device == F ]] || [[ $device == f ]]
	then
		echo "LOCAL_MODULE_CLASS:="                                                                      >> Build.mk
		echo "LOCAL_MODULE_DIR :=                             #src module dir"                           >> Build.mk
		echo "LOCAL_MODULE_OUT_DIR :="                                                                   >>Build.mk
		echo "LOCAL_COPY_FILES :="                                                                       >>Build.mk
		echo "LOCAL_DEPANNER_MODULES :=                       #depend on module name"                    >> Build.mk
		echo "include \$(BUILD_MULTI_COPY)"                                                              >> Build.mk
	else
		echo "No this select"
	fi

else
	echo "No this select"
fi

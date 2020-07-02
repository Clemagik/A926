#!/bin/bash

function gettop
{
	local TOPFILE=build/core/envsetup.mk
	if [ -n "$TOP" -a -f "$TOP/$TOPFILE" ] ; then
		echo $TOP
	else
		if [ -f $TOPFILE ] ; then
			# The following circumlocution (repeated below as well) ensures
			# that we record the true directory name and not one that is
			# faked up with symlink names.
			PWD= /bin/pwd
		else
			local HERE=$PWD
			T=
			while [ \( ! \( -f $TOPFILE \) \) -a \( $PWD != "/" \) ]; do
				\cd ..
				T=`PWD= /bin/pwd`
			done
			\cd $HERE
			if [ -f "$T/$TOPFILE" ]; then
				echo $T
			fi
		fi
	fi
}
unset LUNCH_MENU_CHOICES
function add_lunch_combo()
{
	local new_combo=$1
	local c
	for c in ${LUNCH_MENU_CHOICES[@]} ; do
		if [ "$new_combo" = "$c" ] ; then
			return
		fi
	done
	LUNCH_MENU_CHOICES=(${LUNCH_MENU_CHOICES[@]} $new_combo)
}
function print_lunch_menu()
{
	local uname=$(uname)
	echo
	echo "You're building on" $uname
	echo
	echo "Lunch menu... pick a combo:"

	local i=1
	local choice
	for choice in ${LUNCH_MENU_CHOICES[@]}
	do
		echo "     $i. $choice"
		i=$(($i+1))
	done

	echo
}

function get_build_var()
{
	T=$(gettop)
	if [ ! "$T" ]; then
		echo "Couldn't locate the top of the tree.  Try setting TOP." >&2
		return
	fi

	find_complite
	if [ $? -eq 0 ];then
		CALLED_FROM_SETUP=true BUILD_SYSTEM=build/core \
			make --no-print-directory -C "$T" -f build/core/dumpvar.mk dumpvar-$1
	else
		echo -e "${color_failed}Failed!${color_reset}"
	fi
}

function printconfig()
{
	T=$(gettop)
	if [ ! "$T" ]; then
		echo "Couldn't locate the top of the tree.  Try setting TOP." >&2
		return
	fi
	get_build_var report_config
}

function findmakefile()
{
	TOPFILE=build/core/envsetup.mk
	local HERE=$PWD
	T=
	while [ \( ! \( -f $TOPFILE \) \) -a \( $PWD != "/" \) ]; do
		T=`PWD= /bin/pwd`
		if [ -f "$T/Build.mk" ]; then
			echo $T/Build.mk
			\cd $HERE
			return
		fi
		\cd ..
	done
	\cd $HERE
}


function mm()
{
	# If we're sitting in the root of the build tree, just do a
	# normal make.
	if [ -f build/core/envsetup.mk -a -f Makefile ]; then
		make $@
	else
		# Find the closest Android.mk file.
		T=$(gettop)
		local M=$(findmakefile)
		local MODULES=
		local GET_INSTALL_PATH=
		local ARGS=
		# Remove the path to top as the makefilepath needs to be relative
		local M=`echo $M|sed 's:'$T'/::'`
		if [ ! "$T" ]; then
			echo "Couldn't locate the top of the tree.  Try setting TOP."
		elif [ ! "$M" ]; then
			echo "Couldn't locate a makefile from the current directory."
		else
			MAKE_JLEVEL=
			for ARG in $@; do
				case $ARG in
					GET-INSTALL-PATH) GET_INSTALL_PATH=$ARG;;
					clean) MODULES=clean;;
					-j*) MAKE_JLEVEL="MAKE_JLEVEL="`echo $ARG | tr -cd "[0-9]"`;;
				esac
			done

			if [ -n "$GET_INSTALL_PATH" ]; then
				MODULES=
				ARGS=GET-INSTALL-PATH
			elif [ -n "$MODULES" ];then
				ARGS=
			elif [ -n "$MAKE_JLEVEL" ];then
					 ARGS=$MAKE_JLEVEL
			else
				MODULES=all_modules
				ARGS=$@
			fi
			ONE_SHOT_MAKEFILE=$M make -C $T -f build/core/main.mk $MODULES $ARGS
		fi
	fi
}

function mma()
{
  if [ -f build/core/envsetup.mk -a -f Makefile ]; then
    make $@
  else
    T=$(gettop)
    if [ ! "$T" ]; then
      echo "Couldn't locate the top of the tree.  Try setting TOP."
    fi
    local MY_PWD=`PWD= /bin/pwd|sed 's:'$T'/::'`
    make -C $T -f build/core/main.mk $@ all_modules BUILD_MODULES_IN_PATHS="$MY_PWD"
  fi
}

function croot()
{
    T=$(gettop)
    if [ "$T" ]; then
        \cd $(gettop)
    else
        echo "Couldn't locate the top of the tree.  Try setting TOP."
    fi
}

function f_directory()
{
	dirnum=0
	currentfile=$(ls -l $1 |awk '/^d/ {print $NF}')
	echo $currentfile
	for i in $currentfile
	do
		dirnum=`expr $dirnum + 1`
	done
	return $dirnum
}

function find_complite()
{
	T=$(gettop)
	TOOLCHAINS_DIR_PATH=$T/prebuilts/toolchains
	SL_TOOLCHAIN=$TOOLCHAINS_DIR_PATH/mips-gcc-glibc
	TOOLCHAIN=$SL_TOOLCHAIN/bin

	#test if the soft-link is available.
	if [ -L $SL_TOOLCHAIN ];then
		Invalid_link=`find -L $TOOLCHAINS_DIR_PATH -maxdepth 1 -type l`
		for f_link in $Invalid_link
		do
			if [  $f_link = $SL_TOOLCHAIN ];then
				echo "Invalid link,remove it!"
				echo "rm --> $f_link"
				rm $f_link
			fi
		done
	fi

	if [ -L $SL_TOOLCHAIN ];then
		TOOLCHAINS_LINKPATH=`readlink $SL_TOOLCHAIN`
		TOOLCHAINS_DIR=`basename $TOOLCHAINS_LINKPATH`
		COMPILER_LIB_VERSION=`ls --color=never $TOOLCHAINS_DIR_PATH/$TOOLCHAINS_DIR/lib/gcc/mips-linux-gnu/`
	else
		f_directory $TOOLCHAINS_DIR_PATH
		TDIRNUM=$dirnum
		if [ $TDIRNUM -le 0 ];then
			echo  -e " ${color_failed}ERROR: Toolchain Not Found!!${color_reset}"
			return 1
		elif [ $TDIRNUM -eq 1 ];then
			TOOLCHAINS_DIR=$(ls  --color=never -l $TOOLCHAINS_DIR_PATH |awk '/^d/ {print $NF}')
			`ln -s $TOOLCHAINS_DIR_PATH/$TOOLCHAINS_DIR $TOOLCHAINS_DIR_PATH/mips-gcc-glibc`
			COMPILER_LIB_VERSION=`ls --color=never $TOOLCHAINS_DIR_PATH/$TOOLCHAINS_DIR/lib/gcc/mips-linux-gnu/`
			if [ $? -ne 0 ];then
				echo -e "${color_failed}ERROR: Glibc Not Found!!${color_reset}"
				return 1
			fi
		else
			echo  -e "${color_failed}ERROR:	Multiple tool chains are found,please manually create mips-gcc-glibc!${color_reset}"
			return 1
		fi
	fi

	export TOOLCHAINS_DIR
	export COMPILER_LIB_VERSION
	export DEVICE_COMPILER_PREFIX='mips-linux-gnu'
	export COMPILER_PATH=$TOOLCHAINS_DIR_PATH/$TOOLCHAINS_DIR

	if [[ $PATH =~ $TOOLCHAIN ]];then
		echo "PATH=$PATH"
	else
		export PATH=$TOOLCHAIN:$PATH
	fi
}

function analysis_product()
{
	i=1
	TARGET_EXT_SUPPORT=
	TARGET_EXT2_SUPPORT=
	while(true)
	do
		split=`echo $1|cut -d "_" -f$i`

		if [ "$split" != "" ];then
			if [ "$i" == "1" ];then
				export TARGET_DEVICE=`echo $split | cut -d "." -f1`
				export TARGET_DEVICE_SUBVERSION=`echo $split. | cut -d "." -f2`
				echo $TARGET_DEVICE_SUBVERSION
				echo $TARGET_DEVICE
			fi
			if [ "$i" == "2" ];then
				export TARGET_STORAGE_MEDIUM=$split
				echo $TARGET_STORAGE_MEDIUM
			fi
			if [ "$i" == "3" ];then
				export TARGET_EXT_SUPPORT=$split
				echo $TARGET_EXT_SUPPORT
			fi
			if [ "$i" == "4" ];then
				export TARGET_EXT2_SUPPORT=$split
				echo $TARGET_EXT2_SUPPORT
			fi
			((i++))
		else

			break
		fi
	done

}

function patch_system()
{
	build/core/tools/gen_sys_script.py $TARGET_DEVICE $TARGET_STORAGE_MEDIUM $TARGET_EXT_SUPPORT
}

function autoenvsetup()
{
	build/core/tools/auto_env_setup.sh
}


function autotouchmk()
{
	T=$(gettop)
	$T/build/core/tools/auto_touch_mk.sh
}
function mnewdevice()
{
	T=$(gettop)
	$T/build/core/tools/mnewdevice.sh $1
}

_complete_local_file()
{
	local file xspec i j
	local -a dirs files

	file=$1
	xspec=$2
	# Since we're probably doing file completion here, don't add a space after.
	if [[ $(type -t compopt) = "builtin" ]]; then
		compopt -o plusdirs
		if [[ "${xspec}" == "" ]]; then
			COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -f -- "${cur}") )
		else
			compopt +o filenames
			COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -f -X "${xspec}" -- "${cur}") )
		fi
	else
		# Work-around for shells with no compopt
		dirs=( $(compgen -d -- "${cur}" ) )
		if [[ "${xspec}" == "" ]]; then
			files=( ${COMPREPLY[@]:-} $(compgen -f -- "${cur}") )
		else
			files=( ${COMPREPLY[@]:-} $(compgen -f -X "${xspec}" -- "${cur}") )
		fi
		COMPREPLY=( $(
		for i in "${files[@]}"; do
			local skip=
			for j in "${dirs[@]}"; do
				if [[ $i == $j ]]; then
					skip=1
					break
				fi
			done
			[[ -n $skip ]] || printf "%s\n" "$i"
		done
		))
		COMPREPLY=( ${COMPREPLY[@]:-} $(
		for i in "${dirs[@]}"; do
			printf "%s/\n" "$i"
		done
		))
	fi
}

function _patch_system()
{
	local cur
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"

	_complete_local_file "${cur}"

	return 0
}
complete -F _patch_system patch_system

function lunch()
{
	local answer

	if [ "$1" ] ; then
		answer=$1
	else
		print_lunch_menu
		echo -n "Which would you like? [${LUNCH_MENU_CHOICES[0]}] "
		read answer
	fi

	local selection=

	if [ -z "$answer" ]
	then
		selection=${LUNCH_MENU_CHOICES[0]}
	elif (echo -n $answer | grep -q -e "^[0-9][0-9]*$")
	then
		if [ $answer -le ${#LUNCH_MENU_CHOICES[@]} ]
		then
			selection=${LUNCH_MENU_CHOICES[$(($answer-1))]}
		fi
	elif (echo -n $answer | grep -q -e "^[^\-][^\-]*-[^\-][^\-]*$")
	then
		selection=$answer
	fi

	if [ -z "$selection" ]
	then
		echo
		echo "Invalid lunch combo: $answer"
		return 1
	fi
	export TARGET_BUILD_APPS=

	local product=$(echo -n $selection | sed -e "s/-.*$//")
	echo selection is $selection
	echo product is $product
#	check_product $product
	if [ $? -ne 0 ]
	then
		echo
		echo "** Don't have a product spec for: '$product'"
		echo "** Do you have the right repo manifest?"
		product=
	fi

	local variant=$(echo -n $selection | sed -e "s/^[^\-]*-//")
	echo variant is $variant
#	check_variant $variant
	if [ $? -ne 0 ]
	then
		echo
		echo "** Invalid variant: '$variant'"
		echo "** Must be one of ${VARIANT_CHOICES[@]}"
		variant=
	fi

	if [ -z "$product" -o -z "$variant" ]
	then
		echo
		return 1
	fi

	export TARGET_PRODUCT=$product
	export TARGET_BUILD_VARIANT=$variant
	export TARGET_BUILD_TYPE=release

	analysis_product $product

	printconfig
}

function _lunch()
{
	local cur prev opts
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"
	prev="${COMP_WORDS[COMP_CWORD-1]}"

	COMPREPLY=( $(compgen -W "${LUNCH_MENU_CHOICES[*]}" -- ${cur}) )
	return 0
}

function get_make_command()
{
  echo command make
}

function make()
{
	local start_time=$(date +"%s")
	local ARGS=
	local ARG=
	if [ -f build/core/envsetup.mk -a -f Makefile ]; then
		for ARG in $@; do
			case $ARG in
				-j*)
					ARGS+="MAKE_JLEVEL="`echo $ARG | tr -cd "[0-9]"`
					;;
				*)
					ARGS+=$ARG
					;;
			esac
			ARGS+=" "
		done
    else
		ARGS=$@
	fi
	$(get_make_command) $ARGS

	local ret=$?
    local end_time=$(date +"%s")
    local tdiff=$(($end_time-$start_time))
    local hours=$(($tdiff / 3600 ))
    local mins=$((($tdiff % 3600) / 60))
    local secs=$(($tdiff % 60))
    echo
    if [ $ret -eq 0 ] ; then
        echo -n -e "${color_success}#### make completed successfully "
    else
        echo -n -e "${color_failed}#### make failed to build some targets "
    fi
    if [ $hours -gt 0 ] ; then
        printf "(%02g:%02g:%02g (hh:mm:ss))" $hours $mins $secs
    elif [ $mins -gt 0 ] ; then
        printf "(%02g:%02g (mm:ss))" $mins $secs
    elif [ $secs -gt 0 ] ; then
        printf "(%s seconds)" $secs
    fi
    echo -e " ####${color_reset}"
    echo
    return $ret
}

ncolors=$(tput colors 2>/dev/null)
if [ -n "$ncolors" ] && [ $ncolors -ge 8 ]; then
	color_failed="\e[0;31m"
	color_success="\e[0;32m"
	color_reset="\e[00m"
else
	color_failed=""
	color_success=""
	color_reset=""
fi
complete -F _lunch lunch

#cp the main Makefile
cp -u -v  $(gettop)/build/Makefile $(gettop)

device_common=`cat $(gettop)/build/core/envsetup.mk | grep "TARGET_COMMON_PATH:"`
device_common=${device_common#*:*/}
if [ -d $(gettop)/$device_common ]; then
	echo "[Warning] $device_common exists, Using Legacy Build System."
	export USE_LEGACY_BUILD_SYSTEM="YES"
else
	export USE_LEGACY_BUILD_SYSTEM="NO"
fi

# Execute the contents of any vendorsetup.sh files we can find.
for f in `test -d device && find  device -maxdepth 5 -name 'vendorsetup.sh' 2> /dev/null`
do
	echo "including $f"
	. $f
done

if [ -f iot/build/iotenv.sh ]; then
	source iot/build/iotenv.sh
fi


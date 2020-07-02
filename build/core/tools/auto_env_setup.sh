echo "***********************************************************************"
echo "* NOTE: The automatic installation only supports Ubuntu 14.04 so far  *"
echo "*       You have to do manual install if not on Ubuntu 14.04          *"
echo "***********************************************************************"
echo "********************************************"
echo "* Following tools are required "
echo "* "
echo "*-- make"
echo "*-- build-essential"
echo "*-- fakeroot"
echo "*-- dpkg-dev"
echo "*-- libtool"
echo "*-- automake"
echo "*-- autoconf"
echo "*-- autopoint"
echo "*-- pkg-config"
echo "*-- ncurses-dev"
echo "*-- libgettextpo-dev"
echo "*-- libasprintf-dev"
echo "*-- gettext"
echo "*-- bc"
echo "*-- mtd-utils"
echo "*-- mkcramfs"
echo "*-- u-boot-tools"
echo "*-- doxygen"
echo "*-- fakeroot"
echo "*-- libpng12-dev"
echo "********************************************"
echo " Please enter Y to start the installation or any other keys to exit"
read REPLY
if [[ $REPLY == Y ]]
then
	sudo apt-get update
	sudo apt-get install  make  \
	   build-essential  \
	   fakeroot  \
	   dpkg-dev  \
	   libtool  \
	   automake  \
	   autoconf  \
	   autopoint  \
	   pkg-config  \
	   ncurses-dev  \
	   libgettextpo-dev  \
	   libasprintf-dev  \
	   gettext  \
	   bc  \
	   u-boot-tools \
	   doxygen \
	   mkcramfs \
	   genext2fs \
	   openjdk-7-jdk \
	   openjdk-7-jre\
	   fakeroot  \
	   libpng12-dev \
	   mtd-utils -y --force-yes
else
	echo "exit!"
fi


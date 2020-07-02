#!/usr/bin/env python

import os
import sys
import filecmp
import shutil
from os.path import walk,join


def scan_sys(arg,dirname,names):

	if(dirname == path_out_common_sys):
		dir_name=dirname.replace(path_out_common_sys,"")
	else:
		dir_name = dirname.replace(path_out_common_sys + os.sep,"")

	dirname_target = join(arg,dir_name)

	for filename in names:
		#common have but the target not have need rm
		if os.path.isdir(join(dirname,filename)) and not os.path.isdir(join(dirname_target,filename)):
			dispose_comom_system_fd.writelines(("rm -rf %s\n" % join(dirname,filename)))
		elif os.path.isfile(join(dirname,filename)) and not os.path.isfile(join(dirname_target,filename)):
			dispose_comom_system_fd.writelines(("rm -f %s\n" % join(dirname,filename)))
                elif os.path.islink(join(dirname,filename)) and not os.path.islink(join(dirname_target,filename)):
			dispose_comom_system_fd.writelines(("rm -f %s\n" % join(dirname,filename)))

def scan_target_sys(arg,dirname,names):

	if(dirname == arg):
		dir_name = dirname.replace(arg,"")
	else:
		dir_name = dirname.replace(arg + os.sep,"")

	dirname_common = join(path_out_common_sys,dir_name)
	dirname_out = join(path_out_sys_patch,dir_name)

	for filename in names:
		#common have but the target not have need rm
		if os.path.isdir(join(dirname,filename)) and not os.path.isdir(join(dirname_common,filename)):
			patch_system_fd.writelines(("mkdir -p %s\n" % (join(dirname_out,filename))))

		elif os.path.isfile(join(dirname,filename)) and not os.path.isfile(join(dirname_common,filename)):
			if not os.path.isdir(dirname_out):
				patch_system_fd.writelines(("mkdir -p %s\n" %dirname_out))
			patch_system_fd.writelines(("cp -adf %s %s\n" % (join(dirname,filename),join(dirname_out,filename))))
		elif os.path.isfile(join(dirname,filename)) and os.path.isfile(join(dirname_common,filename)):
			if not os.path.islink(join(dirname,filename)) and not os.path.islink(join(dirname_common,filename)):
				if not filecmp.cmp(join(dirname,filename),join(dirname_common,filename)):
					if not os.path.isdir(dirname_out):
						patch_system_fd.writelines(("mkdir -p %s\n" %dirname_out))
					patch_system_fd.writelines(("cp -adpf %s %s\n" % (join(dirname,filename),join(dirname_out,filename))))
			elif os.path.islink(join(dirname,filename)) and os.path.islink(join(dirname_common,filename)):
				target_sys_rellink = os.readlink(join(dirname,filename))
				common_sys_rellink = os.readlink(join(dirname_common,filename))
				if target_sys_rellink != common_sys_rellink:
					if not os.path.isdir(dirname_out):
						patch_system_fd.writelines(("mkdir -p %s\n" %dirname_out))
					patch_system_fd.writelines(("cp -adpf %s %s\n" % (join(dirname,filename),join(dirname_out,filename))))
			else:
				patch_system_fd.writelines(("mkdir -p %s\n" %dirname_out))
				patch_system_fd.writelines(("cp -adpf %s %s\n" % (join(dirname,filename),join(dirname_out,filename))))

def pretreat_target_sys(target_sys,target_ext_support):

        global dispose_comom_system_fd
        global patch_system_fd
	global path_out_sys_patch

        system_patch_name = "system" + target_ext_support.upper() + ".patch"
        patch_system_script = "system_rm_redundancy" + target_ext_support.upper() + ".sh"
        create_system_patch_script = "create_system_patch" + target_ext_support.upper() + ".sh"

	path_out_sys_patch = path_product + os.sep + system_patch_name

        if os.path.exists(path_out_common_sys):
            shutil.rmtree(path_out_common_sys,True)
            os.system(("cp -rf %s %s" % (path_common_sys,path_out_common_sys)))
        else:
            os.system(("mkdir -p %s" % ("out/product/" + target_device)))
            os.system(("cp -rf %s %s" % (path_common_sys,path_out_common_sys)))


        dispose_comom_system_fd = open(path_product + os.sep + patch_system_script,"w")
        patch_system_fd = open(path_product + os.sep + create_system_patch_script,"w")
        print "create %s " % (path_product + os.sep + system_patch_name)

        os.system(("chmod +x %s" % (path_product + os.sep + patch_system_script)))
        os.system(("chmod +x %s" % (path_product + os.sep + create_system_patch_script)))

        dispose_comom_system_fd.writelines(("#!/bin/bash\n"))
        patch_system_fd.writelines(("#!/bin/bash\n"))
        if not os.path.isdir(path_out_sys_patch):
            patch_system_fd.writelines(("mkdir -p %s\n" % path_out_sys_patch))


        walk(path_out_common_sys,scan_sys,target_sys)
        walk(target_sys,scan_target_sys,target_sys)

        dispose_comom_system_fd.close()
        patch_system_fd.close()

        os.system(path_product + os.sep + create_system_patch_script)
        os.remove(path_product + os.sep + create_system_patch_script)

def main(argv):
	global path_out_common_sys
	global path_target_sys
	global path_common_sys
	global path_target_sys
        global path_product
        global target_device

        target_ext_support = ""
        path_target_ext_sys = ""
        path_target_sys = ""

	path_common_sys = "device/common/system"

        target_device = argv[1]
        target_storage_medium = argv[2]

        if len(argv) >= 4:
            target_ext_support = argv[3]
            path_target_sys = raw_input("device_system(the system must in current directory --> relative path):")
            if path_target_sys.endswith(os.sep):
		path_target_sys = path_target_sys[0:len(path_target_sys) - 1]

            path_target_ext_sys = raw_input("device_ext_system(the system which support ext function must in current directory --> relative path):")
            if path_target_ext_sys.endswith(os.sep):
		path_target_ext_sys = path_target_sys[0:len(path_target_ex_sys) - 1]

            if path_target_sys:
                if not os.path.exists(path_target_sys):
                    print "the device_system %s you input is not exit,please check it \n" % path_target_sys
                    sys.exit(1)

            if path_target_ext_sys:
                if not os.path.exists(path_target_sys):
                    print "the device_system %s you input is not exit,please check it \n" % path_target_ext_sys
                    sys.exit(1)
        else:
            path_target_sys = raw_input("device_system(the system must in current directory --> relative path):")
            if path_target_sys.endswith(os.sep):
		path_target_sys = path_target_sys[0:len(path_target_sys) - 1]

            if not os.path.exists(path_target_sys):
                print "the device_system %s you input is not exit,please check it \n" % path_target_sys


	product_model = raw_input("which model (the value of 'MODEL' in device.mk) ?:");

	path_product = "device/" + target_device + os.sep + target_storage_medium + os.sep + product_model
	if not os.path.exists(path_product):
		print " the path %s is not exit,the model you input maybe error,please check it \n" % (path_product)
		sys.exit(1)

        if path_target_sys:
		path_out_common_sys = "out/product/" + target_device + os.sep + "system"
 		pretreat_target_sys(path_target_sys,"",);
        if path_target_ext_sys:
		path_out_common_sys = "out/product/" + target_device + os.sep + "system" + target_ext_support.upper()
		pretreat_target_sys(path_target_ext_sys,target_ext_support);

if __name__ == '__main__':
	main(sys.argv)

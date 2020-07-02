#!/usr/bin/python
#_*_ coding: UTF-8 _*_

import sys
import string
import os
import re


def ModifyContent(MACROS,CONTENT):
    with open ("Doxyfile","r") as fa:
        Save_list = fa.readlines()

    for i, line in enumerate(Save_list):
        line = line.rstrip()
        if MACROS in line:
            print line
            line = re.sub("(?<=\=).*",CONTENT,line)
            print line
        Save_list[i] = line + "\n"

    with open ("Doxyfile","w")as fa:
        fa.writelines(Save_list)

# read files and insert content
def InsertFile(num,CONTENT):
    with open ("Doxyfile","r")as fa:
        B = fa.read()

    a=B.split('\n')
    a.insert(num, CONTENT+ ' \\' )
    B = '\n'.join(a)

    with open ("Doxyfile","w")as fa:
        fa.write(B)

def GetContentLine(CONTENT):
    with open ("Doxyfile","r") as fileDoxy:
        for num,value in enumerate(fileDoxy):
            if CONTENT in value:
                 return num

if __name__ == "__main__":
    # modify OUTPUT_LANGUAGE
#    ModifyContent("OUTPUT_LANGUAGE"," Chinese" )
    #modfify GENERATE_TREEVIEW
    ModifyContent("GENERATE_TREEVIEW"," YES" )
    #modify INPUT
    ModifyContent("INPUT ","  \ " )
    ModifyContent("HTML_OUTPUT"," docs/html " )
    ModifyContent("LATEX_OUTPUT"," docs/latex " )

    # Insert doc list:
    fa = open(".c.tmp","r")
    linesA = fa.readlines()
    for line in linesA:
        line = line.split()
        line ="\n".join(line)
        print line
        CONTENT_LINE =GetContentLine("INPUT")
        InsertFile(CONTENT_LINE+1,line)
    fa.close()


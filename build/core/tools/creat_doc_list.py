#!/usr/bin/python

import os
import sys


def delblankline(infile,outfile):
    infp = open(infile,"r")
    outfp = open (outfile,"w")
    lines = infp.readlines()
    for line in lines:
        if line.split():
            outfp.writelines(line)
    infp.close()
    outfp.close()

def delblank(infile,outfile):
    infp = open(infile,"r")
    outfp = open (outfile,"w")
    lines = infp.readlines()
    for line in lines:
        line = line.split()
        for s in line:
            s = s + " "
            outfp.writelines(s)
    infp.close()
    outfp.close()

def splitfile(infile,outfile):
    infp = open(infile,"r")
    outfp = open (outfile,"w")
    lines = infp.readlines()
    for line in lines:
        line = "\n".join(line.split())
        #print line
        outfp.writelines(line)
    infp.close()
    outfp.close()

if __name__ == "__main__":
    delblankline(".bbbb.tmp",".a.tmp")
    delblank(".a.tmp",".b.tmp")
    splitfile(".b.tmp",".c.tmp")

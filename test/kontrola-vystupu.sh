#!/bin/bash

H_start="^[0-9][0-9]*: H [0-9][0-9]*: started$"
H_start_n=0

H_queue="^[0-9][0-9]*: H [0-9][0-9]*: going to queue$"
H_queue_n=0

H_mc="^[0-9][0-9]*: H [0-9][0-9]*: creating molecule [0-9][0-9]*$"
H_mc_n=0

H_mcf="^[0-9][0-9]*: H [0-9][0-9]*: molecule [0-9][0-9]* created$"
H_mcf_n=0

H_ne="^[0-9][0-9]*: H [0-9][0-9]*: not enough O or H$"
H_ne_n=0

O_start="^[0-9][0-9]*: O [0-9][0-9]*: started$"
O_start_n=0

O_queue="^[0-9][0-9]*: O [0-9][0-9]*: going to queue$"
O_queue_n=0

O_mc="^[0-9][0-9]*: O [0-9][0-9]*: creating molecule [0-9][0-9]*$"
O_mc_n=0

O_mcf="^[0-9][0-9]*: O [0-9][0-9]*: molecule [0-9][0-9]* created$"
O_mcf_n=0

O_ne="^[0-9][0-9]*: O [0-9][0-9]*: not enough H$"
O_ne_n=0


while read line
do
  if echo $line | grep "$H_start" >/dev/null; then
  	H_start_n=1
  elif echo $line | grep "$H_queue" >/dev/null; then
  	H_queue_n=1
  elif echo $line | grep "$H_mc" >/dev/null; then
  	H_mc_n=1
  elif echo $line | grep "$H_mcf" >/dev/null; then
  	H_mcf_n=1
  elif echo $line | grep "$H_ne" >/dev/null; then
  	H_ne_n=1
  elif echo $line | grep "$O_start" >/dev/null; then
  	O_start_n=1
  elif echo $line | grep "$O_queue" >/dev/null; then
  	O_queue_n=1
  elif echo $line | grep "$O_mc" >/dev/null; then
  	O_mc_n=1
  elif echo $line | grep "$O_mcf" >/dev/null; then
  	O_mcf_n=1
  elif echo $line | grep "$O_ne" >/dev/null; then
  	O_ne_n=1
  else
  	echo "Line format error:" $line
  fi
done

if [ ! X$H_start_n = "X1" ]; then 
	echo "WARNING: no H started"
fi
if [ ! X$H_queue_n = "X1" ]; then 
	echo "WARNING: no H going to queue"
fi
if [ ! X$H_mc_n = "X1" ]; then 
	echo "WARNING: no H started molecule creation"
fi
if [ ! X$H_mcf_n = "X1" ]; then 
	echo "WARNING: no H finished molecule creation"
fi
if [ ! X$H_ne_n = "X1" ]; then 
	echo "WARNING: no H finished with not enough O or H"
fi
if [ ! X$O_start_n = "X1" ]; then 
	echo "WARNING: no O started"
fi
if [ ! X$O_queue_n = "X1" ]; then 
	echo "WARNING: no O going to queue"
fi
if [ ! X$O_mc_n = "X1" ]; then 
	echo "WARNING: no O started molecule creation"
fi
if [ ! X$O_mcf_n = "X1" ]; then 
	echo "WARNING: no O finished molecule creation"
fi
if [ ! X$O_ne_n = "X1" ]; then 
	echo "WARNING: no O finished with not enough O"
fi


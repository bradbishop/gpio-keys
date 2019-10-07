#!/bin/sh

A=0
B=1
C=2
D=3
E=4
F=5
G=6
H=7
I=8
J=9
K=10
L=11
M=12
N=13
O=14
P=15
Q=16
R=17
S=18
T=19
U=20
V=21
W=22
X=23
Y=24
Z=25
AA=26
AB=27
AC=28

if [ -z "$1" ] || [ -z "$2" ]; then
	echo "Specify Aspeed GPIO port and offset.  Ex: E 2"
	exit 1
fi

eval port=\$`echo $1 | tr '[a-z]' '[A-Z]'`

echo $((port*8 + $2))

#!/bin/bash

USERNAME=$1
PASSWORD=$2

PASSWORD=`echo -n $PASSWORD | openssl dgst -hex -md5  | awk '{print $NF}'`

echo "${USERNAME}:${PASSWORD}" >> userfile
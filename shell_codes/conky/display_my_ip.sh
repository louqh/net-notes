#!/bin/bash
wget http://myip.com.tw/ -q -O - | grep -Eo '\<[[:digit:]]{1,3}(\.[[:digit:]]{1,3}){3}\>' | head -n 1

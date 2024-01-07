#!/bin/bash

mkdir -p /etc/sample
# Add lines to create the configuration file with environment variables
echo "mysql_host=$mysql_host" > /etc/sample/sample.cfg
echo "mysql_user=$mysql_user" >> /etc/sample/sample.cfg
echo "mysql_pass="$mysql_pass >> /etc/sample/sample.cfg
echo "mysql_port="$mysql_port >> /etc/sample/sample.cfg
echo "mysql_database="$mysql_database >> /etc/sample/sample.cfg
echo "file_path=/media/" >> /etc/sample/sample.cfg
/source/src/ample /media/
#!/bin/bash -e 

#./deps.sh 
#./reinstall.sh

TAG=`git describe`
source /etc/lsb-release
FILENAME=proxygen-$TAG-bin-$DISTRIB_ID-$DISTRIB_RELEASE.tar.gz
FULL_FILENAME=proxygen-installation/$FILENAME
rm -f $FILENAME || true
cd proxygen-installation
tar -vzcf ../$FILENAME .
cd -
s3cmd put $FILENAME s3://proxygen/
echo Successfully pushed $FILENAME to S3

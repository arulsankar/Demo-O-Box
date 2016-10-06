#!/bin/sh

#IP=$(ip addr | grep 'state global' -A2 | tail -n1 | awk -F'[/ ]+' '{print $3}')
LOCALIP=$(ip addr | awk ' /inet/ { ip[$NF] = $2; sub(/\/.*$/,"",ip[$NF]) } END { print ( "eth0" in ip ? ip ["eth0"] : ip["wlan1"] ) }')
#IP=$(ip route get 8.8.8.8 | awk '/8.8.8.8/ {print $NF}')
#EXTIP=`wget -qO- ipinfo.io/ip ; echo`
#IFace=$(ip route get 8.8.8.8 | awk '/dev/ {f=NR} f&&NR-1==f' RS="")
MAC=$(ip link show eth0 | awk '/ether/ {print $2}')
Date=$(date)
DID=OBox-200
RemotePort=8200
LocalPort=80
SubnetMask=$(/sbin/ifconfig | grep "$LOCALIP" | cut -d ":" -f4 | awk '{print $1}')
#SSH-DIRECTORY="/root/.ssh/$DID"

if [ ! -e /root/.ssh/$DID.pub ]; then
                   sudo ssh-keygen -t rsa -f ~/.ssh/$DID -C $DID -q -P ""
                   sudo chmod 400 ~/.ssh/$DID
                   SSHKEY=$DID:`sudo cat /root/.ssh/$DID.pub`
                else
                   SSHKEY=$DID:`sudo cat /root/.ssh/$DID.pub`
fi
#SSHKEY=$DID:`sudo cat /root/.ssh/$DID.pub` 

CheckInternet()
{
EXTIP=0
Google=0
count=55
while [ $Google -ne 1 ]; do
	ping -c5 8.8.8.8
		if [ $? -eq 0 ]; then
			Google=1;
			EXTIP=`wget -qO- ipinfo.io/ip ; echo`
			#echo "internet is Ok."
		else
			Google=0;
			count=`expr $count - 1`
			#echo "Sorry!!!"
		fi
		if [ "$count" = 0 ]; then
			break
		fi
done
#echo "$Google"
}

DeviceDetails(){
#IP=$(ip addr | grep 'state UP' -A2 | tail -n1 | awk -F'[/ ]+' '{print $3}')
#MAC=$(ip link show eth0 | awk '/ether/ {print $2}')
#DID=OBox-3
#SubnetMask=$(ifconfig | grep "$IP" | cut -d ":" -f4 | awk '{print $1}')
#SSH-DIRECTORY="/home/embedded/.ssh/$DID"

#if [ ! -e /home/embedded/.ssh/$DID.pub ]; then
#	           sudo ssh-keygen -t rsa -f ~/.ssh/$DID -C $DID -q -P ""
#		   sudo chmod 400 ~/.ssh/$DID
#                  SSHKEY=$DID:`sudo cat /home/embedded/.ssh/$DID.pub`
#		else
#		   SSHKEY=$DID:`sudo cat /home/embedded/.ssh/$DID.pub`
#fi
#SSHKEY=$DID:`sudo cat /home/embedded/.ssh/$DID.pub` 
echo "Date: $Date\nLocal IP Address: $LOCALIP\nExternal IP Address: $EXTIP\nMAC Address: $MAC\nDeviceID: $DID\nPort Number: $RemotePort\nssh_key: $SSHKEY\nSubnetMask: $SubnetMask" > /home/embedded/Downloads/Cubie_Backup/OBox_backup_20160328/OBox_Cubie/open-zwave-control-panel/test.txt

#echo "MAC Address: $MAC" > /home/embedded/Downloads/test.txt
DeviceReady=Ready
}

HTTPREQUEST(){
sudo curl -i -H "Accept: application/json" -H "Content-Type:application/json" -X POST --data '{"localipAddress":"'"$LOCALIP"'","externalipAddress":"'"$EXTIP"'","macAddress":"'"$MAC"'",
"serialNumber":"'"$DID"'","portNumber":"'"$RemotePort"'","sshKey":"'"$SSHKEY"'","subnetMask":"'"$SubnetMask"'","isAssigned":"'"false"'"}' "https://obox-1271.appspot.com/rest/obox/device_registration"
}
#print "$HTTP_STATUS"
Final(){
CheckInternet
#echo "$Google"
if [ "$Google" = 1 ]; then
	DeviceDetails
	if [ "$DeviceReady" = "Ready" ]; then
		HTTPREQUEST
		#echo "This is Problem"
	fi
fi
}

SSHTunnel(){
#DeviceDetails
#echo "$DID"
sudo ssh -i /root/.ssh/$DID -f -g -T -R $RemotePort:localhost:$LocalPort $DID@104.197.176.224 -N
}
test(){
#CheckInternet
#echo "$Google"
HTTP_REQUEST=$(Final | grep 'HTTP/1.1' | awk '{print $2}')
echo "$HTTP_REQUEST" 
if [ "$HTTP_REQUEST" = 200 ]; then
	#echo "Success"
	SSHTunnel
fi
}
test

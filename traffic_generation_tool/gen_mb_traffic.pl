# National Autonomous University of Mexico
# Paulo Contreras Flores
# paulo.contreras.flores@gmail.com

#!/usr/bin/perl

use strict;
use INET;


# flush after every write
$| = 1;

my ($socket, $client_socketi, $msg, $MB_trans_id);
$MB_trans_id = "0025";

$msg = $MB_trans_id."00000008020f000000080107";
#$msg = "00c800000008020f000000080103";


$socket = new IO::Socket::INET(
	PeerHost => '192.168.1.120',
	PeerPort => '502',
	Proto => 'tcp')
	or die "Creating socket $!";


for(my $i=0; $i<60; $i++){
	$MB_trans_id++;
	$msg = $MB_trans_id."00000008020f000000080103";
	my $raw = pack('H*', $msg);
	$socket->send($raw) or die "send $!";
	#$socket->send("\x{07}") or die "send $!";

	$socket->recv($msg, 1024);
	print "Received from server ";
	print unpack('H*',$msg)."\n";
	#sleep(1);
}

my $local_ip = $socket->sockhost();
print $local_ip;

$socket->close();

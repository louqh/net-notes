#!/usr/bin/expect -f
# heylinux.com

# Check
if { $argc<6 } {
    send_user "usage: $argv0 <server> <port> <user> <passwd> <rootpasswd> <cmdfile> \n"
    exit
}

# TIMEOUT
set timeout 20

# Login parameters
set server [lindex $argv 0]
set port [lindex $argv 1]
set user [lindex $argv 2]
set passwd [lindex $argv 3]
set rootpasswd [lindex $argv 4]
set cmdfile [ open [lindex $argv 5] ]

# Logfile
log_file log/run.log

# Login Server
spawn ssh -p $port $user@$server

## Enable this and Disable the "spawn ssh ..." above if you are using ssh-key.
#spawn ssh -i ssh-key/Identity.ppk -p $port $user@$server

expect {
"yes/no)?\ " {send "yes\r";exp_continue}

"*assword:\ " {send "$passwd\r"}

## Disable the "*assword:\ ..." above if you are using ssh-key, and Enable this if your ssh-key has passphrase.
#	"Identity.ppk':\ " {send "$passwd\r"}
}

# Login as Root
expect "*]$\ " {send "su - root\r"}
expect "*assword:\ " {send "$rootpasswd\r"}

# Run Commands
expect "*]#\ " {
while {[gets $cmdfile cmd] >= 0} {
    send "$cmd\r"
}
}

# Exit Root
expect "*]#\ " {send "exit\r"}

# Exit User
expect "*]$\ " {send "exit\r"}

# Close File
close $cmdfile

# Exit Expect
expect eof

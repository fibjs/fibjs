#!/usr/bash

if [ $# -lt 1 ]; then
	echo ""
	echo "option [conn | close]"
	echo ""
	exit 0
fi

user=verdant
concurrency=100000
duration=600
logdir="log/"
logfile="ctrl.log."
ip_array=("171" "193" "209" "213" "231" "219" "222" "223" "224" "225")  
conn=$(($concurrency/${#ip_array[*]}))
sleeptime=1
# sleeptime=$(($concurrency/10000+1))

# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/fibjs"
# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/node"
# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/go" 
remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/nginx"

function main()
{
	echo "${#ip_array[*]} client each: $conn"
	for ip in ${ip_array[*]}  
	do  
		if [ -e "$logdir$logfile$ip" ]; then
			rm "$logdir$logfile$ip"
		fi	   
	done

	if [ $1 = 'close' ]; then
		for ip in ${ip_array[*]}  
		do  	 
	    	result=$(ssh -t -t $user@"192.168.1."$ip "killall -9 wrk">> $logdir$logfile$ip &)
	    	sleep "$sleeptime"
	    	echo $result
	    done
		exit 0
	fi

	if [ $1 = 'conn' ]; then
		for ip in ${ip_array[*]}  
		do  	 
	    	ssh -t -t $user@"192.168.1."$ip "$remote_cmd">> $logdir$logfile$ip &  
	    	sleep "$sleeptime"
		done 
	fi
}

function recovery()
{
	running=0
	server_ok=()
	while [ $running -ne ${#ip_array[*]} ]
	do
		for ip in ${ip_array[*]}  
		do  	
			if [ ! -s "$logdir$logfile$ip" ]; then 
				echo "restart:""$logdir$logfile$ip"
				sleep $((5*$sleeptime))
				ssh -t -t $user@"192.168.1."$ip "killall -9 wrk"
				sleep $((10*$sleeptime))
				ssh -t -t $user@"192.168.1."$ip "$remote_cmd">> $logdir$logfile$ip &
			else
				if [[ "${server_ok[@]/$ip/}" = "${server_ok[@]}" ]]; then
					server_ok[${#server_ok[@]}]=$ip
					running=$(($running+1))
				fi
			fi
    	done
    done
}

function calculate()
{
	result1=0
	result2=1
	while [ $1 = 'conn' -a $result1 != $result2 ]
	do
		result1=`cat $logdir$logfile*|grep Requests/sec|awk '{print $2}'|awk '{sum+=$1} END {print sum}'`
		if [ "$result1" = "" ]; then
			result1=0
			sleep 10
			continue
		fi
		sleep 10
		result2=`cat $logdir$logfile*|grep Requests/sec|awk '{print $2}'|awk '{sum+=$1} END {print sum}'`
	done
	echo "Requests/sec: $result1"
	cat $logdir$logfile*|grep Latency
}

main $1
sleep $((${#ip_array[*]}*$sleeptime))

# 客户端异常重连机制，可根据情况打开
# if [ $1 = 'conn' ]; then
# 	recovery  
# fi

sleep $duration
echo "calculate..."
calculate $1

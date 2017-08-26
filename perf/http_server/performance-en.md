## Benchmark details

Test the performance limitation of dynamic HTTP Server, including two comparisons:

* Between different versions of fibjs (before and after optimise)
* Between recent fibjs, Nodejs(cluster), Nginx, Go

### Test environment and steps

Server
- CPU: 8 cores(4sockets*2cores), 2.1GHz/core
- Memory: 24GB

Clients(10):
- CPU: one core, 2.1GHz/core
- Memory: 1GB

Test steps:

1. Start the benchmark server.
2. The test client serves invoke the local wrk to request the benchmark server(30 min).
3. Test concurrency from 10K to 100K, increased by 10K

### Benchmark 1: between different versions of fibjs(before and after optimise)

The three versions of fibjs are:

1. v0: original fibjs
2. v1: optimise the size of SOCKET_BUFF_SIZE
3. v2: release the memory of buffer when socket is waiting

#### Benchmark Result

QPS（req/sec）

\      |   v0  |   v2  |  v3 |
------ | ----- | ----- | -----
10000  | 12750 | 15971 | 17705
20000  | 12757 | 15480 | 17893
30000  | 12551 | 15362 | 18045
40000  | 11858 | 15087 | 17326
50000  | 11678 | 15503 | 17237
60000  | 11708 | 15093 | 16800
70000  | 10682 | 14674 | 16513
80000  | 9395  | 14223 | 15840
90000  | 8733  | 13681 | 15209
100000 | 8645  | 13375 | 14664

QPS trends line chart as shown below:

![case1-QPS](./pic/case1-QPS.png)

Memory usage(GB)

\      |   v0  |  v1  |  v2  |
------ | ----- | ---- | ----
10000  | 1.55  | 0.37 | 0.14
20000  | 2.95  | 0.68 | 0.22
30000  | 4.32  | 1.05 | 0.27
40000  | 5.82  | 1.32 | 0.35
50000  | 7.27  | 1.63 | 0.41
60000  | 8.79  | 1.89 | 0.47
70000  | 10.15 | 2.32 | 0.53
80000  | 11.57 | 2.63 | 0.6
90000  | 13.03 | 2.98 | 0.67
100000 | 14.5  | 3.36 | 0.79

Memory usage trends line chart as shown below:

![case1-Mem](./pic/case1-Mem.png)

### Benchmark 2: optimised fibjs vs other servers

Servers are：

1. Fibjs(optimised)           (v0.1.7)
2. Nodejs with cluster        (v0.10.25)
3. Go with multiple core      (v1.5.1)
4. Nginx based on module      (v1.9.2)

#### Benchmark Result

QPS（req/sec）

\      | fibjs |nodejs|   go  | nginx |
------ | ----- | ----- | ----- | ------
10000  | 17705 | 8268 | 22284 | 19393
20000  | 17893 | 5866 | 21975 | 19096
30000  | 18045 | 4412 | 21531 | 18521
40000  | 17326 | 2817 | 21362 | 18331
50000  | 17237 | 2179 | 21277 | 18328
60000  | 16800 | 1853 | 21113 | 17936
70000  | 16513 | 1536 | 21029 | 17644
80000  | 15840 | 1385 | 20497 | 16973
90000  | 15209 | 1232 | 19843 | 16966
100000 | 14664 | 1089 | 19386 | 16805

QPS trends line chart as shown below:

![case2-QPS](./pic/case2-QPS.png)

Memory usage(GB)

\      |fibjs |nodejs|  go  | nginx |
------ | ---- | ---- | ---- | ----
10000  | 0.14 | 0.56 | 0.37 | 0.1
20000  | 0.22 | 0.68 | 0.68 | 0.1
30000  | 0.27 | 0.76 | 1.05 | 0.11
40000  | 0.35 | 0.84 | 1.32 | 0.12
50000  | 0.41 | 0.92 | 1.63 | 0.12
60000  | 0.47 | 1.02 | 1.89 | 0.12
70000  | 0.53 | 1.08 | 2.32 | 0.13
80000  | 0.6  | 1.14 | 2.63 | 0.13
90000  | 0.67 | 1.3  | 2.98 | 0.14
100000 | 0.79 | 1.45 | 3.36 | 0.14

Memory usage trends line chart as shown below:

![case2-Mem](./pic/case2-Mem.png)

Server CPU usage (percentage)

\         |  fibjs  |   nodejs  |    go   |   nginx  |
--------- | ------- | --------- | ------- | ---------
CPU Usage | 500~600 | (90~98)*8 | 200~400 | (30~35)*8

### Conclusion

Model

* Both fibjs and Go use single process with multi-thread model.
* Both nodejs(cluster) and Nginx use multi-process model.

Memory usage

* Go's memory usage is higher than the others which are 320KB per request.
* Both fibjs and nodejs need the same memory which is 72K per request. However, the memory usage of Node.js' base system is higher than fibjs'.
* Nginx adopts Zero Copy which requires very low memory usage.

QPS

* CPU Utilization of both Nginx and Go is much less than 100% and has excellent performance at high concurrency.
* Fibjs uses multi-thread model to handle the HTTP protocol which leads to high CPU Utilization than Nginx and Go. It also has good performance at high concurrency.
* CPU Utilization of Node.js's is almost 100% and performance declines quickly when the qps great than 3W.

### Related code

#### Server_code_1_fibjs
```js
var http = require('http'),
  net = require('net'),
  coroutine = require('coroutine');

var interval = 1000;

var hdlr = new http.Handler(function(req) {
  if (req.address == "/fibjs") {
    req.response.write('Hello, World!');
  }
})

var demon = function() {
  while (true) {
    console.error("connections:", svr.stats.connections,
      "\trequest:", hdlr.stats.request,
      "\tresponse:", hdlr.stats.response);
    hdlr.stats.reset();
    svr.stats.reset();
    coroutine.sleep(interval);
  }
}

var svr = new net.TcpServer(8080, hdlr);

coroutine.start(demon);
svr.run();
```

#### Server_code_2_nodejs_cluster
```js
var cluster = require('cluster'),
  http = require('http'),
  url = require('url'),
  numCPUs = require('os').cpus().length;

if (cluster.isMaster) {
  console.log("master start...");

  // Fork workers.
  for (var i = 0; i < numCPUs; i++) {
    cluster.fork();
  }

  cluster.on('listening', function(worker, address) {
    console.log('listening: worker ' + worker.process.pid + ', Address: ' + address.address + ":" + address.port);
  });

  cluster.on('exit', function(worker, code, signal) {
    console.log('worker ' + worker.process.pid + ' died');
  });
} else {
  http.createServer(function(req, res) {
    var pathname = url.parse(req.url).pathname;
    if ("/node" === pathname) {
      res.writeHead(200);
      res.end("Hello World!\n");
    }
  }).listen(8080);
}
```

#### Server_code_3_go
```go
package main
import (
    "net/http"
    "runtime"
    "log"
)

func hdlr_hello(rw http.ResponseWriter, req *http.Request) {
    req.ParseForm()  
    rw.Write([]byte("Hello world!"))
}

func main() {
    runtime.GOMAXPROCS(runtime.NumCPU())
    http.HandleFunc("/go", hdlr_hello) 
    err := http.ListenAndServe(":8083", nil)
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
        // panic(err)
    }
}
```

#### Server_code_4_nginx_module

```c
#include <ngx_config.h> 
#include <ngx_core.h> 
#include <ngx_http.h> 
#include <ngx_buf.h>

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf); 

static ngx_command_t ngx_http_hello_world_commands[]={ 
    { 
        ngx_string("hello_world"),          
        NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,  
        ngx_http_hello_world,               
        0,                                  
        0,                                  
        NULL                                
    }, 
    ngx_null_command  //读入ngx_null_command 指令后停止 
}; 

//ngx_http_<module name>_module_ctx用于创建和合并三个配置 
static ngx_http_module_t ngx_http_hello_world_module_ctx={ 
    NULL,   //preconfiguration 
    NULL,   //postconfiguration 
    NULL,   //create main configuration 
    NULL,   //init main configuration 
    NULL,   //create server configuration 
    NULL,   //merge server configuration 
    NULL,   //create location configuration 
    NULL    //merge localtion configuration 
};

//nginx进程，线程相关，ngx_http_<module name>_module把数据处理关联到特定模块
ngx_module_t ngx_http_hello_world_module={ 
    NGX_MODULE_V1, 
    &ngx_http_hello_world_module_ctx,   //module context 
    ngx_http_hello_world_commands,      //module directives 
    NGX_HTTP_MODULE,                    //module type 
    NULL,  //init master 
    NULL,  //init module 
    NULL,  //init process 
    NULL,  //init thread 
    NULL,  //exit thread 
    NULL,  //exit process 
    NULL,  //exit master 
    NGX_MODULE_V1_PADDING 
};

static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r) 
{
    if(!(r->method & NGX_HTTP_GET))
        return NGX_HTTP_NOT_ALLOWED;
 
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if(rc != NGX_OK)
        return rc;
 
    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response = ngx_string("Hello World!");
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;
 
    rc = ngx_http_send_header(r);
    if(rc == NGX_ERROR || rc > NGX_OK || r->header_only)
        return rc;
 
    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if(b == NULL)
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
 
    ngx_memcpy(b->pos, response.data, response.len);
    b->last = b->pos + response.len;
    b->last_buf = 1;
 
    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;
        
    return ngx_http_output_filter(r,&out); 
}


static char *ngx_http_hello_world(ngx_conf_t *cf,ngx_command_t *cmd,void *conf) 
{ 
    ngx_http_core_loc_conf_t *clcf; 

    clcf = ngx_http_conf_get_module_loc_conf(cf,ngx_http_core_module); 
    clcf->handler = ngx_http_hello_world_handler; 
    return NGX_CONF_OK; 
} 
```

### Script_cluster_schedule

```sh
#!/usr/bash

if [ $# -lt 1 ]; then
  echo ""
  echo "option [conn | close]"
  echo ""
  exit 0
fi

user=verdant
concurrency=50000
duration=600
logdir="log/"
logfile="ctrl.log."
ip_array=("171" "193" "209" "213" "231" "219" "222" "223" "224" "225")  
conn=$(($concurrency/${#ip_array[*]}))
sleeptime=1
# sleeptime=$(($concurrency/10000+1))

remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/fibjs"
# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/node"
# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/go" 
# remote_cmd="~/wrk -t10 -c$conn -d""$duration""s http://192.168.1.61:8080/nginx"

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

if [ $1 = 'conn' ]; then
  recovery  
fi

sleep $duration
echo "calculate..."
calculate $1
```
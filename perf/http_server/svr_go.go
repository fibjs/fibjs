package main
import (
    "net/http"
    "runtime"
    "log"
)

func hdlr_hello(rw http.ResponseWriter, req *http.Request) {
    req.ParseForm()  //解析参数
    rw.Write([]byte("Hello world!"))
}

func main() {
    runtime.GOMAXPROCS(runtime.NumCPU())
    http.HandleFunc("/go", hdlr_hello) //设置访问的路由
    err := http.ListenAndServe(":8080", nil) //设置监听的端口
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
        // panic(err)
    }
}
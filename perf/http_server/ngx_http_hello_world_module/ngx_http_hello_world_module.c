#include <ngx_config.h> 
#include <ngx_core.h> 
#include <ngx_http.h> 
#include <ngx_buf.h>

/*
 *cf   指向ngx_conf_t 结构体指针，从指令后面传过来的参数 
 *cmd  指向当前结构体ngx_command_t 的指针(互相指) 
 *conf 指向自定义模块配置结构体的指针 
 */
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf); 

static ngx_command_t ngx_http_hello_world_commands[]={ 
    { 
        ngx_string("hello_world"),          //指令名称，nginx.conf中使用 
        NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,  //注释1 
        ngx_http_hello_world,               //回调函数，上面申明 
        0,                                  //保持的值存放位置：全局，server，location 
        0,                                  //指令的值保存位置 
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
    NULL,	//init master 
    NULL,	//init module 
    NULL,	//init process 
    NULL,	//init thread 
    NULL,	//exit thread 
    NULL,	//exit process 
    NULL,	//exit master 
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

//回调函数，1获得location中的“核心”结构体，2为他分配个处理函数 
static char *ngx_http_hello_world(ngx_conf_t *cf,ngx_command_t *cmd,void *conf) 
{ 
    ngx_http_core_loc_conf_t *clcf; 

    clcf = ngx_http_conf_get_module_loc_conf(cf,ngx_http_core_module); 
    clcf->handler = ngx_http_hello_world_handler; 
    return NGX_CONF_OK; 
} 

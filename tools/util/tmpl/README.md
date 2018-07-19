# <%-title%>
<% var now_collect = '';
  for(var m in defs){
    if(defs[m].declare.type == type){
    if(now_collect !== defs[m].collect)
    {
        now_collect = defs[m].collect;
        if(now_collect){%>* <%-now_collect%>
<%}}
    if(now_collect){%>  -<%}else{%>*<%}%> [<%-defs[m].declare.name%>](ifs/<%-defs[m].declare.name%>.md) - <%-defs[m].declare.doc.descript%>
<%}}%>
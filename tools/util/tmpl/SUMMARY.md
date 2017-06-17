<% for(var m in defs){
    if(defs[m].declare.type == type){
    %>* [<%-defs[m].declare.name%>](ifs/<%-defs[m].declare.name%>.md)
<%}}%>
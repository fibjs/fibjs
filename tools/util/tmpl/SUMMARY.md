<% var keys = Object.keys(defs);
 keys.sort();
 keys.forEach(m => {
    if(defs[m].declare.type == type){
    %>* [<%-defs[m].declare.name%>](<%-prefix%>ifs/<%-defs[m].declare.name%>.md)
<%}})%>
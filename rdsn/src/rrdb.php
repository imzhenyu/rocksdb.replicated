<?php 
$_PROG = new t_program("rrdb");

$_PROG->namespaces["cpp"] = "dsn.apps";

$tmp = new t_program("dsn");
$_PROG->includes[$tmp->name] = $tmp;
$tmp->namespaces["cpp"] = "dsn";

$tmp = new t_struct($_PROG, "update_request");
$tmp->add_field("key", "dsn.blob");
$tmp->add_field("value", "dsn.blob");

$tmp = new t_service($_PROG, "rrdb");
$tmp2 = $tmp->add_function("rrdb.int", "put");
$tmp2->add_param("update", "rrdb.update_request");
$tmp2 = $tmp->add_function("rrdb.int", "remove");
$tmp2->add_param("key", "dsn.blob");
$tmp2 = $tmp->add_function("rrdb.int", "merge");
$tmp2->add_param("update", "rrdb.update_request");
$tmp2 = $tmp->add_function("dsn.blob", "get");
$tmp2->add_param("key", "dsn.blob");

?>

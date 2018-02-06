<?php

if (function_exists("tfbsql_test")){
  echo "function tfbsql_test exists\n";
  tfbsql_test();
}else{
  echo "ERROR: function tfbsql_test does not exists\n";
}



$link=null;
if (function_exists("fbsql_connect")){
  echo "function fbsql_connect exists\n";
  $link = fbsql_connect("192.168.178.136","_system","");
  if ($link===false){
    throw new \Exception("Error Processing Request fbsql_connect", 1);
  }
  fbsql_database_password($link,"");
  fbsql_database($link,"TUALO");
  fbsql_set_transaction($link,FBSQL_LOCK_PESSIMISTIC,FBSQL_ISO_READ_UNCOMMITTED);

  for($i=0;$i<1;$i++){
    $result = fbsql_db_query("TUALO","select * from testdata;",$link);
    while($row = fbsql_fetch_array($result)){
      echo $row[0].' '.$row[1]."\n";
    }
    fbsql_free_result($result);
  }

  fbsql_close($link);
  echo "\nDONE\n";
  exit();
}else{
  echo "ERROR: function fbsql_connect does not exists\n";
}

exit();

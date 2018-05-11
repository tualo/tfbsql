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
  $link = fbsql_connect("127.0.0.1","_system","");
  echo "1)\n";
  if ($link===false){
    throw new \Exception("Error Processing Request fbsql_connect", 1);
  }
  var_dump($link);
  echo "2)\n";
  fbsql_database_password($link,"");
  echo "3)\n";
  fbsql_database($link,"TUALO");
  echo "4)\n";
  fbsql_set_transaction($link,FBSQL_LOCK_PESSIMISTIC,FBSQL_ISO_READ_UNCOMMITTED);
  echo "5)\n";
  
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

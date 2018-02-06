# tfbsql

tfbsql is a not complete replacement of the missing frontbase module for php7.

## included functions

  * fbsql_connect
  * fbsql_database_password
  * fbsql_database
  * fbsql_set_transaction
  * fbsql_fetch_array
  * fbsql_free_result

## requirements

  To build this module you have to install php-cpp from http://www.php-cpp.com
  and FrontBase v8.2.9 from http://frontbase.com/

## install php-cpp

  git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
  cd PHP-CPP
  make
  make install


## build


  git clone https://github.com/tualo/tfbsql.git
  cd tfbsql
  make

## install

  make install

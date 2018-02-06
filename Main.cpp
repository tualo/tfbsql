#include "Main.h"



int mdOk(TFBSQLLinkWrapper* link, FBCMetaData* md, char* sql)
{
	//FBCDatabaseConnection* c = link->connection;
	int result = 1;
  link->errorNo = 0;
	if (link->errorText)
	{
    free(link->errorText);
    link->errorText = NULL;
  }
  if (md == NULL)
	{
		link->errorNo = 1;
		link->errorText = strdup("Connection to database server was lost");
    Php::warning << link->errorText<< std::flush;
		result = 0;
	}
  else if (fbcmdErrorsFound(md))
  {
    FBCErrorMetaData* emd = fbcmdErrorMetaData(md);
    char*             emg = fbcemdAllErrorMessages(emd);
    if (emg)
      Php::warning << "Error in statement: " << sql << " " << strdup(emg) << std::flush;
    else
      Php::warning << "No message" << std::flush;

    link->errorText = strdup(emg);
    link->errorNo = fbcemdErrorCodeAtIndex(emd, 0);
    //free(emg);
    fbcemdRelease(emd);

    result = 0;
  }
  else if (fbcmdWarningsFound(md))
  {
    FBCErrorMetaData* emd = fbcmdErrorMetaData(md);
    char*             emg = fbcemdAllErrorMessages(emd);
    if (emg)
      Php::warning << "Error in statement: " << sql << std::flush;
    else
      Php::warning << "No message" << std::flush;

    link->errorText = strdup(emg);
    link->errorNo = fbcemdErrorCodeAtIndex(emd, 0);
    free(emg);
    fbcemdRelease(emd);
    result = 1;
  }
	return result;
}


void tfbsql_test()
{
    Php::out << "tfbsql_test version 1.0" << std::endl;
}


Php::Value fbsql_connect(Php::Parameters &parameters){

  TFBSQLLinkWrapper* lw = new TFBSQLLinkWrapper();
  if (parameters.size()>0){
    lw->hostName         = parameters[0].stringValue();
  }else{
    lw->hostName         = std::string( "localhost" );
  }
  if (parameters.size()>1){
    lw->userName         = parameters[1].stringValue();
  }else{
    lw->userName         = std::string( "_SYSTEM" );
  }
  if (parameters.size()>2){
    lw->userPassword         = parameters[2].stringValue();
  }else{
    lw->userPassword         = std::string( "" );
  }

  if (TFBSQL_DEBUG)
  Php::out << "fbsql_connect " << std::endl
    << " lw->hostName " << lw->hostName << std::endl
    << " lw->userName " << lw->userName << std::endl
    << " lw->userPassword " << lw->userPassword << std::endl;



  return Php::Object("TFBSQLLinkWrapper", lw);
}


Php::Value fbsql_database(Php::Parameters &parameters){

  Php::Value linkValue=parameters[0];
  TFBSQLLinkWrapper* lw=(TFBSQLLinkWrapper *)linkValue.implementation();

  if (parameters.size()>1){
    lw->databaseName         = parameters[1].stringValue();
  }

  if (TFBSQL_DEBUG)
  Php::out << "fbsql_connect " << std::endl
    << " lw->hostName " << lw->hostName << std::endl
    << " lw->userName " << lw->userName << std::endl
    << " lw->userPassword " << lw->userPassword << std::endl
    << " lw->databaseName " << lw->databaseName << std::endl;

  lw->open();

  return lw->databaseName;
}


Php::Value fbsql_database_password(Php::Parameters &parameters){
  Php::Value linkValue=parameters[0];
  TFBSQLLinkWrapper* lw=(TFBSQLLinkWrapper *)linkValue.implementation();

  if (parameters.size()>1){
    lw->databasePassword         = parameters[1].stringValue();
  }else{

  }
  return lw->databasePassword;
}


void fbsql_set_transaction(Php::Parameters &parameters){
  Php::Value linkValue=parameters[0];

  int Locking =parameters[1];
  int Isolation =parameters[2];

  TFBSQLLinkWrapper* lw=(TFBSQLLinkWrapper *)linkValue.implementation();

  char strSQL[1024];
  char *strLocking[] = {(char*)"DEFERRED", (char*)"OPTIMISTIC", (char*)"PESSIMISTIC"};
  char *strIsolation[] = {(char*)"READ UNCOMMITTED", (char*)"READ NCOMMITTED", (char*)"REPEATABLE READ", (char*)"SERIALIZABLE", (char*)"VERSIONED"};


  if (  Locking < 0 || Locking > 2) {
    throw Php::Exception("Invalid locking type.");
    return;
  }
  if ( Isolation < 0 || Isolation > 4) {
    throw Php::Exception("Invalid isolation type.");
    return;
  }


  if (snprintf(strSQL, sizeof(strSQL) , (char*)"SET TRANSACTION LOCKING %s, ISOLATION %s;", strLocking[ Locking ], strIsolation[ Isolation ]) < 0) {
    return;
  }
  lw->query(std::string(strSQL));
}


Php::Value fbsql_db_query(Php::Parameters &parameters){
  std::string dnname = parameters[0];
  std::string command = parameters[1];
  Php::Value linkValue=parameters[2];
  TFBSQLLinkWrapper* lw=(TFBSQLLinkWrapper *)linkValue.implementation();



  FBCMetaData*   md, *meta;
	char*          tp;
	char*          fh = NULL;
	unsigned int   sR = 1, cR = 0;

  //Php::out <<  (char*)command.c_str() << std::endl;
  meta = fbcdcExecuteDirectSQL(lw->connection, (char*)command.c_str() );
  //meta = fbcdcExecuteDirectSQLWithLength(lw->connection, (char*)command.c_str(), sizeof((char*)command.c_str()) );

  if (TFBSQL_DEBUG)
  Php::out << "query executed " << std::endl;

	if (!mdOk(lw, meta, (char*)command.c_str()))
	{
		fbcmdRelease(meta);
    if (TFBSQL_DEBUG)
    Php::out << "query executed  FAILED " << std::endl;
    return false;
	}
	else {
		if (fbcmdHasMetaDataArray(meta)) {
			sR = fbcmdMetaDataArrayCount(meta);
			md = (FBCMetaData*)fbcmdMetaDataAtIndex(meta, cR);
		}
		else
			md = meta;


    if (TFBSQL_DEBUG)
    Php::out << "query executed successfully " << std::endl;

		tp = fbcmdStatementType(md);
		if (tp == NULL) {
			fbcmdRelease(meta);
      return true;
		}
		else if ((tp[0] == 'C') || (tp[0] == 'R'))
		{
			if (sR == 1 && md) fbcmdRelease(md);
			return true;
		}
		else if ((fh = fbcmdFetchHandle(md)) || tp[0] == 'E' || (tp[0] == 'U' && fh))
		{

      TFBSQLResultWrapper* rw = new TFBSQLResultWrapper();
      rw->lw = lw;
      rw->resultmetaData = meta;
      rw->metaData = md;
			rw->selectResults = sR;
			rw->currentResult = cR;


      if (tp[0] != 'E')
			{
        if (TFBSQL_DEBUG)
        Php::out << "columnCount " << fbcmdColumnCount(md) << std::endl;
        if (TFBSQL_DEBUG)
        Php::out << "fetchHandle " << fh << std::endl;
        if (TFBSQL_DEBUG)
        Php::out << "tp " << tp << std::endl;

        rw->columnCount = fbcmdColumnCount(md);
        rw->fetchHandle = fh;
        rw->rowCount = fbcmdRowCount(md);

        if (TFBSQL_DEBUG)
        Php::out << "rowCount " << rw->rowCount << std::endl;

			}
			else
			{
        char* r = fbcmdMessage(md);
        if (TFBSQL_DEBUG)
        Php::out << "fbcmdMessage " << r << std::endl;
        if ((rw->list = fbcplParse(r)))
        {
          rw->rowCount = fbcplCount(rw->list);
          rw->columnCount = 7;
        }
      }
      return Php::Object("TFBSQLResultWrapper", rw);

		}
		else if (tp[0] == 'I' || tp[0] == 'U')
		{
			if (tp[0] == 'I') lw->insert_id = fbcmdRowIndex(md);
			if (sR == 1 && md) fbcmdRelease(md);
			return true;
		}
		else if(tp[0] == 'A' || tp[0] == 'D')
		{
			if (sR == 1 && md) fbcmdRelease(md);
			return true;
		}
		if (lw->connected) lw->affectedRows = fbcmdRowCount(md);
	}

  return false;
}

Php::Value fbsql_free_result(Php::Parameters &parameters){
  Php::Value resultValue=parameters[0];
  TFBSQLResultWrapper* rw=(TFBSQLResultWrapper *)resultValue.implementation();
  if (rw->resultmetaData){
    fbcmdRelease(rw->resultmetaData);
  }

  return true;
}

Php::Value fbsql_fetch_array(Php::Parameters &parameters){
  Php::Array return_value;
  Php::Value resultValue=parameters[0];
  int result_type = FBSQL_NUM;
  int i;
  if (parameters.size()>1){
    result_type = parameters[1];
  }
  TFBSQLResultWrapper* rw=(TFBSQLResultWrapper *)resultValue.implementation();

  //int rowIndex = rw->rowIndex;
  if (((result_type & FBSQL_NUM) != FBSQL_NUM) && ((result_type & FBSQL_ASSOC) != FBSQL_ASSOC))
  {
    Php::warning << (char*)"Illegal result type use FBSQL_NUM, FBSQL_ASSOC, or FBSQL_BOTH" << std::flush;
    return false;
  }


  FBCRow* row = fbcmdFetchRow(rw->resultmetaData);
  if (row==NULL){
    return false;
  }else{
    for (i=0; i<rw->columnCount; i++){

      if (row[i] == NULL) {
        return_value[i] = nullptr;
      }else{

        const FBCDatatypeMetaData *dtmd = fbcmdDatatypeMetaDataAtIndex(rw->resultmetaData, i);
        unsigned dtc = fbcdmdDatatypeCode(dtmd);
        if ( (dtc==FB_Character)
          || (dtc==FB_VCharacter)
          || (dtc==FB_Date)
          || (dtc==FB_Time)
          || (dtc==FB_TimeTZ)
          || (dtc==FB_Timestamp)
          || (dtc==FB_TimestampTZ)
        ){
          return_value[i] = std::string( (char*)row[i] );
        }else if ( (dtc==FB_PrimaryKey) || (dtc==FB_Integer) ){
          return_value[i] = (int)row[i]->integer;
        }else if ( (dtc==FB_TinyInteger) ){
          return_value[i] = (char)row[i]->tinyInteger;
        }else if ( (dtc==FB_SmallInteger) ){
          return_value[i] = (short)row[i]->shortInteger;
        }else if ( (dtc==FB_LongInteger) ){
          return_value[i] = (int)row[i]->longInteger;
        }else if ( (dtc==FB_Boolean) ){
          switch(row[i]->boolean) {
             case 0:  return_value[i] = false;  break;
             case 1:  return_value[i] = true;   break;
             default: return_value[i] = nullptr;    break;
          }
        }else if ( (dtc==FB_Numeric)
          || (dtc==FB_Decimal)
          || (dtc==FB_Float)
          || (dtc==FB_Real)
          || (dtc==FB_Double)
        ){
          return_value[i] = (double)row[i]->numeric;
        }else{
          return_value[i] = nullptr;
        }
      }

    }
  }

  return return_value;
}


Php::Value fbsql_close(Php::Parameters &parameters){
  Php::Value linkValue=parameters[0];
  TFBSQLLinkWrapper* lw=(TFBSQLLinkWrapper *)linkValue.implementation();
  //lw->get();
  lw->close();
  return true;
}

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module()
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("tfbsql", "1.0");


        Php::Class<TFBSQLLinkWrapper> linkWrapper("TFBSQLLinkWrapper");
        extension.add(std::move(linkWrapper));

        Php::Class<TFBSQLResultWrapper> resultWrapper("TFBSQLResultWrapper");
        extension.add(std::move(resultWrapper));

        extension.add<tfbsql_test>("tfbsql_test");

        extension.add<fbsql_connect>("fbsql_connect");


        extension.add<fbsql_database>("fbsql_database", {
          Php::ByRef("link",  "TFBSQLLinkWrapper" )
        });
        extension.add<fbsql_database_password>("fbsql_database_password", {
          Php::ByRef("link",  "TFBSQLLinkWrapper" )
        });

        extension.add<fbsql_set_transaction>("fbsql_set_transaction", {
          Php::ByRef("link",  "TFBSQLLinkWrapper" ),
          Php::ByVal("locking",  Php::Type::Numeric ),
          Php::ByVal("isolation",  Php::Type::Numeric )
        });

        extension.add<fbsql_db_query>("fbsql_db_query", {
          Php::ByVal("dbname",  Php::Type::String ),
          Php::ByVal("query",  Php::Type::String ),
          Php::ByRef("link",  "TFBSQLLinkWrapper" )
        });

        extension.add<fbsql_free_result>("fbsql_free_result", {
          Php::ByRef("link",  "TFBSQLResultWrapper" )
        });

        extension.add<fbsql_fetch_array>("fbsql_fetch_array", {
          Php::ByRef("link",  "TFBSQLResultWrapper" )
        });



        extension.add<fbsql_close>("fbsql_close", {
          Php::ByVal("link",  "TFBSQLLinkWrapper" )
        });


        extension.add(Php::Constant("FBSQL_ASSOC", FBSQL_ASSOC));
        extension.add(Php::Constant("FBSQL_NUM", FBSQL_NUM));
        extension.add(Php::Constant("FBSQL_BOTH", FBSQL_BOTH));

        extension.add(Php::Constant("FBSQL_LOCK_DEFERRED", FBSQL_LOCK_DEFERRED));
        extension.add(Php::Constant("FBSQL_LOCK_OPTIMISTIC", FBSQL_LOCK_OPTIMISTIC));
        extension.add(Php::Constant("FBSQL_LOCK_PESSIMISTIC", FBSQL_LOCK_PESSIMISTIC));

        extension.add(Php::Constant("FBSQL_ISO_READ_UNCOMMITTED", FBSQL_ISO_READ_UNCOMMITTED));
        extension.add(Php::Constant("FBSQL_ISO_READ_COMMITTED", FBSQL_ISO_READ_COMMITTED));
        extension.add(Php::Constant("FBSQL_ISO_REPEATABLE_READ", FBSQL_ISO_REPEATABLE_READ));
        extension.add(Php::Constant("FBSQL_ISO_SERIALIZABLE", FBSQL_ISO_SERIALIZABLE));
        extension.add(Php::Constant("FBSQL_ISO_VERSIONED", FBSQL_ISO_VERSIONED));

        extension.add(Php::Constant("FBSQL_LOB_DIRECT", FBSQL_LOB_DIRECT));
        extension.add(Php::Constant("FBSQL_LOB_HANDLE", FBSQL_LOB_HANDLE));

        extension.add(Php::Constant("DIGEST_BUFFER_SIZE", DIGEST_BUFFER_SIZE));

        // @todo    add your own functions, classes, namespaces to the extension

        // return the extension
        return extension;
    }
}

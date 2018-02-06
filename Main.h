#include <phpcpp.h>
#include <iostream>


#include <FBCAccess/FBCAccess.h>

#include <php.h>

#define TFBSQL_DEBUG 0

#define FBSQL_ASSOC		1<<0
#define FBSQL_NUM		1<<1
#define FBSQL_BOTH		(FBSQL_ASSOC|FBSQL_NUM)

#define FBSQL_LOCK_DEFERRED 0
#define FBSQL_LOCK_OPTIMISTIC 1
#define FBSQL_LOCK_PESSIMISTIC 2		/* default */

#define FBSQL_ISO_READ_UNCOMMITTED 0
#define FBSQL_ISO_READ_COMMITTED 1
#define FBSQL_ISO_REPEATABLE_READ 2
#define FBSQL_ISO_SERIALIZABLE 3		/* default */
#define FBSQL_ISO_VERSIONED 4

#define FBSQL_LOB_DIRECT 0				/* default */
#define FBSQL_LOB_HANDLE 1				/* default */

#define DIGEST_BUFFER_SIZE 17			/* fbcDigestPassword() expects a preallocated buffer for 16 bytes plus termination */



class TFBSQLLinkWrapper : public Php::Base
{
private:


public:

  FBCDatabaseConnection* connection;
    bool connected;
    bool autoCommit;
    int affectedRows;
    int insert_id;
    int errorNo;
    char* errorText;

    TFBSQLLinkWrapper() {
      connected = false;

      userPassword="";
      hostName="localhost";
      userName="_system";
      databasePassword="";
      databaseName="";


      errorNo=0;

    }
    virtual ~TFBSQLLinkWrapper() = default;
    std::string hostName;
    std::string userName;
    std::string userPassword;
    std::string databasePassword;
    std::string databaseName;


    void close(){
      if (connected){
        if (fbcdcConnected(connection)){
          fbcdcClose(connection);
        }
        fbcdcRelease(connection);
      }
    }

    void query(std::string sql){
      FBCMetaData* md;
      md = fbcdcExecuteDirectSQL( connection, (char* )sql.c_str() );
      fbcmdRelease(md);
    }

    void  open(){

    	FBCMetaData* md;

      if (!connected){
        int port = atoi(databaseName.c_str());
        if (port>0 && port<65535) // strange behavior from old fbsql module!
          connection = fbcdcConnectToDatabaseUsingPort( hostName.c_str(), port,  databasePassword.c_str());
        else
          connection = fbcdcConnectToDatabase(databaseName.c_str(), hostName.c_str(), databasePassword.c_str());

        if (connection == NULL)
        {
          Php::warning << "php_fbsql_select_db >>> " << fbcdcClassErrorMessage() << std::flush;
          connected = false;
        }else{

          md = fbcdcCreateSession(connection, (char*)"PHP", userName.c_str(), userPassword.c_str(), userName.c_str());
      		if (fbcmdErrorsFound(md))
      		{
            Php::warning << fbcdcClassErrorMessage() << std::flush;
      			fbcmdRelease(md);
      			fbcdcClose(connection);
      			fbcdcRelease(connection);

            connected = false;
            return;

      		}
      		fbcmdRelease(md);

          if (connection)
      		{
      			if (autoCommit)
      				md = fbcdcExecuteDirectSQL(connection, (char*)"SET COMMIT TRUE;");
      			else
      				md = fbcdcExecuteDirectSQL(connection, (char*)"SET COMMIT FALSE;");
      			fbcmdRelease(md);
      		}

          connected = true;
        }
      }

    }

};


class TFBSQLResultWrapper : public Php::Base
{
private:

public:

    TFBSQLResultWrapper(){
      rowHandler  = NULL;
      fetchHandle = NULL;
      resultmetaData    = NULL;
      metaData    = NULL;
      batchSize   = 10000;//batch_size > 0 ? batch_size : FB_SQL_G(batchSize);
      rowCount    = 0x7fffffff;
      columnCount = 0;
      rowIndex    = 0;
      columnIndex = 0;
      row         = NULL;
      array       = NULL;
      list        = NULL;
      selectResults = 0;
      currentResult = 0;

    }
    virtual ~TFBSQLResultWrapper() = default;
    TFBSQLLinkWrapper* lw;



    FBCRow* rowHandler;
    char* fetchHandle;
    FBCMetaData* resultmetaData;
    FBCMetaData* metaData;
    unsigned int batchSize;
    unsigned int rowCount;
    int columnCount;
    unsigned int rowIndex;
    int columnIndex;
    void** row;
    FBCPList* array;
    FBCPList* list;
    unsigned int selectResults;
    unsigned int currentResult;


};

#ifndef _PostgreSQL_PostgeSQL_h_
#define _PostgreSQL_PostgeSQL_h_

#ifndef flagNOPOSTGRESQL

#include <Sql/Sql.h>
#include <libpq-fe.h>

// debian: sudo apt-get install libpq-dev

namespace Upp {

// Postgre -> Value types
// Bytea_v values are stored as bytea data, but recived as string type
const char *PostgreSQLReadString(const char *s, String& stmt);


// Deprecated, use SqlPerformScript instead
bool PostgreSQLPerformScript(const String& text,
#ifdef NOAPPSQL
	StatementExecutor& se,
#else
	StatementExecutor& se = SQLStatementExecutor(),
#endif
	Gate<int, int> progress_canceled = Null
);

struct PGInfo : Moveable<PGInfo> {
    String pgDatabase;
    String pgUser;
    String pgHost;
    String pgHostAddr;
    String pgPort;
    int pgServerVersion = 0;
    bool pgSSL = false;
    String pgSslLibrary;
    String pgSslProtocol;
    String pgSslKeyBits;
    String pgSslCipher;
    String pgSslCompression;

    void Clear() {
        pgDatabase.Clear();
        pgUser.Clear();
        pgHost.Clear();
        pgHostAddr.Clear();
        pgPort.Clear();
        pgServerVersion = 0;
        pgSSL = false;
        pgSslLibrary.Clear();
        pgSslProtocol.Clear();
        pgSslKeyBits.Clear();
        pgSslCipher.Clear();
        pgSslCompression.Clear();
    }
};

String PostgreSQLTextType(int n);

class PostgreSQLConnection;

class PostgreSQLSession : public SqlSession {
public:
	virtual bool                  IsOpen() const                   { return conn; }
	virtual RunScript             GetRunScript() const             { return &PostgreSQLPerformScript; }

	virtual Vector<String>        EnumUsers();
	virtual Vector<String>        EnumDatabases();
	virtual Vector<String>        EnumTables(String schema);
	virtual Vector<String>        EnumViews(String schema);
	virtual Vector<String>        EnumSequences(String schema);
	virtual Vector<SqlColumnInfo> EnumColumns(String database, String table);
	virtual Vector<String>        EnumPrimaryKey(String database, String table);
	virtual String                EnumRowID(String database, String table);
	virtual Vector<String>        EnumReservedWords();

protected:
	virtual SqlConnection *CreateConnection();

private:
	PGconn               *conn;
	PGresult             *result;

    PGInfo                pginfo;

	String                conns;
	bool                  keepalive;
	bool                  hex_blobs;
	bool                  noquestionparams = false;

	VectorMap<String, String> pkache;

	void                  ExecTrans(const char * statement);
	Vector<String>        EnumData(char type, const char *schema = nullptr);
	String                ErrorMessage(const String& msg) const { return FromCharset(msg); }
	int                   level;
	byte                  charset;

	String                FromCharset(const String& s) const;
	String                ToCharset(const String& s) const;

	void                  DoKeepAlive();

	friend class PostgreSQLConnection;

public:
	Gate1<int>            WhenReconnect;

	bool                  Open(const char *connect);
	bool                  ConnectionOK();
	bool                  ReOpen();
	void                  Close();

	void                  SetCharset(byte chrset)         { charset = chrset; }
	void                  KeepAlive(bool b = true)        { keepalive = b; DoKeepAlive(); }
	void                  NoQuestionParams(bool b = true) { noquestionparams = b; }

	String                GetUser()                       { return PQuser(conn); }
	operator PGconn *     ()                              { return conn; }

	virtual void          Begin();
	virtual void          Commit();
	virtual void          Rollback();
	virtual int           GetTransactionLevel() const;

	PostgreSQLSession()                                   { conn = nullptr; Dialect(PGSQL); level = 0; keepalive = hex_blobs = false; }
	~PostgreSQLSession()                                  { Close(); }
	PGconn * GetPGConn()                                  { return conn; }

    PGInfo GetPGInfo()                                    { return pginfo; }
};

class PgSequence : public ValueGen {
	SqlId       ssq;
	SqlId&      seq;
	SqlSession *session;

public:
	virtual Value  Get();

	Value operator++()                                                  { return Get(); }

	void Set(SqlId id, SqlSession& s)                                   { ssq = id; session = &s; }

#ifndef NOAPPSQL
	void Set(SqlId id)                                                  { ssq = id; session = nullptr; }
	PgSequence(const char *name) : ssq(name), seq(ssq)                  { session = nullptr; }
	PgSequence(SqlId& seq) : seq(seq)                                   { session = nullptr; }
#endif
	PgSequence(const char *name, SqlSession& s) : ssq(name), seq(ssq)   { session = &s; }
	PgSequence(SqlId& seq, SqlSession& s) : seq(seq)                    { session = &s; }
	PgSequence() : seq(ssq)                                             { session =  nullptr; }
};

}

#endif

#endif

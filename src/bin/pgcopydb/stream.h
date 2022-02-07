/*
 * src/bin/pgcopydb/stream.h
 *	 SQL queries to discover the source database stream
 */

#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>

#include "copydb.h"
#include "pgsql.h"

typedef enum
{
	STREAM_ACTION_BEGIN = 'B',
	STREAM_ACTION_COMMIT = 'C',
	STREAM_ACTION_INSERT = 'I',
	STREAM_ACTION_UPDATE = 'U',
	STREAM_ACTION_DELETE = 'D',
	STREAM_ACTION_TRUNCATE = 'T'
} StreamAction;

typedef struct StreamCounters
{
	uint64_t total;
	uint64_t begin;
	uint64_t commit;
	uint64_t insert;
	uint64_t update;
	uint64_t delete;
	uint64_t truncate;
} StreamCounters;

typedef struct StreamContext
{
	CopyFilePaths *cfPaths;

	uint64_t startLSN;
	char walFileName[MAXPGPATH];
	FILE *jsonFile;

	StreamCounters counters;
} StreamContext;

typedef struct LogicalMessageMetadata
{
	uint32_t xid;
	char lsn[PG_LSN_MAXLENGTH];
	StreamAction action;
} LogicalMessageMetadata;


typedef struct StreamSpecs
{
	CopyFilePaths cfPaths;
	PostgresPaths pgPaths;

	char source_pguri[MAXCONNINFO];
	char logrep_pguri[MAXCONNINFO];
	char target_pguri[MAXCONNINFO];

	TransactionSnapshot sourceSnapshot;
	char slotName[NAMEDATALEN];
	uint64_t startLSN;

	bool restart;
	bool resume;
} StreamSpecs;

bool stream_init_specs(CopyDataSpec *copySpecs, StreamSpecs *specs, char *slotName);

bool startLogicalStreaming(StreamSpecs *specs);

bool streamToFiles(LogicalStreamContext *context);

bool parseMessageMetadata(LogicalMessageMetadata *metadata, const char *buffer);

bool buildReplicationURI(const char *pguri, char *repl_pguri);

#endif /* STREAM_H */

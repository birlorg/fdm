/* $Id$ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef DB

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <string.h>
#include <tdb.h>

#include "fdm.h"

int	db_item(TDB_CONTEXT *, TDB_DATA, TDB_DATA, void *);

struct db *
db_open(char *path)
{
	struct db	*db;
	TDB_CONTEXT	*tdb;

#ifndef DB_UNSAFE
	tdb = tdb_open(path, 0, 0, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
#else
	tdb = tdb_open(path, 0, TDB_NOLOCK, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
#endif
	if (tdb == NULL)
		return (NULL);

	db = xcalloc(1, sizeof *db);
	db->tdb = tdb;
	return (db);
}

void
db_close(struct db *db)
{
	tdb_close(db->tdb);
	xfree(db);
}

int
db_add(struct db *db, char *k)
{
	TDB_DATA	key, value;
	struct dbitem	v;
	uint64_t	tim;

	memset(&v, 0, sizeof v);
	tim = time(NULL);
	v.tim = htole64(tim);

	key.dptr = k;
	key.dsize = strlen(k);

	value.dptr = (char *) &v;
	value.dsize = sizeof v;

	return (tdb_store(db->tdb, key, value, TDB_REPLACE));
}

int
db_contains(struct db *db, char *k)
{
	TDB_DATA	key;

	key.dptr = k;
	key.dsize = strlen(k);

	return (tdb_exists(db->tdb, key));
}

int
db_size(struct db *db)
{
	return (tdb_traverse(db->tdb, NULL, NULL));
}

int
db_item(TDB_CONTEXT *tdb, TDB_DATA key, TDB_DATA value, void *ptr)
{
	uint64_t	*lim = ptr;
	struct dbitem	 v;

 	if (value.dsize != sizeof v)
		return (-1);
	memcpy(&v, value.dptr, sizeof v);

	if (letoh64(v.tim) < *lim)
		return (tdb_delete(tdb, key));
	return (0);
}

int
db_expire(struct db *db, uint64_t age)
{
	uint64_t	lim;

	lim = time(NULL);
	if (lim <= age)
		return (0);
	lim -= age;

	if (tdb_traverse(db->tdb, db_item, &lim) == -1)
		return (-1);
	return (0);
}

#endif /* DB */
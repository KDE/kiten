// ksaver.cpp
//
// Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <klocale.h>
#include <kio/netaccess.h>

#include "kloader.h"

class KLoader::KLoaderPrivate
{
public:
	KLoaderPrivate() : isLocal(true), file(0), textStream(0), dataStream(0) {}
    ~KLoaderPrivate()
        {
            delete file;
            delete textStream;
            delete dataStream;
        }
	bool isLocal;
	QString tempFile;
	QFile *file;
	KURL url;
	QString error;
	QTextStream *textStream;
	QDataStream *dataStream;
};

KLoader::KLoader(const KURL &target)
{
	d = new KLoaderPrivate;
	d->url = target;

	if(d->url.protocol() == "file")
	{
		d->isLocal = true;
		d->file = new QFile(d->url.path());
	}
	else
	{
		d->isLocal = false;
	}
}

KLoader::~KLoader()
{
	close();
	delete d;
}

bool KLoader::open(void)
{
	if(d->isLocal)
	{
		if(!d->file->open(IO_ReadOnly))
		{
			d->error = i18n("Could not read from %1.").arg(d->url.prettyURL());
			return false;
		}
	}
	else
	{
		if(!KIO::NetAccess::download(d->url, d->tempFile))
		{
			d->error = i18n("Could not read from %1.").arg(d->url.prettyURL());
			return false;
		}

		d->file = new QFile(d->tempFile);
		if(!d->file->open(IO_ReadOnly))
		{
			d->error = i18n("Could not read from %1.").arg(d->tempFile);
			return false;
		}
	}
	return true;
}

void KLoader::close(void)
{
	d->textStream = 0;
	d->dataStream = 0;

	delete d->file;
	d->file = 0;

	if(!d->isLocal)
		KIO::NetAccess::removeTempFile(d->tempFile);
}

QString KLoader::error(void)
{
	return d->error;
}

QFile &KLoader::file(void)
{
	if(d->file)
		return *d->file;
	else
		return *static_cast<QFile *>(0);
}

QTextStream &KLoader::textStream()
{
	if(d->textStream)
	{
		return *d->textStream;
	}
	else if(d->file)
	{
		d->textStream = new QTextStream(d->file);
		return *d->textStream;
	}
	else
	{
		return *static_cast<QTextStream *>(0);
	}
}

QDataStream &KLoader::dataStream()
{
	if(d->dataStream)
	{
		return *d->dataStream;
	}
	else if(d->file)
	{
		d->dataStream = new QDataStream(d->file);
		return *d->dataStream;
	}
	else
	{
		return *static_cast<QDataStream *>(0);
	}
}

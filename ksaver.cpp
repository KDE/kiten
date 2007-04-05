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
#include <ktempfile.h>
#include <kio/netaccess.h>

#include "ksaver.h"

class KSaver::KSaverPrivate
{
public:
	KSaverPrivate() : isLocal(true), tempFile(0), file(0), textStream(0), dataStream(0) {}
    ~KSaverPrivate()
        {
            delete file;
            delete textStream;
            delete dataStream;
            delete tempFile;
        }
	bool isLocal;
	KTempFile *tempFile;
	QFile *file;
	KURL url;
	QString error;
	QTextStream *textStream;
	QDataStream *dataStream;
};

KSaver::KSaver(const KURL &_target)
{
	d = new KSaverPrivate;
	d->url = _target;

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

KSaver::~KSaver()
{
	close();
	delete d;
}

bool KSaver::open(void)
{
	if(d->isLocal)
	{
		if(d->file->open(IO_WriteOnly))
		{
			return true;
		}
		else
		{
			d->error = i18n("Could not write to %1.").arg(d->url.prettyURL());
			return false;
		}
	}
	else
	{
		d->tempFile = new KTempFile;
		return true;
	}
}

bool KSaver::close(void)
{
	if(d->textStream)
		d->textStream = 0;

	if(d->dataStream)
		d->dataStream = 0;

	if(d->isLocal)
	{
		if(!d->file) return true;

		delete d->file;
		d->file = 0;
		return true;
	}
	else
	{
		if(!d->tempFile) return true;

		d->tempFile->close();

		bool retval = KIO::NetAccess::upload(d->tempFile->name(), d->url, 0);

		delete d->tempFile;
		d->tempFile = 0;

		return retval;
	}
}

QString KSaver::error(void)
{
	return d->error;
}

QFile &KSaver::file(void)
{
	if(d->isLocal && d->file)
		return *d->file;
	else if(!d->isLocal && d->tempFile)
		return *d->tempFile->file();
	else
		return *static_cast<QFile *>(0);
}

QTextStream &KSaver::textStream()
{
	if(d->textStream)
	{
		return *d->textStream;
	}
	else if(d->isLocal && d->file)
	{
		d->textStream = new QTextStream(d->file);
		return *d->textStream;
	}
	else if(!d->isLocal && d->tempFile)
	{
		d->textStream = d->tempFile->textStream();
		return *d->textStream;
	}
	else
	{
		return *static_cast<QTextStream *>(0);
	}
}

QDataStream &KSaver::dataStream()
{
	if(d->dataStream)
	{
		return *d->dataStream;
	}
	else if(d->isLocal && d->file)
	{
		d->dataStream = new QDataStream(d->file);
		return *d->dataStream;
	}
	else if(!d->isLocal && d->tempFile)
	{
		d->dataStream = d->tempFile->dataStream();
		return *d->dataStream;
	}
	else
	{
		return *static_cast<QDataStream *>(0);
	}
}

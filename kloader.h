// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
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

#ifndef KLOADER_H
#define KLOADER_H

#include <qfile.h>

/**
 * KLoader provides a way to load files in the most efficient way for
 * both local and remote URLs.
 */
class KLoader
{
public:
	/**
	 * The constructor takes the url and decides the best way to save,
	 * which will mean using something like KIO::NetAccess or QFile.
	 */
	KLoader(const KURL &_target);

	/**
	 * The destructor closes if necessary.
	 */
	~KLoader();

	/**
	 * open actually tries to open the file
	 *
	 * true on success, false on failure (get the error in @ref error)
	 */
	bool open(void);

	/**
	 * close closes the file.
	 */
	void close(void);

	/**
	 * If open or close returns false, there was an error, and error
	 * returns what the error was, when available.
	 */
	QString error(void);

	/**
	 * file returns a QFile open for reading, which may be for a temporary
	 * file on the local filesystem.
	 *
	 * If this is called before the file is opened, you will crash.
	 */
	QFile &file(void);

	/**
	 * You can use this to read in your data.
	 *
	 * If this is called before the file is opened, you will crash.
	 */
	QTextStream &textStream(void);

	/**
	 * You can use this to read in your data.
	 *
	 * If this is called before the file is opened, you will crash.
	 */
	QDataStream &dataStream(void);
private:
	class KLoaderPrivate;
	KLoaderPrivate *d;
};

#endif

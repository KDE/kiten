/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2006  Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef ENTRYEDICT_H_
#define ENTRYEDICT_H_

#include "Entry.h"
#include <QStringList>

class QString;
class DictQuery;

class /* NO_EXPORT */ EntryEDICT : public Entry {
	public:
//		EntryEDICT(const EntryEDICT &x) : Entry(x) {} //No special members to copy in this one
		EntryEDICT(const QString &dict) : Entry(dict), common(false){}
		EntryEDICT(const QString &dict, const QString &entry) : Entry(dict) {loadEntry(entry);}
		Entry *clone() const { return new EntryEDICT(*this); }

		virtual QString toHTML(printType=printAuto) const;

		virtual bool loadEntry(const QString &);
		virtual QString dumpEntry() const;

		virtual inline QString HTMLWord() const;
		virtual inline QString Common() const;

	protected:
		virtual inline QString kanjiLinkify(const QString &) const;

		/* EDICT supports the following word types */
		enum WordType
		{
			adj = 0 , /*	adjective (keiyoushi) */
			adj_na, /*	adjectival nouns or quasi-adjectives (keiyodoshi) */
			adj_no, /*	nouns which may take the genitive case particle `no' */
			adj_pn, /*	pre-noun adjectival (rentaishi) */
			adj_t, /*	`taru' adjective */
			adv, /*	adverb (fukushi) */
			adv_n, /*	adverbial noun */
			adv_to, /*	adverb taking the `to' particle */
			aux, /*	auxiliary */
			aux_v, /*	auxiliary verb */
			aux_adj, /*	auxiliary adjective */
			conj, /*	conjunction */
			exp, /*	Expressions (phrases, clauses, etc.) */
			id, /*	idiomatic expression */
			inte, /*	interjection (kandoushi)
						int is a keyword, thus 'inte' instead */
			iv, /*	irregular verb */
			n, /*	noun (common) (futsuumeishi) */
			n_adv, /*	adverbial noun (fukushitekimeishi) */
			n_pref, /*	noun, used as a prefix */
			n_suf, /*	noun, used as a suffix */
			n_t, /*	noun (temporal) (jisoumeishi) */
			neg, /*	negative (in a negative sentence, or with negative verb) */
			neg_v, /*	negative verb (when used with) */
			num, /*	numeric */
			pref, /*	prefix */
			prt, /*	particle */
			suf, /*	suffix */
			v1, /*	Ichidan verb */
			v5, /*	Godan verb (not completely classified) */
			v5aru, /*	Godan verb _ -aru special class */
			v5b, /*	Godan verb with `bu' ending */
			v5g, /*	Godan verb with `gu' ending */
			v5k, /*	Godan verb with `ku' ending */
			v5k_s, /*	Godan verb - iku/yuku special class */
			v5m, /*	Godan verb with `mu' ending */
			v5n, /*	Godan verb with `nu' ending */
			v5r, /*	Godan verb with `ru' ending */
			v5r_i, /*	Godan verb with `ru' ending (irregular verb) */
			v5s, /*	Godan verb with `su' ending */
			v5t, /*	Godan verb with `tsu' ending */
			v5u, /*	Godan verb with `u' ending */
			v5u_s, /*	Godan verb with `u' ending (special class) */
			v5uru, /*	Godan verb _ uru old class verb (old form of Eru) */
			vi, /*	intransitive verb */
			vk, /*	kuru verb _ special class */
			vs, /*	noun or participle which takes the aux. verb suru */
			vs_i, /*	suru verb - irregular */
			vs_s, /*	suru verb - special class */
			vt, /*	transitive verb */
			vz, /*	zuru verb _ (alternative form of -jiru verbs) */

			NUMTYPES /* works because the first element is defined as 0 */
		};

		/**
		 *  Multihash from pretty names like "Verb" to the enums v1, v5r, etc.
		 */
		static const QMultiHash<QString, WordType> &WordTypesPretty();

		/**
		 * Hash from abbreviations in the dictionary to the corresponding
		 * enums
		 */
		static const QHash<QString, WordType> &WordTypes();

		/**
		  * These hashes are constructed in the first call to the above static
		  * methods */
		static QMultiHash<QString, WordType> *wordTypesPretty;
		static QHash<QString, WordType> *wordTypes;

		/* A list of the wordtypes the word belongs to */
		QList<WordType> types;

		/* Is this word marked as "common" in EDICT */
		bool common;
	};

#endif

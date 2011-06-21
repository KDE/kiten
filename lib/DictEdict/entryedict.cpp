/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "entryedict.h"

#include "dictfileedict.h"

#include <KDebug>
#include <KLocalizedString>

#define QSTRINGLISTCHECK(x) (x==NULL?QStringList():*x)

EntryEdict::EntryEdict( const QString &dict )
: Entry( dict )
{
}

EntryEdict::EntryEdict( const QString &dict, const QString &entry )
: Entry( dict )
{
  loadEntry( entry );
}

Entry* EntryEdict::clone() const
{
  return new EntryEdict( *this );
}

QString EntryEdict::common() const
{
  if ( getExtendedInfoItem( QString( "common" ) ) == "1" )
  {
    return "<span>Common</span>";
  }
  else
  {
    return QString();
  }
}

/**
 * Regenerate a QString like the one we got in loadEntry()
 */
QString EntryEdict::dumpEntry() const
{
  return Word
         + (  (Readings.count() == 0 ) ? " " : " [" + Readings.first() + "] " )
         + '/' + Meanings.join( "/" ) + '/';
}

QString EntryEdict::HTMLWord() const
{
  return "<span class=\"Word\">"
         + ( Word.isEmpty() ? kanjiLinkify( Meanings.first() ) : kanjiLinkify( Word ) )
         + "</span>";
}

/**
 * Makes a link out of each kanji in @param inString
 */
QString EntryEdict::kanjiLinkify( const QString &inString ) const
{
  QString outString;

  for( int i = 0; i < inString.length(); i++ )
  {
    if( isKanji( inString.at( i ) ) )
    {
      outString += makeLink( QString( inString.at( i ) ) );
    }
    else
    {
      outString += inString.at( i );
    }
  }

  return outString;
}

/**
 * Take a QString and load it into the Entry as appropriate
 * The format is basically: KANJI [KANA] /(general information) gloss/gloss/.../
 * Note that they can rudely place more (general information) in gloss's that are
 * not the first one.
 */
bool EntryEdict::loadEntry( const QString &entryLine )
{
  /* Set tempQString to be the reading and word portion of the entryLine */
  int endOfKanjiAndKanaSection = entryLine.indexOf( '/' );
  if( endOfKanjiAndKanaSection == -1 )
  {
    return false;
  }
  QString tempQString = entryLine.left( endOfKanjiAndKanaSection );
  /* The actual Word is the beginning of the line */
  int endOfKanji = tempQString.indexOf( ' ' );
  if( endOfKanji == -1 )
  {
    return false;
  }
  Word = tempQString.left( endOfKanji );

  /* The Reading is either Word or encased in '[' */
  Readings.clear();
  int startOfReading = tempQString.indexOf( '[' );
  if( startOfReading != -1 )  // This field is optional for EDICT (and kiten)
  {
    Readings.append( tempQString.left( tempQString.lastIndexOf( ']' ) ).mid( startOfReading + 1 ) );
  }
  /* TODO: use this code or not?
  * app does not handle only reading and no word entries
  * very well so far
  else
  {
    Readings.append(Word);
    Word.clear();
  }
  */

  /* set Meanings to be all of the meanings in the definition */
  QString remainingLine = entryLine.mid( endOfKanjiAndKanaSection );
  //Trim to last '/'
  remainingLine = remainingLine.left( remainingLine.lastIndexOf( '/' ) );
  Meanings = remainingLine.split( '/', QString::SkipEmptyParts );

  if( Meanings.size() == 0 )
  {
    return false;
  }

  if( Meanings.last() == "(P)" )
  {
    ExtendedInfo[ QString( "common" ) ] = "1";
    Meanings.removeLast();
  }

  QString firstWord = Meanings.first();
  QStringList stringTypes;

  //Pulls the various types out
  //TODO: Remove them from the original string
  for ( int i = firstWord.indexOf( "(" );
        i != -1;
        i = firstWord.indexOf( "(", i + 1 ) )
  {
    QString parantheses = firstWord.mid( i + 1, firstWord.indexOf( ")", i ) - i - 1 );
    stringTypes += parantheses.split( ',' );
  }

  Types = stringTypes;

  foreach( const QString &str, stringTypes )
  {
    if( EdictFormatting::PartsOfSpeech.contains( str ) )
    {
      m_typeList += str;
    }
    else if( EdictFormatting::FieldOfApplication.contains( str ) )
    {
      ExtendedInfo[ "field" ] = str;
    }
    else if( EdictFormatting::MiscMarkings.contains( str ) )
    {
      m_miscMarkings += str;
    }
  }

  return true;
}

/**
 * Returns a HTML version of an Entry
 */
QString EntryEdict::toHTML() const
{
  QString result = "<div class=\"EDICT\">";
  bool isCommon = ( getExtendedInfoItem( QString( "common" ) ) == "1" );
  if( isCommon )
  {
    result += "<div class=\"Common\">";
  }

  foreach( const QString &field, QSTRINGLISTCHECK( DictFileEdict::displayFields ) )
  {
    if( field == "--NewLine--" )		 result += "<br>";
    else if( field == "Word/Kanji" ) result += HTMLWord()+' ';
    else if( field == "Meaning" )		 result += HTMLMeanings()+' ';
    else if( field == "Reading" )		 result += HTMLReadings()+' ';
    else if( field == "C" )			     result += common();
    else kDebug() << "Unknown field: " << field;
  }
  if( isCommon )
  {
    result += "</div>";
  }

  result += "</div>";
  return result;
}

#ifdef KITEN_EDICTFORMATTING

namespace EdictFormatting
{
/**
 * The basic idea of this function is to provide a mapping from possible entry types to
 * possible things the user could enter. Then our code for the matching entry can simply
 * use this mapping to determine if a given entry could be understood to match the user's input.
 *
 * There are two basic approaches we could take:
 *   Convert the user's entry into a list of types, see if the Entry type matches any of
 *           the conversions from this list (the list comparisons will be MANY enums).
 *   Convert our Entry types to a list of acceptable string aliases. Then compare the
 *           user's input to this list (the list will be a relatively small list of strings).
 *
 * My gut instinct is that the first case (comparison of a largish list of ints) will be
 * faster, and so that's the one that's implemented here.
 *
 * The following are the minimum list of case-insensitive aliases that the user could enter:
 *   noun
 *   verb:
 *     ichidan
 *     godan
 *   adjective
 *   adverb
 *   particle
 *
 * Note that our File Parser will also expand to general cases, if not included already:
 * For Example: v5aru -> v5aru,v5 (so that a search for "godan" will find it)
 * Also note that the basic edict dictionary does not separate ikeiyoushi out from the
 * category "adj", so further breakdown of the "adjective" type would be misleading.
 */

  // Forward declarations of our functions to be used.
  QMultiHash<QString, QString> createPartOfSpeechCategories();
  QSet<QString> createPartsOfSpeech();
  QSet<QString> createMiscMarkings();
  QSet<QString> createFieldOfApplication();
 
  // Private variables.
  QString noun      = QString( i18nc( "This must be a single word", "Noun" ) );
  QString verb      = QString( i18nc( "This must be a single word", "Verb" ) );
  QString adjective = QString( i18nc( "This must be a single word", "Adjective" ) );
  QString adverb    = QString( i18nc( "This must be a single word", "Adverb" ) );
  QString particle  = QString( i18nc( "This must be a single word", "Particle" ) );
  QString ichidanVerb   = QString( i18nc( "This is a technical japanese linguist's term... and probably should not be translated(except possibly in far-eastern languages), this must be a single word", "Ichidan" ) );
  QString godanVerb     = QString( i18nc( "This is a technical japanese linguist's term... and probably should not be translated, this must be a single word", "Godan" ) );



  // Define our public variables.
  QMultiHash<QString, QString> PartOfSpeechCategories = createPartOfSpeechCategories();
  QSet<QString> PartsOfSpeech      = createPartsOfSpeech();
  QSet<QString> MiscMarkings       = createMiscMarkings();
  QSet<QString> FieldOfApplication = createFieldOfApplication();

  // PartOfSpeechCategories needs to has some values before this line.
  QStringList Nouns = PartOfSpeechCategories.values( noun );
  QStringList Verbs = PartOfSpeechCategories.values( verb );  
  QStringList Adjectives = PartOfSpeechCategories.values( adjective ); 
  QStringList Adverbs = PartOfSpeechCategories.values( adverb );
  QStringList IchidanVerbs = PartOfSpeechCategories.values( ichidanVerb );
  QStringList GodanVerbs = PartOfSpeechCategories.values( godanVerb );
  QStringList Particles = PartOfSpeechCategories.values( particle );



  QMultiHash<QString, QString> createPartOfSpeechCategories()
  { 
    QMultiHash<QString, QString> categories;

    //Nouns
    categories.insert( noun, "n" );
    categories.insert( noun, "n-adv" );
    categories.insert( noun, "n-pref" );
    categories.insert( noun, "n-suf" );
    categories.insert( noun, "n-t" );
    categories.insert( noun, "adv_n" );

    //Ichidan Verbs
    categories.insert( verb,    "v1" );
    categories.insert( ichidanVerb, "v1" );

    //Godan Verbs
    categories.insert( verb,  "v5" );
    categories.insert( verb,  "v5aru" );
    categories.insert( verb,  "v5b" );
    categories.insert( verb,  "v5g" );
    categories.insert( verb,  "v5k" );
    categories.insert( verb,  "v5k_s" );
    categories.insert( verb,  "v5m" );
    categories.insert( verb,  "v5n" );
    categories.insert( verb,  "v5r" );
    categories.insert( verb,  "v5r_i" );
    categories.insert( verb,  "v5s" );
    categories.insert( verb,  "v5t" );
    categories.insert( verb,  "v5u" );
    categories.insert( verb,  "v5u_s" );
    categories.insert( verb,  "v5uru" );
    categories.insert( godanVerb, "v5" );
    categories.insert( godanVerb, "v5aru" );
    categories.insert( godanVerb, "v5b" );
    categories.insert( godanVerb, "v5g" );
    categories.insert( godanVerb, "v5k" );
    categories.insert( godanVerb, "v5k_s" );
    categories.insert( godanVerb, "v5m" );
    categories.insert( godanVerb, "v5n" );
    categories.insert( godanVerb, "v5r" );
    categories.insert( godanVerb, "v5r_i" );
    categories.insert( godanVerb, "v5s" );
    categories.insert( godanVerb, "v5t" );
    categories.insert( godanVerb, "v5u" );
    categories.insert( godanVerb, "v5u_s" );
    categories.insert( godanVerb, "v5uru" );

    //Other Verbs
    categories.insert( verb, "iv" );
    categories.insert( verb, "vi" );
    categories.insert( verb, "vk" );
    categories.insert( verb, "vs" );
    categories.insert( verb, "vs_i" );
    categories.insert( verb, "vs_s" );
    categories.insert( verb, "vt" );
    categories.insert( verb, "vz" );

    //Adjectives
    categories.insert( adjective, "adj" );
    categories.insert( adjective, "adj_na" );
    categories.insert( adjective, "adj_no" );
    categories.insert( adjective, "adj_pn" );
    categories.insert( adjective, "adj_t" );

    //Adverbs
    categories.insert( adverb, "adv" );
    categories.insert( adverb, "adv_n" );
    categories.insert( adverb, "adv_to" );

    //Particle
    categories.insert( particle, "prt" );

    return categories;
  }

  QSet<QString> createPartsOfSpeech()
  {
    QSet<QString> category;

    category << "n"     << "n-adv"  << "n-pref" << "n-suf"  << "n-t"  << "adv_n"
             << "v1"    << "v1"     << "v5"     << "v5aru"  << "v5b"  << "v5g"  << "v5k"   << "v5k_s"
             << "v5m"   << "v5n"    << "v5r"    << "v5r_i"  << "v5s"  << "v5t"  << "v5u"   << "v5u_s"
             << "v5uru" << "v5"     << "v5aru"  << "v5b"    << "v5g"  << "v5k"  << "v5k_s" << "v5m"
             << "v5n"   << "v5r"    << "v5r_i"  << "v5s"    << "v5t"  << "v5u"  << "v5u_s" << "v5uru"
             << "iv"    << "vi"     << "vk"     << "vs"     << "vs_i" << "vs_s" << "vt"    << "vz"
             << "adj"   << "adj_na" << "adj_no" << "adj_pn" << "adj_t"
             << "adv"   << "adv_n"  << "adv_to"
             << "prt";

    return category;
  }

  QSet<QString> createFieldOfApplication()
  {
    QSet<QString> category;

    //Field of Application terms
    category << "Buddh" << "MA"   << "comp" << "food" << "geom"
             << "ling"  << "math" << "mil"  << "physics";

    return category;
  }

  QSet<QString> createMiscMarkings()
  {
    QSet<QString> category;

    //Miscellaneous Markings (in EDICT terms)
    category << "X"    << "abbr" << "arch" << "ateji"   << "chn"   << "col" << "derog"
             << "eK"   << "ek"   << "fam"  << "fem"     << "gikun" << "hon" << "hum" << "iK"   << "id"
             << "io"   << "m-sl" << "male" << "male-sl" << "ng"    << "oK"  << "obs" << "obsc" << "ok"
             << "poet" << "pol"  << "rare" << "sens"    << "sl"    << "uK"  << "uk"  << "vulg";

    return category;
  }
}

#endif

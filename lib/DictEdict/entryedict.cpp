/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
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

QString EntryEdict::getTypes() const
{
  return m_types.join( outputListDelimiter );
}

QStringList EntryEdict::getTypesList() const
{
  return m_types;
}

bool EntryEdict::isAdjective() const
{
  foreach( const QString &type, EdictFormatting::Adjectives )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isAdverb() const
{
  foreach( const QString &type, EdictFormatting::Adverbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isExpression() const
{
  foreach( const QString &type, EdictFormatting::Expressions )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isFukisokuVerb() const
{
  foreach( const QString &type, EdictFormatting::FukisokuVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isGodanVerb() const
{
  foreach( const QString &type, EdictFormatting::GodanVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isIchidanVerb() const
{
  foreach( const QString &type, EdictFormatting::IchidanVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isNoun() const
{
  foreach( const QString &type, EdictFormatting::Nouns )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isParticle() const
{
  return m_types.contains( EdictFormatting::Particle );
}

bool EntryEdict::isPrefix() const
{
  foreach( const QString &type, EdictFormatting::Prefix )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isSuffix() const
{
  foreach( const QString &type, EdictFormatting::Suffix )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isVerb() const
{
  return isFukisokuVerb() || isGodanVerb() || isIchidanVerb();
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

  foreach( const QString &str, stringTypes )
  {
    if( EdictFormatting::PartsOfSpeech.contains( str ) )
    {
      m_types += str;
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

bool EntryEdict::matchesWordType( const DictQuery &query ) const
{
  if( ! query.isEmpty() )
  {
    if( query.getMatchWordType() == DictQuery::Verb
        && isVerb() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Noun
        && isNoun() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Adjective
        && isAdjective() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Adverb
        && isAdverb() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Expression
        && isExpression() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Prefix
        && isPrefix() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Suffix
        && isSuffix() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Any )
    {
      return true;
    }
  }

  return false;
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
  QStringList   createNounsList();
  QStringList   createVerbsList();
  QStringList   createExpressionsList();
  QStringList   createPrefixesList();
  QStringList   createSuffixesList();
 
  // Private variables.
  QString noun      = QString( i18nc( "This must be a single word", "Noun" ) );
  QString verb      = QString( i18nc( "This must be a single word", "Verb" ) );
  QString adjective = QString( i18nc( "This must be a single word", "Adjective" ) );
  QString adverb    = QString( i18nc( "This must be a single word", "Adverb" ) );
  QString particle  = QString( i18nc( "This must be a single word", "Particle" ) );
  QString ichidanVerb   = QString( i18nc( "This is a technical japanese linguist's term... and probably should not be translated (except possibly in far-eastern languages), this must be a single word", "Ichidan" ) );
  QString godanVerb     = QString( i18nc( "This is a technical japanese linguist's term... and probably should not be translated, this must be a single word", "Godan" ) );
  QString fukisokuVerb  = QString( i18nc( "This is a technical japanese linguist's term... and probably should not be translated, this must be a single word", "Fukisoku" ) );
  QString expression = QString( i18n( "Expression" ) );
  QString idiomaticExpression = QString( i18n( "Idiomatic expression" ) );
  QString prefix = QString( i18n( "Prefix" ) );
  QString suffix = QString( i18n( "Suffix" ) );
  QString nounPrefix = QString( i18n( "Noun (used as a prefix)" ) );
  QString nounSuffix = QString( i18n( "Noun (used as a suffix)" ) );


  // Define our public variables.
  QMultiHash<QString, QString> PartOfSpeechCategories = createPartOfSpeechCategories();
  QSet<QString> PartsOfSpeech      = createPartsOfSpeech();
  QSet<QString> MiscMarkings       = createMiscMarkings();
  QSet<QString> FieldOfApplication = createFieldOfApplication();

  // PartOfSpeechCategories needs to has some values before this line.
  QStringList Nouns         = createNounsList();
  QStringList Adjectives    = PartOfSpeechCategories.values( adjective );
  QStringList Adverbs       = PartOfSpeechCategories.values( adverb );
  QStringList IchidanVerbs  = PartOfSpeechCategories.values( ichidanVerb );
  QStringList GodanVerbs    = PartOfSpeechCategories.values( godanVerb );
  QStringList FukisokuVerbs = PartOfSpeechCategories.values( fukisokuVerb );
  QStringList Verbs         = createVerbsList();
  QStringList Expressions   = createExpressionsList();
  QStringList Prefix        = createPrefixesList();
  QStringList Suffix        = createSuffixesList();
  QString     Particle      = PartOfSpeechCategories.value( particle );



  QStringList createNounsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( noun ) );
    list.append( PartOfSpeechCategories.values( nounPrefix ) );
    list.append( PartOfSpeechCategories.values( nounSuffix ) );
    return list;
  }

  QStringList createVerbsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( verb ) );
    list.append( IchidanVerbs );
    list.append( GodanVerbs );
    list.append( FukisokuVerbs );
    return list;
  }

  QStringList createExpressionsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( expression ) );
    list.append( PartOfSpeechCategories.values( idiomaticExpression ) );
    return list;
  }

  QStringList createPrefixesList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( prefix ) );
    list.append( PartOfSpeechCategories.values( nounPrefix ) );
    return list;
  }

  QStringList createSuffixesList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( suffix ) );
    list.append( PartOfSpeechCategories.values( nounSuffix ) );
    return list;
  }

  QMultiHash<QString, QString> createPartOfSpeechCategories()
  { 
    QMultiHash<QString, QString> categories;

    //Nouns
    categories.insert( noun, "n" );
    categories.insert( noun, "n-adv" );
    categories.insert( noun, "n-t" );
    categories.insert( noun, "adv-n" );

    //Noun (used as a prefix)
    categories.insert( nounPrefix, "n-pref" );

    //Noun (used as a suffix)
    categories.insert( nounSuffix, "n-suf" );

    //Ichidan Verbs
    categories.insert( ichidanVerb, "v1" );
    categories.insert( ichidanVerb, "vz" );

    //Godan Verbs
    categories.insert( godanVerb, "v5" );
    categories.insert( godanVerb, "v5aru" );
    categories.insert( godanVerb, "v5b" );
    categories.insert( godanVerb, "v5g" );
    categories.insert( godanVerb, "v5k" );
    categories.insert( godanVerb, "v5k-s" );
    categories.insert( godanVerb, "v5m" );
    categories.insert( godanVerb, "v5n" );
    categories.insert( godanVerb, "v5r" );
    categories.insert( godanVerb, "v5r-i" );
    categories.insert( godanVerb, "v5s" );
    categories.insert( godanVerb, "v5t" );
    categories.insert( godanVerb, "v5u" );
    categories.insert( godanVerb, "v5u-s" );
    categories.insert( godanVerb, "v5uru" );
    categories.insert( godanVerb, "v5z" );

    //Fukisoku verbs
    categories.insert( fukisokuVerb, "iv" );
    categories.insert( fukisokuVerb, "vk" );
    categories.insert( fukisokuVerb, "vn" );
    categories.insert( fukisokuVerb, "vs-i" );
    categories.insert( fukisokuVerb, "vs-s" );

    //Other Verbs
    categories.insert( verb, "vi" );
    categories.insert( verb, "vs" );
    categories.insert( verb, "vt" );
    categories.insert( verb, "aux-v" );

    //Adjectives
    categories.insert( adjective, "adj-i" );
    categories.insert( adjective, "adj-na" );
    categories.insert( adjective, "adj-no" );
    categories.insert( adjective, "adj-pn" );
    categories.insert( adjective, "adj-t" );
    categories.insert( adjective, "adj-f" );
    categories.insert( adjective, "adj" );
    categories.insert( adjective, "aux-adj" );

    //Adverbs
    categories.insert( adverb, "adv" );
    categories.insert( adverb, "adv-n" );
    categories.insert( adverb, "adv-to" );

    //Particle
    categories.insert( particle, "prt" );

    //Expression
    categories.insert( expression, "exp" );

    //Idiomatic expression
    categories.insert( idiomaticExpression, "id" );

    //Prefix
    categories.insert( prefix, "pref" );

    //Suffix
    categories.insert( suffix, "suf" );

    return categories;
  }

  QSet<QString> createPartsOfSpeech()
  {
    QSet<QString> category;

    category << "adj-i" << "adj-na" << "adj-no" << "adj-pn" << "adj-t" << "adj-f"
             << "adj" << "adv" << "adv-n" << "adv-to" << "aux" << "aux-v"
             << "aux-adj" << "conj" << "ctr" << "exp" << "id" << "int"
             << "iv" << "n" << "n-adv" << "n-pref" << "n-suf" << "n-t"
             << "num" << "pn" << "pref" << "prt" << "suf" << "v1"
             << "v5" << "v5aru" << "v5b" << "v5g" << "v5k" << "v5k-s"
             << "v5m" << "v5n" << "v5r" << "v5r-i" <<  "v5s" << "v5t"
             << "v5u" << "v5u-s" << "v5uru" << "v5z" << "vz" << "vi"
             << "vk" << "vn" << "vs" << "vs-i" << "vs-s" << "vt";

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

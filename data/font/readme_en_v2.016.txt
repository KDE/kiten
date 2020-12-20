INTRODUCTION
This font provides an easy way to view stroke order diagrams for over 6350
kanji, 183 kana symbols, the Latin characters and quite a few other symbols.
I have also used it as a dumping ground for my own character creation doodles.

My hope is that this font will assist people who are learning kanji. I
also hope it will help teachers of Japanese in the preparation of
classroom material. Beware that Japanese stroke order can differ from the
stroke order used in other languages that use Chinese characters.

HOW TO INSTALL AND USE THIS FONT
Install this font as you would any other TrueType font. In the parts of your
document where you want the kanji to be annotated with stroke order numbers
simply set your document's font to KanjiStrokeOrders. You will need to set
the size of the font to be large to allow the stroke order numbers to
show up: 100pt seems to be the minimum usable size.

For what it's worth in this context, KanjiStrokeOrders provides small capitals.
Access them using the smcp tag. For example, in XeTeX use

\font\smallcaps="KanjiStrokeOrders:+smcp" at 10pt

I have had one report from the field of Mac OS X 10.4.11 complaining about
missing OpenType data, although this does not happen with OS X 10.5. I am
told that KSOF works fine on OS X 10.4.11 if you ignore the warning, so it's
nothing to worry about. Thanks are due to Phil for reporting this.

LICENCE
This font may be freely distributed under the terms of the BSD-style licence
included in LICENSES/BSD-3-Clause.txt.

The kanji stroke order diagrams remain under the copyright of Ulrich Apel
and the AAAA and Wadoku projects. If you find this font useful then please
consider making a donation to the Wadoku project by pointing your Web
browser at http://www.wadoku.de and clicking on "Spenden".

The small number of characters in this font that were inexpertly knocked up
by the author.

INTEGRATION WITH JWPCE
This font enables you to view kanji stroke orders in JWPce. Set the "Big
Kanji Font" to KanjiStrokeOrders in the Options dialog. Some kanji stroke order
diagrams might show a different stroke count to that reported by JWPce in the
same kanji information window. This is because some kanji have more than one
valid stroke count.

TECHNICAL INFORMATION
I created this font by using the Batik library to convert the AAAA and
Wadoku projects' SVG files into high-resolution PNG images. I turned
these into a font using Fontforge. I then filled in some missing characters
using another font I have created called Choumei. Previously I filled in the
gaps with the public domain font Tuffy but I no longer use those glyphs.

EXAMPLES
Included in this archive are two contrived examples of the use of this font
with popular office software. They are in PDF format.

CORRECTIONS
As the changelog below shows, users have found quite a few errors in the
stroke orders diagrams. There are probably more.  If you find any errors please
let me know by email: mail |at| nihilist (dot) org (dot) uk. You can expect
your changes to appear within a few weeks and I can email you to let you know
that the new version is available. When reporting an error please tell me what
you think is wrong with the kanji and provide a link to a reference if
possible.

While working on this font I was interested to find that references generally
considered to be authoratitive sometimes disagree on stroke order and even
stroke count. For example, U+6E1B and U+53CE vary in stroke order and Kanjidic
lists hundreds of kanji with multiple valid stroke counts. Appendix E of the
Kanjidic documentation discusses stroke counts in detail. If you are not sure
whether a character needs correcting please let me know anyway so I can
investigate.

OPENTYPE FEATURES
+smcp Smallcaps for a-z.
+utop Thomas More's Utopian characters plus the product integral symbol.
      The lowercase Latin characters map to their Utopian counterparts and z maps to
      the product integral symbol. Note that Thomas More wrote _Utopia_ in
      Latin and, as such, this range of characters does not include the letters
      j, u, w or z (but z maps to the product integral symbol).
+spec The Sinclair ZX spectrum block graphics characters. These originally
      occupied the range 0x80 to 0x8F in the Sinclair ZX Spectrum character set.
      The symbol @ maps to the blank block and the letters A to P correspond
      to the non-blank block graphics. Why @? Because in the ZX Spectrum
      character set @ was 0x40 and A-P were 0x41-0x4F.

CHANGELOG
2011/03/29  2.016 Added Thomas More's Utopian characters (see +utop above) at
                  U+F800 to U+F818. Added Product Integral symbol (see +utop
                  above) at U+F919. Added Sinclair ZX Spectrum block graphics
                  at U+F7F0 to U+F7FF. Added radicals at U+91D2, U+2ECA, U+8A01,
                  U+2EB6, U+2EAA, U+2EB7, U+2EA7, U+2EBB, U+7529, U+9763,
                  U+2EBC. Improved readability of U+9751. Adding missing character
                  glyph at U+0000.
2010/10/05  2.015 Removed one stroke from U+6357, notionally in compliance with
                  JIS X 0213 and the Unicode reference table but actually
                  because someone suggested it. There are many other characters
                  in KSOF that don't match JIS X 0213 but I don't intend to
                  change those; I made an exception for U+6357 because it's slated
                  to become a Joyo kanji. Corrected U+6E90, U+646F, U+7D68,
                  U+8650, U+6F5F, U+6557, U+8FB6 (controversial: it is unclear
                  whether this radical has 3 or 4 strokes). Added the following
                  extra radicals thanks to the generous help of Durand D'souza:
                  U+722B, U+2E99 (an unusual radical), U+6236, U+4E5A, U+725C,
                  U+2F21, U+8980, U+72AD, U+9751, U+6C35, U+9578, U+2EAE,
                  U+8FB5, U+5C23, U+5DDC, U+4E61, U+6237, U+6B7A, U+7CF9, U+8080.
                  Corrected U+718A. Added U+FA35 (variant of U+5351). Corrected
                  U+62C9. Shortened first stroke of U+308A. Shortened U+30FC.
2009/10/04  2.014 Added missing stroke order numbers to U+30EE, U+4EDD, U+9FA0.
2009/08/30  2.013 Changed U+53CE to 4-stroke version. Updated U+5099 with more
                  common version of cliff radical. Clarified U+6163. Corrected
                  U+7CA5, U+8258, U+5396
2008/12/29  2.012 Corrected U+544E, U+9D28, U+6388, U+5D8B
2008/10/01  2.011 Corrected U+9280, U+8607, U+6089, U+6955, U+7566, U+79A6,
                  U+7B08, U+9803, U+86E4, U+88DF. Clarified U+8511.
2008/07/17  2.010 Corrected U+74DC, U+9CE9, U+79BD, U+8DA8, U+82BB, U+50FB,
                  U+4FB6, U+586B, U+5835, U+5C3B, U+63F7, U+64E2. Clarified
                  U+50AD. Thanks once again to Scott Krogh.
2008/01/02: 2.009 Corrected U+76FE, U+8846, U+885B, U+5171. Thanks to Aaron
                  and Scott Krogh for spotting these.
2008/04/04: 2.008 Corrected U+5440, U+5BDE, U+6357, U+6E76, U+74DC, U+7C6C,
                  U+6A8E, U+6A23, U+9244, U+85AC, U+7CDE. Merged in the
                  Choumei v1.10 glyphs, thus providing KanjiStrokeOrders with
                  all the JIS 0208 characters. Added support for small caps.
2008/03/03: 2.007 Corrected U+641C, U+854B, U+6F80, U+83DF, U+5C65, U+65C6,
                  U+6F01, U+750D, U+750E, U+750C, U+7504, U+7503, U+74E9,
                  U+821B, U+74F1, U+5178, U+82E1, U+5BC3, U+9B42, U+6F11,
                  U+5E43, U+91D0, U+74F8, U+74EE, U+74F0, U+74F7, U+7513,
                  U+7505, U+7E4A, U+7515, U+6975, U+64D2, U+79BD, U+6F13,
                  U+6A8E, U+9B51, U+7DFB. Removed glyph list and moved &, ', (,
                  ) and * into their correct encoding slots. Correction from
                  Aaron: U+6E1B.
2008/02/29: 2.006 Corrections from Aaron: U+5EA7, U+51F8. Fixed
                  appearance of many characters and increased the overall
                  size of the glyphs.
2008/02/27: 2.005 More corrections from Scott Krogh: U+6570, U+6551, U+7B20,
                  U+4FC4, U+4F3C, U+74F6, U+629E, U+96EA, U+4E21, U+6E80,
                  U+5EFB, U+5EB6, U+534A, U+7554, U+754F, U+5224, U+5C04,
                  U+8DEF, U+96A3, U+9F62, U+5B9B, U+6028, U+990A, U+8A55.
                  Replaced Tuffy glyphs with those from Choumei, thus making
                  the font more consistent in appearence. Added a list of
                  glyphs provided by the font.
2008/01/14: 2.004 Fixed bugs in U+5BA2, U+843D, U+539F, U+9ED2, U+9B5A, U+5B89,
                  U+6728, U+837B, U+56E0, U+5EAB, U+78E8, U+61BE, U+6545,
                  U+790E, U+96F2, U+8328, U+596E, U+97FB, U+9054, U+5742,
                  U+70AD, U+5897 and U+66DC. Many thanks to the eagle-eyed
                  Scott Krogh for pointing out the errors.
2007/12/13: 2.003 Fixed bug with missing stroke number at U+5144 and adjusted
                  offset of U+4E2D.
2007/05/08: 2.002 Fixed bug whereby hiragana 'mo' was missing.
2007/01/08: 2.001 Improved the character 'n'. Corrected the character U+5E83.
                  Fixed the character spacing.
2006/12/28: 2.0   Greatly expanded with new stroke order diagrams to cover many
                  more kanji, the Latin alphabet, punctuation and some other
                  symbols. This used a fresh set of SVG data from Ulrich Apel.
2006/10/31: 1.01  Corrected the text strings embedded in the font.
2006/10/27: 1.0   Initial revision

ACKNOWLEDGEMENTS
Almost all the work for this font was done by the AAAA and Wadoku projects
under the supervision of Ulrich Apel. I just had the idea of turning the
SVG data into a font and did so. Thanks are due to Scott Krogh, Aaron, Paul
Blay and Jeremy Suurkivi for reporting some errors in the stroke order data.
I have fixed all these errors in the current version. Thanks also to Durand
D'souza for doing the background research on the Kanji radicals I added in
version 2.015.

Tim Eyre

mail |at| nihilist (dot) org (dot) uk

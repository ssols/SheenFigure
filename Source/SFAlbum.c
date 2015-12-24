/*
 * Copyright (C) 2015 Muhammad Tayyab Akram
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SFConfig.h>
#include <SFTypes.h>

#include <stdlib.h>
#include <string.h>

#include "SFAssert.h"
#include "SFAlbum.h"

SF_INTERNAL void SFAlbumInitialize(SFAlbumRef album, SFCodePoint *codePointArray, SFUInteger codePointCount)
{
	/* There must be some code points. */
	SFAssert(codePointArray != NULL && codePointCount > 0);

    album->codePointArray = codePointArray;
	album->mapArray = NULL;
    album->codePointCount = codePointCount;
    album->elementCount = 0;

    SFListInitialize(&album->_glyphs, sizeof(SFGlyphID));
    SFListInitialize(&album->_details, sizeof(SFGlyphDetail));
    SFListInitialize(&album->_positions, sizeof(SFPoint));
    SFListInitialize(&album->_advances, sizeof(SFInteger));
}

SF_INTERNAL void SFAlbumAllocateGlyphs(SFAlbumRef album)
{
	SFUInteger capacity = album->codePointCount * 2;

    SFListReserveRange(&album->_glyphs, 0, capacity);
    SFListReserveRange(&album->_details, 0, capacity);

	album->elementCount = album->codePointCount;
}

SF_INTERNAL void SFAlbumAllocatePositions(SFAlbumRef album)
{
    SFListReserveRange(&album->_positions, 0, album->elementCount);
    SFListReserveRange(&album->_advances, 0, album->elementCount);
}

SF_INTERNAL void SFAlbumAddGlyph(SFAlbumRef album, SFGlyphID glyph, SFUInteger association) {
    /* Initialize glyph along with its details. */
    SFAlbumSetGlyph(album, album->elementCount, glyph);
    SFAlbumSetTraits(album, album->elementCount, SFGlyphTraitNone);
    SFAlbumSetAssociation(album, album->elementCount, association);

    /* Increment element count. */
    album->elementCount++;
}

SF_INTERNAL void SFAlbumReserveGlyphs(SFAlbumRef album, SFUInteger index, SFUInteger count)
{
    SFListReserveRange(&album->_glyphs, index, count);
    SFListReserveRange(&album->_details, index, count);
}

static void SFValidateAlbumIndex(SFAlbumRef album, SFUInteger index)
{
    SFAssert(index < album->elementCount);
}

SF_INTERNAL SFGlyphID SFAlbumGetGlyph(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetVal(&album->_glyphs, index);
}

SF_INTERNAL void SFAlbumSetGlyph(SFAlbumRef album, SFUInteger index, SFGlyphID glyph)
{
    SFValidateAlbumIndex(album, index);
    SFListSetVal(&album->_glyphs, index, glyph);
}

SF_INTERNAL SFGlyphTrait SFAlbumGetTraits(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetRef(&album->_details, index)->traits;
}

SF_INTERNAL void SFAlbumSetTraits(SFAlbumRef album, SFUInteger index, SFGlyphTrait traits)
{
    SFValidateAlbumIndex(album, index);
    SFListGetRef(&album->_details, index)->traits = traits;
}

SF_INTERNAL SFUInteger SFAlbumGetAssociation(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetRef(&album->_details, index)->association;
}

SF_INTERNAL void SFAlbumSetAssociation(SFAlbumRef album, SFUInteger index, SFUInteger association)
{
    SFValidateAlbumIndex(album, index);
    SFListGetRef(&album->_details, index)->association = association;
}

SF_INTERNAL SFPoint SFAlbumGetPosition(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetVal(&album->_positions, index);
}

SF_INTERNAL void SFAlbumSetPosition(SFAlbumRef album, SFUInteger index, SFPoint position)
{
    SFValidateAlbumIndex(album, index);
    SFListSetVal(&album->_positions, index, position);
}

SF_INTERNAL SFInteger SFAlbumGetAdvance(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetVal(&album->_advances, index);
}

SF_INTERNAL void SFAlbumSetAdvance(SFAlbumRef album, SFUInteger index, SFInteger advance)
{
    SFValidateAlbumIndex(album, index);
    SFListSetVal(&album->_advances, index, advance);
}

SF_INTERNAL SFUInt16 SFAlbumGetOffset(SFAlbumRef album, SFUInteger index)
{
    SFValidateAlbumIndex(album, index);
    return SFListGetRef(&album->_details, index)->offset;
}

SF_INTERNAL void SFAlbumSetOffset(SFAlbumRef album, SFUInteger index, SFUInt16 offset)
{
    SFValidateAlbumIndex(album, index);
    SFListGetRef(&album->_details, index)->offset = offset;
}

SF_INTERNAL void SFAlbumFinalize(SFAlbumRef album) {
    SFListFinalize(&album->_glyphs);
    SFListFinalize(&album->_details);
    SFListFinalize(&album->_positions);
    SFListFinalize(&album->_advances);
}

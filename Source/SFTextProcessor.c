/*
 * Copyright (C) 2015-2018 Muhammad Tayyab Akram
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

#include "SFAlbum.h"
#include "SFArtist.h"
#include "SFAssert.h"
#include "SFBase.h"
#include "SFCommon.h"
#include "SFData.h"
#include "SFFont.h"
#include "SFGDEF.h"
#include "SFGSUB.h"
#include "SFPattern.h"

#include "SFGlyphDiscovery.h"
#include "SFGlyphManipulation.h"
#include "SFGlyphPositioning.h"
#include "SFGlyphSubstitution.h"
#include "SFTextProcessor.h"

static void _SFApplyFeatureRange(SFTextProcessorRef textProcessor, SFFeatureKind featureKind, SFUInteger index, SFUInteger count);

static SFLookupType _SFPrepareLookup(SFTextProcessorRef textProcessor, SFUInt16 lookupIndex, SFData *outLookupTable);
static void _SFApplySubtables(SFTextProcessorRef textProcessor, SFData lookupTable, SFLookupType lookupType);

SF_INTERNAL void SFTextProcessorInitialize(SFTextProcessorRef textProcessor, SFPatternRef pattern,
    SFAlbumRef album, SFTextDirection textDirection, SFTextMode textMode, SFBoolean zeroWidthMarks)
{
    SFData gdef;

    /* Pattern must NOT be null. */
    SFAssert(pattern != NULL);
    /* Album must NOT be null. */
    SFAssert(album != NULL);

    textProcessor->_pattern = pattern;
    textProcessor->_album = album;
    textProcessor->_glyphClassDef = NULL;
    textProcessor->_textDirection = textDirection;
    textProcessor->_textMode = textMode;
    textProcessor->_zeroWidthMarks = zeroWidthMarks;
    textProcessor->_containsZeroWidthCodepoints = SFFalse;

    gdef = pattern->font->tables.gdef;
    if (gdef) {
        textProcessor->_glyphClassDef = SFGDEF_GlyphClassDefTable(gdef);
    }

    SFLocatorInitialize(&textProcessor->_locator, album, gdef);
}

SF_INTERNAL void SFTextProcessorDiscoverGlyphs(SFTextProcessorRef textProcessor)
{
    SFAlbumBeginFilling(textProcessor->_album);
    _SFDiscoverGlyphs(textProcessor);
}

SF_INTERNAL void SFTextProcessorSubstituteGlyphs(SFTextProcessorRef textProcessor)
{
    SFAlbumRef album = textProcessor->_album;
    SFPatternRef pattern = textProcessor->_pattern;
    SFData gsubTable = pattern->font->tables.gsub;

    if (gsubTable) {
        SFOffset lookupListOffset = SFHeader_LookupListOffset(gsubTable);
        SFData lookupListTable = SFData_Subdata(gsubTable, lookupListOffset);

        textProcessor->_lookupList = lookupListTable;
        textProcessor->_lookupOperation = _SFApplySubstitutionSubtable;

        _SFApplyFeatureRange(textProcessor, SFFeatureKindSubstitution, 0, pattern->featureUnits.gsub);
    }

    SFAlbumEndFilling(album);
}

static void _SFHandleZeroWidthGlyphs(SFTextProcessorRef textProcessor)
{
    if (textProcessor->_containsZeroWidthCodepoints) {
        SFFontRef font = textProcessor->_pattern->font;
        SFAlbumRef album = textProcessor->_album;
        SFUInteger glyphCount = album->glyphCount;
        SFGlyphID spaceGlyph;
        SFUInteger index;

        spaceGlyph = SFFontGetGlyphIDForCodepoint(font, ' ');

        for (index = 0; index < glyphCount; index++) {
            SFGlyphTraits traits = SFAlbumGetAllTraits(album, index);
            if (traits & SFGlyphTraitZeroWidth) {
                SFAlbumSetGlyph(album, index, spaceGlyph);
                SFAlbumSetX(album, index, 0);
                SFAlbumSetY(album, index, 0);
                SFAlbumSetAdvance(album, index, 0);
            }
        }
    }
}

static void _SFMakeMarksZeroWidth(SFTextProcessorRef textProcessor)
{
    SFAlbumRef album = textProcessor->_album;
    SFUInteger count = SFAlbumGetGlyphCount(album);
    SFUInteger index;

    for (index = 0; index < count; index++) {
        SFGlyphTraits traits = SFAlbumGetAllTraits(album, index);
        if (traits & SFGlyphTraitMark) {
            SFAlbumSetAdvance(album, index, 0);
        }
    }
}

SF_INTERNAL void SFTextProcessorPositionGlyphs(SFTextProcessorRef textProcessor)
{
    SFAlbumRef album = textProcessor->_album;
    SFPatternRef pattern = textProcessor->_pattern;
    SFFontRef font = pattern->font;
    SFData gposTable = font->tables.gpos;
    SFUInteger glyphCount = album->glyphCount;
    SFUInteger index;

    SFAlbumBeginArranging(album);

    /* Set positions and advances of all glyphs. */
    for (index = 0; index < glyphCount; index++) {
        SFGlyphID glyphID = SFAlbumGetGlyph(album, index);
        SFGlyphTraits traits = SFAlbumGetAllTraits(album, index);
        SFAdvance advance = 0;

        /* Ignore placeholder glyphs. */
        if (traits != SFGlyphTraitPlaceholder) {
            advance = SFFontGetAdvanceForGlyph(font, SFFontLayoutHorizontal, glyphID);
        }

        SFAlbumSetX(album, index, 0);
        SFAlbumSetY(album, index, 0);
        SFAlbumSetAdvance(album, index, advance);
    }

    if (gposTable) {
        SFOffset lookupListOffset = SFHeader_LookupListOffset(gposTable);
        SFData lookupListTable = SFData_Subdata(gposTable, lookupListOffset);

        textProcessor->_lookupList = lookupListTable;
        textProcessor->_lookupOperation = _SFApplyPositioningSubtable;

        _SFApplyFeatureRange(textProcessor, SFFeatureKindPositioning, pattern->featureUnits.gsub, pattern->featureUnits.gpos);
        _SFHandleZeroWidthGlyphs(textProcessor);

        if (textProcessor->_zeroWidthMarks) {
            _SFMakeMarksZeroWidth(textProcessor);
        }

        _SFResolveAttachments(textProcessor);
    }

    SFAlbumEndArranging(album);
}

SF_INTERNAL void SFTextProcessorWrapUp(SFTextProcessorRef textProcessor)
{
    SFAlbumWrapUp(textProcessor->_album);
}

static void _SFApplyFeatureRange(SFTextProcessorRef textProcessor, SFFeatureKind featureKind, SFUInteger index, SFUInteger count)
{
    SFPatternRef pattern = textProcessor->_pattern;
    SFUInteger limit = index + count;
    SFBoolean reversable = (featureKind == SFFeatureKindSubstitution);

    for (; index < limit; index++) {
        SFFeatureUnitRef featureUnit = &pattern->featureUnits.items[index];
        SFUInt16 *lookupArray = featureUnit->lookupIndexes.items;
        SFUInteger lookupCount = featureUnit->lookupIndexes.count;
        SFUInteger lookupIndex;

        /* Apply all lookups of the feature unit. */
        for (lookupIndex = 0; lookupIndex < lookupCount; lookupIndex++) {
            SFAlbumRef album = textProcessor->_album;
            SFLocatorRef locator = &textProcessor->_locator;
            SFData lookupTable;
            SFLookupType lookupType;

            SFLocatorReset(locator, 0, album->glyphCount);
            SFLocatorSetFeatureMask(locator, featureUnit->featureMask);

            lookupType = _SFPrepareLookup(textProcessor, lookupArray[lookupIndex], &lookupTable);

            /* Apply current lookup on all glyphs. */
            if (!reversable || lookupType != SFLookupTypeReverseChainingContext) {
                while (SFLocatorMoveNext(locator)) {
                    _SFApplySubtables(textProcessor, lookupTable, lookupType);
                }
            } else {
                SFLocatorJumpTo(locator, album->glyphCount);

                while (SFLocatorMovePrevious(locator)) {
                    _SFApplySubtables(textProcessor, lookupTable, lookupType);
                }
            }
        }
    }
}

SF_PRIVATE void _SFApplyLookup(SFTextProcessorRef textProcessor, SFUInt16 lookupIndex)
{
    SFData lookupTable;
    SFLookupType lookupType;

    lookupType = _SFPrepareLookup(textProcessor, lookupIndex, &lookupTable);
    _SFApplySubtables(textProcessor, lookupTable, lookupType);
}

static SFLookupType _SFPrepareLookup(SFTextProcessorRef textProcessor, SFUInt16 lookupIndex, SFData *outLookupTable)
{
    SFData lookupListTable = textProcessor->_lookupList;
    SFData lookupTable;
    SFLookupType lookupType;
    SFLookupFlag lookupFlag;

    lookupTable = SFLookupList_LookupTable(lookupListTable, lookupIndex);
    lookupType = SFLookup_LookupType(lookupTable);
    lookupFlag = SFLookup_LookupFlag(lookupTable);

    SFLocatorSetLookupFlag(&textProcessor->_locator, lookupFlag);

    if (lookupFlag & SFLookupFlagUseMarkFilteringSet) {
        SFUInt16 subtableCount = SFLookup_SubtableCount(lookupTable);
        SFUInt16 markFilteringSet = SFLookup_MarkFilteringSet(lookupTable, subtableCount);

        SFLocatorSetMarkFilteringSet(&textProcessor->_locator, markFilteringSet);
    }

    *outLookupTable = lookupTable;
    return lookupType;
}

static void _SFApplySubtables(SFTextProcessorRef textProcessor, SFData lookupTable, SFLookupType lookupType)
{
    SFUInt16 subtableCount = SFLookup_SubtableCount(lookupTable);
    SFUInteger subtableIndex;

    /* Apply subtables in order until one of them performs substitution/positioning. */
    for (subtableIndex = 0; subtableIndex < subtableCount; subtableIndex++) {
        SFData subtable = SFLookup_SubtableData(lookupTable, subtableIndex);

        if (textProcessor->_lookupOperation(textProcessor, lookupType, subtable)) {
            /* A subtable has performed substitution/positioning, so break the loop. */
            break;
        }
    }
}

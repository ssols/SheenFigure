/*
 * Copyright (C) 2016-2018 Muhammad Tayyab Akram
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

#include <vector>

#include "OpenType/Builder.h"
#include "TextProcessorTester.h"

using namespace std;
using namespace SheenFigure::Tester;
using namespace SheenFigure::Tester::OpenType;

void TextProcessorTester::testSingleSubstitution()
{
    Builder builder;

    /* Test the first format. */
    {
        /* Test with unmatching glyph. */
        testSubstitution(builder.createSingleSubst({ 0 }, 0), { 1 }, { 1 });
        /* Test with zero delta. */
        testSubstitution(builder.createSingleSubst({ 1 }, 0), { 1 }, { 1 });
        /* Test with positive delta. */
        testSubstitution(builder.createSingleSubst({ 1 }, 99), { 1 }, { 100 });
        /* Test with negative delta. */
        testSubstitution(builder.createSingleSubst({ 100 }, -99), { 100 }, { 1 });
        /* Test with opposite delta. */
        testSubstitution(builder.createSingleSubst({ 1 }, -1), { 1 }, { 0 });
    }

    /* Test the second format. */
    {
        /* Test with unmatching glyph. */
        testSubstitution(builder.createSingleSubst({ {0, 0} }), { 1 }, { 1 });
        /* Test with zero glyph. */
        testSubstitution(builder.createSingleSubst({ {0, 1} }), { 0 }, { 1 });
        /* Test with zero substitution. */
        testSubstitution(builder.createSingleSubst({ {1, 0} }), { 1 }, { 0 });
        /* Test with same substitution. */
        testSubstitution(builder.createSingleSubst({ {1, 1} }), { 1 }, { 1 });
        /* Test with a different substitution. */
        testSubstitution(builder.createSingleSubst({ {1, 100} }), { 1 }, { 100 });
    }
}

void TextProcessorTester::testMultipleSubstitution()
{
    Builder builder;

    /* Test with unmatching glyph. */
    testSubstitution(builder.createMultipleSubst({ {0, { 1, 2, 3 }} }), { 1 }, { 1 });
    /* Test with no glyph. */
    testSubstitution(builder.createMultipleSubst({ {1, { }} }), { 1 }, { 1 });
    /* Test with zero glyph. */
    testSubstitution(builder.createMultipleSubst({ {0, { 1 }} }), { 0 }, { 1 });
    /* Test with zero substitution. */
    testSubstitution(builder.createMultipleSubst({ {1, { 0 }} }), { 1 }, { 0 });
    /* Test with same substitution. */
    testSubstitution(builder.createMultipleSubst({ {1, { 1 }} }), { 1 }, { 1 });
    /* Test with different single substitution. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100 }} }), { 1 }, { 100 });
    /* Test with different two substitutions. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100, 200 }} }), { 1 }, { 100, 200 });
    /* Test with different multiple substitutions. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100, 200, 300 }} }), { 1 }, { 100, 200, 300 });
    /* Test with multiple substitutions having input glyph at the start. */
    testSubstitution(builder.createMultipleSubst({ {1, { 1, 200, 300 }} }), { 1 }, { 1, 200, 300 });
    /* Test with multiple substitutions having input glyph at the middle. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100, 1, 300 }} }), { 1 }, { 100, 1, 300 });
    /* Test with multiple substitutions having input glyph at the end. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100, 200, 1 }} }), { 1 }, { 100, 200, 1 });
    /* Test with multiple substitutions having input glyph everywhere. */
    testSubstitution(builder.createMultipleSubst({ {1, { 1, 1, 1 }} }), { 1 }, { 1, 1, 1 });
    /* Test with multiple repeating substitutions. */
    testSubstitution(builder.createMultipleSubst({ {1, { 100, 100, 100 }} }), { 1 }, { 100, 100, 100 });
    /* Test with multiple zero substitutions. */
    testSubstitution(builder.createMultipleSubst({ {1, { 0, 0, 0 }} }), { 1 }, { 0, 0, 0 });
}

void TextProcessorTester::testAlternateSubstitution()
{
    Builder builder;

    /* Test with unmatching glyph. */
    testSubstitution(builder.createAlternateSubst({ {0, { 0 }} }), { 1 }, { 1 });
    /* Test with zero glyph. */
    testSubstitution(builder.createAlternateSubst({ {0, { 1 }} }), { 0 }, { 1 });
    /* Test with zero substitution. */
    testSubstitution(builder.createAlternateSubst({ {1, { 0 }} }), { 1 }, { 0 });
    /* Test with same substitution. */
    testSubstitution(builder.createAlternateSubst({ {1, { 1 }} }), { 1 }, { 1 });
    /* Test with a different substitution. */
    testSubstitution(builder.createAlternateSubst({ {1, { 100 }} }), { 1 }, { 100 });
    /* Test with an alternate at the start. */
    testSubstitution(builder.createAlternateSubst({ {1, { 100, 200, 300 }} }), { 1 }, { 100 }, { }, 1);
    /* Test with an alternate in the middle. */
    testSubstitution(builder.createAlternateSubst({ {1, { 100, 200, 300 }} }), { 1 }, { 200 }, { }, 2);
    /* Test with an alternate at the end. */
    testSubstitution(builder.createAlternateSubst({ {1, { 100, 200, 300 }} }), { 1 }, { 300 }, { }, 3);
}

void TextProcessorTester::testLigatureSubstitution()
{
    Builder builder;

    /* Test with unmatching glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2, 3 }, 0} }), { 1 }, { 1 });
    /* Test with zero glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 0 }, 1} }), { 0 }, { 1 });
    /* Test with zero substitution. */
    testSubstitution(builder.createLigatureSubst({ {{ 1 }, 0} }), { 1 }, { 0 });
    /* Test with same substitution. */
    testSubstitution(builder.createLigatureSubst({ {{ 1 }, 1} }), { 1 }, { 1 });
    /* Test with different substitution. */
    testSubstitution(builder.createLigatureSubst({ {{ 1 }, 100} }), { 1 }, { 100 });
    /* Test with two different glyphs. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2 }, 100} }), { 1, 2 }, { 100 });
    /* Test with multiple different glyphs. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2, 3 }, 100} }), { 1, 2, 3 }, { 100 });
    /* Test with multiple glyphs translating to first input glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2, 3 }, 1} }), { 1, 2, 3 }, { 1 });
    /* Test with multiple glyphs translating to middle input glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2, 3 }, 2} }), { 1, 2, 3 }, { 2 });
    /* Test with multiple glyphs translating to last input glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 2, 3 }, 3} }), { 1, 2, 3 }, { 3 });
    /* Test with multiple same glyphs translating to itself. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 1, 1 }, 1} }), { 1, 1, 1 }, { 1 });
    /* Test with multiple same glyphs translating to a different glyph. */
    testSubstitution(builder.createLigatureSubst({ {{ 1, 1, 1 }, 100} }), { 1, 1, 1 }, { 100 });
    /* Test with multiple zero glyphs. */
    testSubstitution(builder.createLigatureSubst({ {{ 0, 0, 0 }, 100} }), { 0, 0, 0 }, { 100 });
}

void TextProcessorTester::testReverseChainContextSubstitution()
{
    Builder builder;

    /* Test with unmatching input glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 } }, { { 31 } }),
                     { 21, 0, 31 }, { 21, 0, 31 });
    /* Test with unmatching first backtrack glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 }, { 22} , { 23 } }, { { 31 } }),
                     { 0, 22, 23, 1, 31 }, { 0, 22, 23, 1, 31 });
    /* Test with unmatching middle backtrack glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 }, { 22 }, { 23 } }, { { 31 } }),
                     { 21, 0, 23, 1, 31 }, { 21, 0, 23, 1, 31 });
    /* Test with unmatching last backtrack glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 }, { 22 }, { 23 } }, { { 31 } }),
                     { 21, 22, 0, 1, 31 }, { 21, 22, 0, 1, 31 });
    /* Test with unmatching first lookahead glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 } }, { { 31 }, { 32 }, { 33 } }),
                     { 21, 1, 0, 32, 33 }, { 21, 1, 0, 32, 33 });
    /* Test with unmatching middle lookahead glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 } }, { { 31 }, { 32 }, { 33 } }),
                     { 21, 1, 31, 0, 33 }, { 21, 1, 31, 0, 33 });
    /* Test with unmatching last lookahead glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 } }, { { 31 }, { 32 }, { 33 } }),
                     { 21, 1, 31, 32, 0 }, { 21, 1, 31, 32, 0 });
    /* Test by providing all matching glyphs. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { { 21 }, { 22 }, { 23 } }, { { 31 }, { 32 }, { 33 } }),
                     { 21, 22, 23, 1, 31, 32, 33 }, { 21, 22, 23, 11, 31, 32, 33 });
    /* Test by providing no backtrack and lookahead glyph. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11} }, { }, { }),
                     { 1 }, { 11 });
    /* Test by providing reverse dependent sequence. */
    testSubstitution(builder.createRevChainSubst(
                        { {1, 11}, {2, 12}, {3, 13} },
                        { { 1, 2, 21 } },
                        { { 12, 13, 31 } }),
                     { 21, 1, 2, 3, 31 }, { 21, 11, 12, 13, 31 });
}

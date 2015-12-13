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

#ifndef SF_KNOWLEDGE_INTERNAL_H
#define SF_KNOWLEDGE_INTERNAL_H

#include <SFConfig.h>
#include <SFFeature.h>
#include <SFScript.h>
#include <SFTypes.h>

#include "SFShapingEngine.h"

struct _SFKnowledge;
typedef struct _SFKnowledge SFKnowledge;
typedef SFKnowledge *SFKnowledgeRef;

/* Facade pattern. */

struct _SFKnowledge {
    SFScriptKnowledgeRef _seek;
};

SF_INTERNAL void SFKnowledgeInitialize(SFKnowledgeRef knowledge);
SF_INTERNAL SFBoolean SFKnowledgeSeekScript(SFKnowledgeRef knowledge, SFScript script);

SF_INTERNAL SFUInteger SFKnowledgeCountFeatures(SFKnowledgeRef knowledge);
SF_INTERNAL SFUInteger SFKnowledgeSeekFeature(SFKnowledgeRef knowledge, SFFeature feature);
SF_INTERNAL SFFeature SFKnowledgeGetFeatureAt(SFKnowledgeRef knowledge, SFUInteger index);

SF_INTERNAL SFUInteger SFKnowledgeCountGroups(SFKnowledgeRef knowledge);
SF_INTERNAL SFRange SFKnowledgeGetGroupAt(SFKnowledgeRef knowledge, SFUInteger index);

#endif

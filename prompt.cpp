// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "prompt.h"

#include "binder.h"

Prompt::Prompt(string prompt, QueryNamesForPrefix toQuery)
{
    prompt_ = prompt;
    toQuery_ = toQuery;
}

string Prompt::prompt() const {
    return prompt_;
}

QueryNamesForPrefix Prompt::toQuery() const {
    return toQuery_;
}
